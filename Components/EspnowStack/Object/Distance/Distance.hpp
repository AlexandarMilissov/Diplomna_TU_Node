#ifndef DISTANCE_HPP_
#define DISTANCE_HPP_

#include "Common.h"

#include "OpenSeries.hpp"

#include "math.h"
#include <map>
#include <vector>

class Distance
{
    private:
        static const uint16 minNumberOfValues = CONFIG_MINIMUM_NUMBER_OF_VALUES;
        static const uint16 maxNumberOfValues = CONFIG_MAXIMUM_NUMBER_OF_VALUES;
        static const uint16 distanceUnitsInAMeter = CONFIG_DISTANCE_UNITS_IN_A_METER;

        std::map<ClosedSeries, int> values;
        uint16 valuesCount = 0;
        uint16 valuesCountOld = 0;
        sint64 sumOfAll = 0;
        float mean = 0; // Average
        float standardDeviation = 0;

        ClosedSeries mostCommon;
        uint16 mostCommonCount = 0;
        float confidenceInterval = 0;

        uint16 failedSeries = 0;

        void AddSeriesSafe(ClosedSeries*);
        void AssignMostCommon(ClosedSeries);
        void CalculateStandardDeviation();
        void CalculateConfidenceInterval();
    public:
        Distance();
        ~Distance();
        bool IsCalculationRequired();
        void AddSeries(ClosedSeries*);
        uint32 GetSeriesCount();
        void Recalculate();
        static uint8 GetRequestedRepetitions();
        template <typename valueType>
        static DistanceUnits RSSI_To_DistanceUnits(valueType);
#if CONFIG_ENABLE_MONITOR && CONFIG_ENABLE_MESSAGE_MONITOR && CONFIG_ENABLE_PEER_MONITOR && CONFIG_ENABLE_DISTANCE_MONITOR
        const char* Log();
#endif
};

#endif // DISTANCE_HPP_