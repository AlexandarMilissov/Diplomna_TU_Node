#ifndef COMMON_HPP
#define COMMON_HPP

#include "esp_task_wdt.h"
#include <stdint.h>
#include "freertos/semphr.h"
#include "sdkconfig.h"
#include "stdbool.h"
#include "esp_now.h"
#include "esp_netif_types.h"

#define CORE_0 0
#define CORE_1 1

#define DISABLE_OPTIMIZATION \
    _Pragma("GCC push_options") \
    _Pragma("GCC optimize (\"O0\")")

#define ENABLE_OPTIMIZATION \
    _Pragma("GCC pop_options")

typedef portMUX_TYPE Spinlock;
#define Spinlock_Init portMUX_INITIALIZER_UNLOCKED
#define Enter_Critical_Spinlock(mux)        taskENTER_CRITICAL(&mux)
#define Exit_Critical_Spinlock(mux)         taskEXIT_CRITICAL(&mux)
#define Enter_Critical_Spinlock_ISR(mux)    taskENTER_CRITICAL_ISR(&mux)
#define Exit_Critical_Spinlock_ISR(mux)     taskEXIT_CRITICAL_ISR(&mux)

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t   sint8;
typedef int16_t  sint16;
typedef int32_t  sint32;
typedef int64_t  sint64;

typedef esp_ip4_addr Ipv4Address;

typedef signed int RSSI_Type;

#define sizeof_array(array) (sizeof(array)/sizeof(array[0]))

#ifndef portTICK_PERIOD_MS
    #define portTICK_PERIOD_MS ( ( TickType_t ) 1000 / CONFIG_FREERTOS_HZ )
#endif
#define TaskSleepMiliSeconds(value) vTaskDelay(pdMS_TO_TICKS(value))

#define Map(in, range_to_low, range_to_high, range_from_low, range_from_high) ((in-range_from_low)/(range_from_high-range_from_low) * (range_to_high - range_to_low) + range_to_low)
#define Square(value) (value * value)
#define DUMMY_STATEMENT(var) (void)var

#ifndef LOW
    #define LOW 0x0
#endif

#ifndef HIGH
    #define HIGH 0x1
#endif

#endif
