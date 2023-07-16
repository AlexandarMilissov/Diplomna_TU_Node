#include "Distance.hpp"
#include <cstring>

Distance::Distance()
{
    RSSI = 0;
    RSSI_average = 0;

    memset(lastReceived, 0, sizeof(lastReceived[0]) * NUM_OF_KEPT_RECEIVED);
}

Distance::~Distance()
{
}

void Distance::Receive(RSSI_Type newRSSI)
{
    lastReceived[received_counter % NUM_OF_KEPT_RECEIVED] = newRSSI;
    
    received_counter++;
}

RSSI_Type Distance::GetRSSI()
{
    return RSSI_average;
}

uint16 Distance::GetRequestedRepetiotions()
{
    return NUM_OF_KEPT_RECEIVED;
}

void Distance::TheyFinishedSending()
{
    for (size_t i = 0; i < NUM_OF_KEPT_RECEIVED; i++)
    {
        printf("%-3d ",lastReceived[i]);
    }
    
    printf("\n TheyFinishedSending \n");
}

ConnectionStatus Distance::EvaluateConnection()
{
    static bool b = false;
    if(b == false)
    {
        b = true;
        return CONNECTION_NOT_OK;
    }
    else
    {
        return CONNECTION_OK;
    }
}