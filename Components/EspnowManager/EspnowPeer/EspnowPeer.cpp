#include "EspnowPeer.hpp"
#include "EspnowManager_Interface.hpp"
#include "EspnowManager_Communication.hpp"

#include <cstring>
#include <string>

void EspnowPeer::SendSubscriptionRequest()
{
    if(false == acknowledgeRequired)
    {
        return;
    }
    EspnowMessageRequest RSSI_Message = EspnowMessageRequest(areWeSubscribedToPeer);
    SendMessage(sourceAddress, RSSI_Message.GetPayload());
}

#if CONFIG_ENABLE_MONITOR && CONFIG_ENABLE_MESSAGE_MONITOR && CONFIG_ENABLE_PEER_MONITOR
const char* EspnowPeer::Log()
{
    static std::string peerLog;
    peerLog = "";
    peerLog += std::to_string(sourceAddress[0]) + ":";
    peerLog += std::to_string(sourceAddress[1]) + ":";
    peerLog += std::to_string(sourceAddress[2]) + ":";
    peerLog += std::to_string(sourceAddress[3]) + ":";
    peerLog += std::to_string(sourceAddress[4]) + ":";
    peerLog += std::to_string(sourceAddress[5]) + "\n";

#if CONFIG_ENABLE_MONITOR && CONFIG_ENABLE_MESSAGE_MONITOR && CONFIG_ENABLE_PEER_MONITOR && CONFIG_ENABLE_DISTANCE_MONITOR
    peerLog += distance.Log();
#endif
    return peerLog.c_str();
}
#endif

EspnowPeer::EspnowPeer(const uint8_t *src_addr) : peerLife(peerBeginningLife)
{
    if(false) // TODO: Validate
    {
        throw "Invalid input in constructor!\n";
    }
    memcpy(sourceAddress, src_addr, 6);
    distance = Distance();
}

EspnowPeer::~EspnowPeer()
{
    if(isPeerSubscribedToUs)
    {
        EspnowManager_Unsubscribe();
    }

}

bool EspnowPeer::IsCorrectAddress(const uint8* src_addr)
{
    if(0 == memcmp(sourceAddress, src_addr, 6))
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

    Enter_Critical_Spinlock(subscriptionStateProtection);

    messageStatus = message.GetSubscriptionStatus();

    if(SUBSCRIBE == messageStatus)
    {
        if(!isPeerSubscribedToUs)
        {
            isPeerSubscribedToUs = true;
            EspnowManager_Subscribe();
        }
        ackn = new EspnowMessageAcknowledge(SUBSCRIBE);
    }
    else if(UNSUBSCRIBE == message.GetSubscriptionStatus())
    {
        if(isPeerSubscribedToUs)
        {
            isPeerSubscribedToUs = false;
            EspnowManager_Unsubscribe();
        }
        ackn = new EspnowMessageAcknowledge(UNSUBSCRIBE);
    }
    Exit_Critical_Spinlock(subscriptionStateProtection);

    if(NULL != ackn)
    {
        SendMessage(sourceAddress, ackn->GetPayload());
        delete ackn;
    }

}

void EspnowPeer::ReceiveMessage(EspnowMessageCalculation   message)
{
    OpenSeries* series = NULL;

    Enter_Critical_Spinlock(calculationDataProtection);

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

    Exit_Critical_Spinlock(calculationDataProtection);
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
    Enter_Critical_Spinlock(subscriptionStateProtection);
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
    Exit_Critical_Spinlock(subscriptionStateProtection);

    Enter_Critical_Spinlock(calculationDataProtection);
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
    Exit_Critical_Spinlock(calculationDataProtection);
}
