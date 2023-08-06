#ifndef SERIESCOMMON_HPP_
#define SERIESCOMMON_HPP_

#include "Common.h"
#if CONFIG_USE_RSSI != FALSE
typedef RSSI_Type DistanceUnits;
#else
typedef uint16 DistanceUnits;
#endif
typedef uint32 Series_Id;
typedef uint8  Message_Position_Id;

typedef enum SeriesError_Type
{
    SERIES_FULL,
    SERIES_CLOSED,
    WRONG_SERIES_ID,
    WRONG_MESSAGE_ID,
    NO_ERROR,
}SeriesError_Type;


#endif // SERIESCOMMON_HPP_