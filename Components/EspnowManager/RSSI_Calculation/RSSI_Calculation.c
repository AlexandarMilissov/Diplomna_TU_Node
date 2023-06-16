#include "RSSI_Calculation.h"
#include "Lists.h"

List_I* Peers;

void RSSI_Calculation_Init(void* pvparams)
{
    Peers = NewList();
}

void RSSI_Calculation_Msg_Received(void* vpmessage, size_t receivedMessageSize, RSSI_Type Received_RSSI)
{
    if(sizeof(RSSI_Calculation_Msg) != receivedMessageSize)
    {
        printf("%d != %d\n",sizeof(RSSI_Calculation_Msg),receivedMessageSize);
        return; // The received message is incorrect
    }

    RSSI_Calculation_Msg message = *((RSSI_Calculation_Msg*)vpmessage);
    bool isFound = false;

    Peer* peer;
    First(Peers);
    
    if(Peers->count > 0)
    {
        for(int i = 0; i < Peers->count; i++)
        {
            if(((Peer*)This(Peers))->id == message.id)
            {
                peer = ((Peer*)This(Peers));
                isFound = true;
                break;
            }
            else
            {
                Next(Peers);
            }
        }
    }
    First(Peers);
    
    if(isFound)
    {
        if(peer->received_counter < NUM_OF_KEPT_RECEIVED)
        {
            peer->lastReceived[peer->received_counter] = Received_RSSI;
            peer->RSSI += Received_RSSI;
        }
        else
        {
            peer->RSSI -= peer->lastReceived[peer->received_counter % NUM_OF_KEPT_RECEIVED];
            peer->lastReceived[peer->received_counter % NUM_OF_KEPT_RECEIVED] = Received_RSSI;  
            peer->RSSI += Received_RSSI;
        }
        peer->received_counter++;
    }
    else
    {
        peer = (Peer*)malloc(sizeof(Peer));

        peer->id = message.id;
        peer->received_counter = 0;
        peer->RSSI = 0;

        
        for(int i = 0; i < NUM_OF_KEPT_RECEIVED; i++)
        {
            peer->lastReceived[i] = 0;
        }
        Last(Peers);
        Add(Peers, peer);
    }
    
    printf("Id received:                %d\n", message.id);
    printf("Actual received:            %d\n", Received_RSSI);
    for(int i = 0; i < 5; i++)
    {
        printf("[%d] = %d",i,peer->lastReceived[i]);
        if(i == ((peer->received_counter - 1) % NUM_OF_KEPT_RECEIVED))
        {
            printf(" <- ");
        }
        printf("\n");
    }
    
    printf("Calculated avrg received:   %.1f\n", ((float)peer->RSSI)/5);
    printf("List items count            %d\n", Peers->count);
    First(Peers);

    free(vpmessage);
}