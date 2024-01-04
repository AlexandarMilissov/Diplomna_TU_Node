#include "EspnowPeer.hpp"
#include <cstring>
#include <string>
#include <stdexcept>

void EspnowPeer::Send(const MacAddress address, const Payload payload)
{
    lowerLayer.Send(address, payload);
}

void EspnowPeer::SendBroadcast(const Payload payload)
{
    lowerLayer.SendBroadcast(payload);
}

void EspnowPeer::Receive(const Payload* header, const Payload* data)
{
    Payload message_rssi = Payload(*data);
    Payload message_data = Payload(message_rssi.GetSize() - sizeof(RSSI_Type));
    message_rssi >>= message_data;

    EspnowMessageType message_identifier;
    memcpy(&message_identifier, header->data, MessageTypeSize);
    RSSI_Type* message_rssi_value = (RSSI_Type*)(message_rssi.data);
    // Proccess the message
    try
    {
        switch (message_identifier)
        {
        case NOW_REQUEST:
        {
            EspnowMessageRequest RSSI_Message = EspnowMessageRequest(message_data);
            this->ReceiveMessage(RSSI_Message);
        }
        break;
        case NOW_CALCULATION:
        {
            EspnowMessageCalculation RSSI_Message = EspnowMessageCalculation(*message_rssi_value, message_data);
            this->ReceiveMessage(RSSI_Message);
        }
        break;
        case NOW_KEEP_ALIVE:
        {
            EspnowMessageKeepAlive RSSI_Message = EspnowMessageKeepAlive(message_data);
            this->ReceiveMessage(RSSI_Message);
        }
        break;
        case NOW_ACKNOWLEDGE:
        {
            EspnowMessageAcknowledge RSSI_Message = EspnowMessageAcknowledge(message_data);
            this->ReceiveMessage(RSSI_Message);
        }
        break;
        default:
            break;
        }
    }
    catch(const std::invalid_argument& e)
    {
        logManager.Log(E, "EspnowPeer", "ESP_ERR_INVALID_ARG: %s\n", e.what());
    }
}

void EspnowPeer::SendSubscriptionRequest()
{
    if(false == acknowledgeRequired)
    {
        return;
    }
    EspnowMessageRequest RSSI_Message = EspnowMessageRequest(areWeSubscribedToPeer);
    Payload header(sourceAddress);
    Send(header, RSSI_Message.GetPayload());
}

std::string EspnowPeer::GetMonitorData()
{
    std::string peerLog = sourceAddress.ToString() + "\n";

    peerLog += distance.GetMonitorData();
    return peerLog;
}

EspnowPeer::EspnowPeer(
    IMessageable& messageable,
    IEspnowController& espnowController,
    LogManager& logManager,
    const MacAddress sourceAddress
    ) :
    peerLife(peerBeginningLife),
    lowerLayer(messageable),
    espnowController(espnowController),
    logManager(logManager),
    sourceAddress(sourceAddress)
{
    if(false) // TODO: Validate
    {
        throw "Invalid input in constructor!\n";
    }
    distance = Distance();
}

EspnowPeer::~EspnowPeer()
{
    if(isPeerSubscribedToUs)
    {
        espnowController.Unsubscribe();
    }
    for(auto& s : openSeries)
    {
        delete s.series;
    }
}

bool EspnowPeer::IsCorrectAddress(const MacAddress src_addr)
{
    if(sourceAddress == src_addr)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void EspnowPeer::ReceiveMessage(EspnowMessageRequest       message)
{
    bool messageStatus;
    EspnowMessageAcknowledge* ackn = NULL;

    Enter_Critical_Spinlock(subscriptionStateLock);

    messageStatus = message.GetSubscriptionStatus();

    if(SUBSCRIBE == messageStatus)
    {
        if(!isPeerSubscribedToUs)
        {
            isPeerSubscribedToUs = true;
            espnowController.Subscribe();
        }
        ackn = new EspnowMessageAcknowledge(SUBSCRIBE);
    }
    else if(UNSUBSCRIBE == message.GetSubscriptionStatus())
    {
        if(isPeerSubscribedToUs)
        {
            isPeerSubscribedToUs = false;
            espnowController.Unsubscribe();
        }
        ackn = new EspnowMessageAcknowledge(UNSUBSCRIBE);
    }
    Exit_Critical_Spinlock(subscriptionStateLock);

    if(NULL != ackn)
    {
        Payload header(sourceAddress);
        Send(header, ackn->GetPayload());
        delete ackn;
    }

}

void EspnowPeer::ReceiveMessage(EspnowMessageCalculation   message)
{
    OpenSeries* series = NULL;

    Enter_Critical_Spinlock(calculationDataLock);

    for(auto& s : openSeries)
    {
        if(s.series->IsCorrectId(message.GetSeriesID()))
        {
            series = s.series;
            break;
        }
    }
    if(NULL == series)
    {
        series = new OpenSeries(message.GetSeriesID());
        SeriesLife sl;
        sl.life = seriesBeginningLife;
        sl.series = series;
        openSeries.push_back(sl);
    }
    series->AddValue(message.GetMessagePosition(), message.GetRSSI());

    Exit_Critical_Spinlock(calculationDataLock);
}

void EspnowPeer::ReceiveMessage(EspnowMessageKeepAlive    message)
{
    peerLife = peerBeginningLife;
}

void EspnowPeer::ReceiveMessage(EspnowMessageAcknowledge   message)
{
    if(message.GetStatus() == areWeSubscribedToPeer)
    {
        acknowledgeRequired = false;
    }
}

bool EspnowPeer::IsAlive()
{
    if(peerLife <= 0)
    {
        return false;
    }

    peerLife--;
    return true;
}

void EspnowPeer::Refresh()
{
    Enter_Critical_Spinlock(subscriptionStateLock);
    if(areWeSubscribedToPeer)
    {
        if(!distance.IsCalculationRequired())
        {
            areWeSubscribedToPeer = false;
            acknowledgeRequired = true;
        }
    }
    else
    {
        if(distance.IsCalculationRequired())
        {
            areWeSubscribedToPeer = true;
            acknowledgeRequired = true;
        }
    }
    Exit_Critical_Spinlock(subscriptionStateLock);

    Enter_Critical_Spinlock(calculationDataLock);
    for(auto it = openSeries.begin(); it != openSeries.end();)
    {
        (*it).life--;
        if(0 == (*it).life)
        {
            ClosedSeries* cs = (*it).series->CloseSeries();

            distance.AddSeries(cs);

            delete (cs);

            delete((*it).series);
            it = openSeries.erase(it);
        }
        else
        {
            ++it;
        }
    }
    distance.Recalculate();
    Exit_Critical_Spinlock(calculationDataLock);
}
