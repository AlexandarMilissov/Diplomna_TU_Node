#include "RSSI_Message_Keep_Alive.hpp"

uint8 RSSI_Message_Keep_Alive::GetElementsSize()
{
    return 0;
}

RSSI_Message_Keep_Alive::RSSI_Message_Keep_Alive(RSSI_Type _RSSI)
{
    RSSI = _RSSI;
}

RSSI_Message_Keep_Alive::~RSSI_Message_Keep_Alive()
{
}

void RSSI_Message_Keep_Alive::StaticSend()
{    
    MessageSend(RSSI_KEEP_ALIVE, 0, NULL);
}

void RSSI_Message_Keep_Alive::Send()
{
    StaticSend();
}
