#include "Peer.hpp"
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
    RSSI_Message.Send(sourceAddress);
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
        ackn.Send(sourceAddress);
    }
    else if(UNSUBSCRIBE == message.GetSubsricptionStatus())
    {
        if(isPeerSubscirbedToUs)
        {
            isPeerSubscirbedToUs = false;
            ManagerUnsubscribe();
        }
        RSSI_Message_Acknowledge ackn = RSSI_Message_Acknowledge(UNSUBSCRIBE);
        ackn.Send(sourceAddress);
    }
    else
    {
        ESP_LOGE("To_CPP_Encapsulation","PEER ERROR MESSAGE REQUEST WITH WRONG VALUE:\n");
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

    if(NO_ERROR != series->AddValue(message.GetMessagePosition(), message.GetRSSI()))
    {
        ESP_LOGW("Peer", "Open Series[%lu] adding value failed", message.GetSeriesID());
    }
}

void Peer::RSSI_Msg_Received(RSSI_Message_Keep_Alive    message)
{
    if(areWeSubscirbedToPeer)
    {
        if(!distance.IsCalculationRequired())
        {
            ESP_LOGI("Peer","Calculation Ended");
            distance.LogInfo();
            areWeSubscirbedToPeer = false;
            acknowledgeRequired = true;
        }
    }
    else
    {
        if(distance.IsCalculationRequired())
        {
            ESP_LOGI("Peer","Calculation Started");
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

    distance.Recalculate();
}
