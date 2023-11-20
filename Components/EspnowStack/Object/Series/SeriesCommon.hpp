#ifndef SERIESCOMMON_HPP_
#define SERIESCOMMON_HPP_

#include "Common.h"
#if CONFIG_USE_RSSI != FALSE
typedef RSSI_Type DistanceUnits;
#else
typedef uint16 DistanceUnits;
#endif
typedef uint32 Series_Id;
/**
 * @typedef Message_Position_Id
 * @brief Represents a unique identifier for the position of a message in a series.
 */
typedef uint8 Message_Position_Id;

#endif // SERIESCOMMON_HPP_