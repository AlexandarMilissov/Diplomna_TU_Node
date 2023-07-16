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
    distance.Receive(message.GetRSSI());
}

void Peer::RSSI_Msg_Received(RSSI_Message_Keep_Alive    message)
{
    if(CONNECTION_NOT_OK == distance.EvaluateConnection())
    {
        printf("Keep Alive received, validation failed, sender: %02x:%02x:%02x:%02x:%02x:%02x\n", 
            sourceAddress[0], 
            sourceAddress[1], 
            sourceAddress[2], 
            sourceAddress[3], 
            sourceAddress[4], 
            sourceAddress[5]);
        RSSI_Message_Request RSSI_Message = RSSI_Message_Request(0);
        RSSI_Message.Send();
    }
}

void Peer::RSSI_Msg_Received(RSSI_Message_Acknowledge   message)
{
    if(SENDEND == message.GetStatus())
    {
        distance.TheyFinishedSending();
    }
}
