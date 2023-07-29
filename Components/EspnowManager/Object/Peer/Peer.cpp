#include "Peer.hpp"
#include "RSSI_Message_Interface.hpp"
#include "EspnowManager.h"
#include "To_C_Encapsulation.h"

#include <cstring>

void Peer::SendSubsriptionRequest()
{
    if(false == acknowledgeRequired)
    {
        return;
    }
    RSSI_Message_Request RSSI_Message = RSSI_Message_Request(areWeSubscirbedToPeer);
    RSSI_Message.Send();
}

Peer::Peer(uint8_t *src_addr)
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

bool Peer::IsCorrectAdress(uint8* src_addr)
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
    if(SUBSCRIBE == message.GetSubsricptionStatus())
    {
        if(!isPeerSubscirbedToUs)
        {
            isPeerSubscirbedToUs = true;
            ManagerSubscribe();
        }
        RSSI_Message_Acknowledge ackn = RSSI_Message_Acknowledge(SUBSCRIBE);
        ackn.Send();
    }
    else if(UNSUBSCRIBE == message.GetSubsricptionStatus())
    {
        if(isPeerSubscirbedToUs)
        {
            isPeerSubscirbedToUs = false;
            ManagerUnsubscribe();
        }
        RSSI_Message_Acknowledge ackn = RSSI_Message_Acknowledge(UNSUBSCRIBE);
        ackn.Send();
    }
    else
    {
        printf("ERROR\n");
        while(1);
    }
}

void Peer::RSSI_Msg_Received(RSSI_Message_Calculation   message)
{
    OpenSeries* series = NULL;
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
        sl.life = seriersBeginningLife;
        sl.series = series;
        openSeries.push_back(sl);
    }
    
    series->AddValue(message.GetMessagePosition(), message.GetRSSI());
}

void Peer::RSSI_Msg_Received(RSSI_Message_Keep_Alive    message)
{
    /*
    printf("Keep Alive received (%d, %d) sender: %02x:%02x:%02x:%02x:%02x:%02x\n",
        areWeSubscirbedToPeer,
        distance.IsCalculationRequired(),
        sourceAddress[0], 
        sourceAddress[1], 
        sourceAddress[2], 
        sourceAddress[3], 
        sourceAddress[4], 
        sourceAddress[5]);
        */
    if(areWeSubscirbedToPeer)
    {
        if(!distance.IsCalculationRequired())
        {
            areWeSubscirbedToPeer = false;
            acknowledgeRequired = true;
        }
    }
    else
    {
        if(distance.IsCalculationRequired())
        {
            areWeSubscirbedToPeer = true;
            acknowledgeRequired = true;
        }
    }
}

void Peer::RSSI_Msg_Received(RSSI_Message_Acknowledge   message)
{
    if(message.GetStatus() == areWeSubscirbedToPeer)
    {
        acknowledgeRequired = false;
    }
}

void Peer::UpdateSeries()
{
    for(auto it = openSeries.begin(); it != openSeries.end();)
    {
        (*it).life--;
        if(0 == (*it).life)
        {
            ClosedSeries* cs = (*it).series->CloseSeries();

            distance.AddSeries(cs);

            delete((*it).series);
            it = openSeries.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
