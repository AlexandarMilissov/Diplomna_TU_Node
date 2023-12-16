#ifndef ESPNOWMANAGER_CONFIG_VALIDATION_H_
#define ESPNOWMANAGER_CONFIG_VALIDATION_H_

#include "sdkconfig.h"

#if (CONFIG_NUMBER_OF_MESSAGES_PER_SERIES * CONFIG_SERIES_CYCLIC_SEND_INTERVAL) >= CONFIG_SERIES_INITIATION_INTERVAL
    #error "Invalid send configuration. Overlapping send times."
#endif

#endif // ESPNOWMANAGER_CONFIG_VALIDATION_H_
