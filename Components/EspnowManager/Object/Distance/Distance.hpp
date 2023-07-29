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
        static const uint16 minNumberOfValues = 100;
        static const uint16 maxNumberOfValues = 2000;
        static const uint16 distanceUnitsInAMeter = 100;
        static const DistanceUnits maximumError = 1;

        std::map<ClosedSeries, int> values;
        double maximumMarginOfError = 100.0;
        uint16 valuesCount = 0;
        uint64 sumOfAll = 0;
        double mean = 0; // Average
        double standardDeviation = 0;

        ClosedSeries mostCommon;
        uint16 mostCommonCount = 0;
        double marginOfError = 0; // Margin of error

        void AddSeriesSafe(ClosedSeries*);
        void AssignMostCommon(ClosedSeries);
        void CalculateMaximumMarginOfError();
        void CalculateStandardDeviation();
        void CalculateMarginOfError();
    public:
        Distance();
        ~Distance();
        bool IsCalculationRequired();
        void AddSeries(ClosedSeries*);
        static uint8 GetRequestedRepetiotions();
        static DistanceUnits RSSI_To_DistanceUnits(double);
};

#endif // DISTANCE_HPP_