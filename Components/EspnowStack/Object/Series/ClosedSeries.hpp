#ifndef CLOSEDSERIES_HPP_
#define CLOSEDSERIES_HPP_

#include "Common.h"
#include "SeriesCommon.hpp"

class ClosedSeries
{
    public:
        static const DistanceUnits defaltValue = 0;
    private:
        DistanceUnits value = defaltValue;
    public:
        operator DistanceUnits () const;
        ClosedSeries();
        ClosedSeries(DistanceUnits);
};

#endif // CLOSEDSERIES_HPP_