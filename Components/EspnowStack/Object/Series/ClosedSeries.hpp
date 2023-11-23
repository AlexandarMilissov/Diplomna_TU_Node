#ifndef CLOSEDSERIES_HPP_
#define CLOSEDSERIES_HPP_

#include "Common.h"
#include "SeriesCommon.hpp"

class ClosedSeries
{
    private:
        static const DistanceUnits defaultValue = 0;
        DistanceUnits value = defaultValue;
    public:
        operator DistanceUnits () const;
        ClosedSeries();
        ClosedSeries(DistanceUnits);
        bool IsDefault();
};

#endif // CLOSEDSERIES_HPP_