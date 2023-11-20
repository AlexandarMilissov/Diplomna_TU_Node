#ifndef CLOSEDSERIES_HPP_
#define CLOSEDSERIES_HPP_

#include "Common.h"
#include "SeriesCommon.hpp"

class ClosedSeries
{
    public:
        static const DistanceUnits defaultValue = 0;
    private:
        DistanceUnits value = defaultValue;
    public:
        operator DistanceUnits () const;
        ClosedSeries();
        ClosedSeries(DistanceUnits);
};

#endif // CLOSEDSERIES_HPP_