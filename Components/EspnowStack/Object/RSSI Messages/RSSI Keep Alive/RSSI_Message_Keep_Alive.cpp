#include "RSSI_Message_Keep_Alive.hpp"
#include "EspnowManager_Communication.hpp"

uint8 RSSI_Message_Keep_Alive::GetElementsSize()
{
    return 0;
}

// On receive
RSSI_Message_Keep_Alive::RSSI_Message_Keep_Alive(RSSI_Type _RSSI)
{
    RSSI = _RSSI;
}

RSSI_Message_Keep_Alive::~RSSI_Message_Keep_Alive()
{
}

void RSSI_Message_Keep_Alive::StaticSend()
{
   SendMessage(broadcast_mac, RSSI_KEEP_ALIVE, NULL);
}

void RSSI_Message_Keep_Alive::Send(uint8* dst_addr)
{
    StaticSend();
}
