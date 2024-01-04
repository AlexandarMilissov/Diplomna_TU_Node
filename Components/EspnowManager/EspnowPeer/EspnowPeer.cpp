#include "EspnowPeer.hpp"
#include <cstring>
#include <string>
#include <stdexcept>

void EspnowPeer::Receive(const std::queue<Payload> payloadQueueOriginal)
{
    // Copy the queue to not modify the original
    std::queue<Payload> payloadQueue = payloadQueueOriginal;

    if(payloadQueue.empty())
    {
        throw std::invalid_argument("Payload queue is empty!\n");
    }

    // Get the message identifier
    Payload messageIdentifierPayload = payloadQueue.front();
    payloadQueue.pop();
    EspnowMessageType messageIdentifier = *((EspnowMessageType*)messageIdentifierPayload.data);

    try
    {
        // Call the correct function for the message based on the identifier
        switch (messageIdentifier)
        {
        case NOW_REQUEST:
        {
            EspnowMessageRequest RSSI_Message = EspnowMessageRequest(payloadQueue);
            this->ReceiveMessage(RSSI_Message);
        }
        break;
        case NOW_CALCULATION:
        {
            EspnowMessageCalculation RSSI_Message = EspnowMessageCalculation(payloadQueue);
            this->ReceiveMessage(RSSI_Message);
        }
        break;
        case NOW_KEEP_ALIVE:
        {
            EspnowMessageKeepAlive RSSI_Message = EspnowMessageKeepAlive(payloadQueue);
            this->ReceiveMessage(RSSI_Message);
        }
        break;
        case NOW_ACKNOWLEDGE:
        {
            EspnowMessageAcknowledge RSSI_Message = EspnowMessageAcknowledge(payloadQueue);
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
    std::stack<Payload> payloadStack = RSSI_Message.GetPayload();
    lowerLayer.Send(sourceAddress, payloadStack);
}

std::string EspnowPeer::GetMonitorData()
{
    std::string peerLog = sourceAddress.ToString() + "\n";

    peerLog += distance.GetMonitorData();
    return peerLog;
}

EspnowPeer::EspnowPeer(
    IMessageSender& messageable,
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
        std::stack<Payload> payloadStack = ackn->GetPayload();
        lowerLayer.Send(sourceAddress, payloadStack);
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
