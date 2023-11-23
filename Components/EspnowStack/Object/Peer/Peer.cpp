#include "Peer.hpp"
#include "EspnowManager.h"
#include "To_C_Encapsulation.h"

#include <cstring>

void Peer::SendSubscriptionRequest()
{
    if(false == acknowledgeRequired)
    {
        return;
    }
    RSSI_Message_Request RSSI_Message = RSSI_Message_Request(areWeSubscribedToPeer);
    RSSI_Message.Send(sourceAddress);
}

void Peer::Monitor()
{
    distance.LogInfo();
}

Peer::Peer(const uint8_t *src_addr)
{
    if(false) // TODO: Validate
    {
        throw "Invalid input in constructor!\n";
    }
    memcpy(sourceAddress, src_addr, 6);
    distance = Distance();
}

Peer::~Peer()
{
}

bool Peer::IsCorrectAddress(const uint8* src_addr)
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

void Peer::RSSI_Msg_Received(RSSI_Message_Request       message)
{
    bool messageStatus;

    Enter_Critical_Spinlock(selfProtection);

    messageStatus = message.GetSubscriptionStatus();

    if(SUBSCRIBE == messageStatus)
    {
        if(!isPeerSubscribedToUs)
        {
            isPeerSubscribedToUs = true;
            ManagerSubscribe();
        }
        RSSI_Message_Acknowledge ackn = RSSI_Message_Acknowledge(SUBSCRIBE);
        ackn.Send(sourceAddress);
    }
    else if(UNSUBSCRIBE == message.GetSubscriptionStatus())
    {
        if(isPeerSubscribedToUs)
        {
            isPeerSubscribedToUs = false;
            ManagerUnsubscribe();
        }
        RSSI_Message_Acknowledge ackn = RSSI_Message_Acknowledge(UNSUBSCRIBE);
        ackn.Send(sourceAddress);
    }

    Exit_Critical_Spinlock(selfProtection);
}

void Peer::RSSI_Msg_Received(RSSI_Message_Calculation   message)
{
    OpenSeries* series = NULL;
    Enter_Critical_Spinlock(openSeriesProtection);

    for(auto& s : openSeries)
    {
        if(s.series->IsCorrectId(message.GetSeriesID()))
        {
            series = s.series;
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
    Exit_Critical_Spinlock(openSeriesProtection);
}

void Peer::RSSI_Msg_Received(RSSI_Message_Keep_Alive    message)
{
    Enter_Critical_Spinlock(selfProtection);
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
    Exit_Critical_Spinlock(selfProtection);
}

void Peer::RSSI_Msg_Received(RSSI_Message_Acknowledge   message)
{
    if(message.GetStatus() == areWeSubscribedToPeer)
    {
        acknowledgeRequired = false;
    }
}

void Peer::UpdateSeries()
{
    Enter_Critical_Spinlock(openSeriesProtection);
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
    Exit_Critical_Spinlock(openSeriesProtection);
}
