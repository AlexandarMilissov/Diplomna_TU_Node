#include "Peer.hpp"
#include "RSSI_Message_Interface.hpp"
#include "EspnowManager.h"

#include <cstring>

void Peer::CalculationChain_MainFunction(void* args)
{
    RSSI_Message_Calculation RSSI_Message = RSSI_Message_Calculation();
    RSSI_Message.Send();
}

void Peer::CalculationChain_EndFunction(void* args)
{
    RSSI_Message_Acknowledge RSSI_Message = RSSI_Message_Acknowledge(SENDEND);
    RSSI_Message.Send();

    RSSI_Message_Interface::rssi_Calculation_Request_Status = NOT_REQUESTED;
}

void Peer::Recalculate()
{
}

PeerConnection Peer::EvaluateConnection()
{
    return CONNECTION_OK;
}

Peer::Peer(uint8_t *src_addr)
{
    if(false) // TODO: Validate
    {
        throw "Invalid input in constructor!\n";
    }

    RSSI = 0;
    RSSI_average = 0;

    memcpy(sourceAddress, src_addr, 6);
    memset(lastReceived, 0, sizeof(lastReceived[0]) * NUM_OF_KEPT_RECEIVED);
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
    if(ONGOING == RSSI_Message_Interface::rssi_Calculation_Request_Status)
    {
        RSSI_Message_Acknowledge RSSI_Message = RSSI_Message_Acknowledge(REJECT);
        RSSI_Message.Send();
    }
    else if (NOT_REQUESTED == RSSI_Message_Interface::rssi_Calculation_Request_Status)
    {
        RSSI_Message_Interface::rssi_Calculation_Request_Status = ONGOING;
        
        Task_cfg_struct task_config = {
            .name = "Peer_RSSI_Calculation_Msg_Send",
            .MainFunction = CalculationChain_MainFunction,
            .mainFunctionParams = (void*)sourceAddress,
            .period = 20,
            .core = (uint32)-1,
            .stack_size = 4096,
            .priority = 100,
            .finite = true,
            .repetition = NUM_OF_KEPT_RECEIVED,
            .OnComplete = CalculationChain_EndFunction,
            .onCompleteParams = (void*)sourceAddress
        };
        RequestTask(task_config);

    }
}

void Peer::RSSI_Msg_Received(RSSI_Message_Calculation   message)
{
    lastReceived[received_counter % NUM_OF_KEPT_RECEIVED] = message.GetRSSI();
    received_counter++;
}

void Peer::RSSI_Msg_Received(RSSI_Message_Keep_Alive    message)
{
    if(CALCULATION_INIT  == calculationStatus
    || CONNECTION_NOT_OK == EvaluateConnection())
    {
        RSSI_Message_Request RSSI_Message = RSSI_Message_Request(RSSI);
        RSSI_Message.Send();
    }
}

void Peer::RSSI_Msg_Received(RSSI_Message_Acknowledge   message)
{
    if(SENDEND == message.GetStatus())
    {
        Recalculate();
    }
}
