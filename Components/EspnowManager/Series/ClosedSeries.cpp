#include "ClosedSeries.hpp"

ClosedSeries::operator DistanceUnits() const
{
    return value;
}

ClosedSeries::ClosedSeries()
{
    value = defaultValue;
}

ClosedSeries::ClosedSeries(DistanceUnits _value) : value(_value)
{
}

bool ClosedSeries::IsDefault()
{
    return value == defaultValue;
}