#ifndef COMMON_H
#define COMMON_H

#include "esp_task_wdt.h"
#include <stdint.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/semphr.h"

typedef portMUX_TYPE Spinlock;
#define Spinlock_Init portMUX_INITIALIZER_UNLOCKED
#define Enter_Critical_Spinlock(mux) taskENTER_CRITICAL(&mux)
#define Exit_Critical_Spinlock(mux) taskEXIT_CRITICAL(&mux)
#define Enter_Critical_Spinlock_ISR(mux) taskENTER_CRITICAL_ISR(&mux)
#define Exit_Critical_Spinlock_ISR(mux) taskEXIT_CRITICAL_ISR(&mux)

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t   sint8;
typedef int16_t  sint16;
typedef int32_t  sint32;
typedef int64_t  sint64;

typedef signed int RSSI_Type;

#ifndef portTICK_PERIOD_MS
    #define portTICK_PERIOD_MS ( ( TickType_t ) 1000 / CONFIG_FREERTOS_HZ )
#endif
#define TaskSleepMiliSeconds(value) vTaskDelay(pdMS_TO_TICKS(value))

#define Map(in, range_to_low, range_to_high, range_from_low, range_from_high) ((in-range_from_low)/(range_from_high-range_from_low) * (range_to_high - range_to_low) + range_to_low)

#ifndef LOW
    #define LOW 0x0
#endif

#ifndef HIGH
    #define HIGH 0x1
#endif

#endif