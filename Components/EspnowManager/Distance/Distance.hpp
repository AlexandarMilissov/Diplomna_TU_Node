#ifndef DISTANCE_HPP_
#define DISTANCE_HPP_

#include "Common.hpp"

#include "OpenSeries.hpp"

#include "math.h"
#include <map>
#include <vector>

#include "IMonitorable.hpp"

class Distance : public IMonitorable
{
    private:
        static const uint16 minNumberOfValues = CONFIG_MINIMUM_NUMBER_OF_VALUES;
        static const uint32 maxNumberOfValues = CONFIG_MAXIMUM_NUMBER_OF_VALUES;
        static const uint16 resolution = CONFIG_RESOLUTION;

        std::map<DistanceUnits, int> values;
        uint16 valuesCount = 0;
        uint16 valuesCountOld = 0;
        sint64 sumOfAll = 0;
        float mean = 0; // Average
        float standardDeviation = 0;

        DistanceUnits mostCommon;
        uint16 mostCommonCount = 0;
        float confidenceInterval = 0;

        void AssignMostCommon(DistanceUnits);
        void CalculateStandardDeviation();
        void CalculateConfidenceInterval();
    public:
        Distance();
        ~Distance();
        bool IsCalculationRequired();
        void AddValue(DistanceUnits);
        uint32 GetValuesCount();
        void Recalculate();
        static uint8 GetRequestedRepetitions();
        static DistanceUnits Float_To_DistanceUnits(float);

        std::string GetMonitorData();
};

#endif // DISTANCE_HPP_