#include "Common.hpp"

// TODO: Move this to WifiManager

uint8_t my_esp_now_mac[ESP_NOW_ETH_ALEN];
uint8_t broadcast_mac [ESP_NOW_ETH_ALEN] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
