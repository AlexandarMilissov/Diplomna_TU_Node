#include "To_CPP_Encapsulation.hpp"

#include "Peer.hpp"
#include "RSSI_Message_Request.hpp"
#include "RSSI_Message_Calculation.hpp"
#include "RSSI_Message_Keep_Alive.hpp"
#include "RSSI_Message_Interface.hpp"

#include <list>
#include <algorithm>

using namespace std;


list<Peer> Peers;
void To_CPP_Encapsulation_Init(void* pvparrams)
{
    Peers = {};
}

void Send_Keep_Alive_Msg()
{
    RSSI_Message_Keep_Alive::StaticSend();
}

esp_err_t RSSI_MessageReceive(uint8_t *src_addr, MessageStruct messageStruct, size_t size, wifi_pkt_rx_ctrl_t *rx_ctrl)
{
    Peer* sender = NULL;

    // Indentify the sender

    for(auto& peer : Peers)
    {
        if(peer.IsCorrectAdress(src_addr))
        {
            sender = &peer;
            break;
        }
    }
    
    if(NULL == sender)
    {
        sender = new Peer(src_addr);
        Peers.push_back(*sender);
    }

    // Proccess the message
    try
    {
        switch (messageStruct.messageType)
        {
        case RSSI_REQUEST:
        {
            RSSI_Message_Request RSSI_Message = RSSI_Message_Request(messageStruct);
            sender->RSSI_Msg_Received(RSSI_Message);
        }
        break;
        case RSSI_CALCULATION:
        {
            RSSI_Message_Calculation RSSI_Message = RSSI_Message_Calculation(rx_ctrl->rssi);
            sender->RSSI_Msg_Received(RSSI_Message);
        }
        break;
        case RSSI_KEEP_ALIVE:
        {
            RSSI_Message_Keep_Alive RSSI_Message = RSSI_Message_Keep_Alive(rx_ctrl->rssi);
            sender->RSSI_Msg_Received(RSSI_Message);
        }
        break;
        case RSSI_ACKNOWLEDGE:
        {
            RSSI_Message_Acknowledge RSSI_Message = RSSI_Message_Acknowledge(messageStruct);
            sender->RSSI_Msg_Received(RSSI_Message);
        }
        break;
        default:
            break;
        }
    }
    catch(const ValidationFailedException& e)
    {
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}