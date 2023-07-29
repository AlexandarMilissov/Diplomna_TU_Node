#include "ClosedSeries.hpp"

ClosedSeries::operator DistanceUnits() const
{
    return value;
}

ClosedSeries::ClosedSeries()
{
}

ClosedSeries::ClosedSeries(DistanceUnits _value) : value(_value)
{
}