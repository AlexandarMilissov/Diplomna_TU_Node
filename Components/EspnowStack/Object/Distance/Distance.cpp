#include "Distance.hpp"
#include "Distance_ref.hpp"

#include <string>
#include <cmath>

#define LOG_SCALE_BASE 10
#define FORMULA_CONSTANT 10
const float minimumConfidenceInterval = (float)CONFIG_MINIMUM_CONFIDENCE_INTERVAL / 2.0;

void Distance::AddSeriesSafe(ClosedSeries *cs)
{
    // Add the value to the map
    // If the value has been added before
    // Increase the number of occurrences by 1
    // If its the first time we see the value
    // Add it and set the number of occurrences to 1
    if (values.count(*cs))
    {
        values[*cs]++;
    }
    else
    {
        values[*cs] = 1;
    }
    valuesCount++;
    sumOfAll += *cs;

    AssignMostCommon(*cs);
}

void Distance::AssignMostCommon(ClosedSeries cs)
{
    // If this is the first value assign it
    if (0 == mostCommonCount)
    {
        mostCommon = cs;
        mostCommonCount++;
    }
    // If this is the same value as the most most common
    // Increase the mostCommonCount
    else if (cs == mostCommon)
    {
        mostCommonCount = values[cs];
    }
    // If this series has more values than the most common
    // Assign it as the most common
    else if (values[cs] > mostCommonCount)
    {
        mostCommon = cs;
        mostCommonCount = values[cs];
    }
}

void Distance::CalculateStandardDeviation()
{
    // Find the mean
    mean = ((float)sumOfAll / (float)valuesCount);

    // Calculate standard deviation (σ)
    float tempStandardDeviation = 0;
    float standardDeviationInstance = 0;

    for (auto &valueSet : values)
    {
        standardDeviationInstance = mean - valueSet.first;
        tempStandardDeviation += Square(standardDeviationInstance) * valueSet.second;
    }
    tempStandardDeviation /= (valuesCount - 1);

    standardDeviation = sqrt(tempStandardDeviation);
}

void Distance::CalculateConfidenceInterval()
{
    confidenceInterval = standardDeviation / sqrt(valuesCount);
    confidenceInterval *= Z95;
}

Distance::Distance()
{
}

Distance::~Distance()
{
}

uint8 Distance::GetRequestedRepetitions()
{
    return OpenSeries::GetNumberOfMessagesPerSeries();
}

bool Distance::IsCalculationRequired()
{
    bool ret = false;
    if (minNumberOfValues > valuesCount)
    {
        ret = true;
    }
    else if (valuesCount > maxNumberOfValues)
    {
        ret = false;
    }
    else if (confidenceInterval > minimumConfidenceInterval)
    {
        ret = true;
    }

    return ret;
}

void Distance::AddSeries(ClosedSeries *cs)
{
    // Check if the added value is valid
    // If it's not then discard it
    if (cs->IsDefault())
    {
        failedSeries++;
    }
    else
    {
        AddSeriesSafe(cs);
    }
}

uint32 Distance::GetSeriesCount()
{
    return valuesCount;
}

void Distance::Recalculate()
{
    // No new values have been added so
    // There is no need to recalculate
    if (valuesCount == valuesCountOld)
    {
        return;
    }
    valuesCountOld = valuesCount;

    // There are not enough values to calculate reliably
    // End the function and safe computation time
    if (valuesCount < minNumberOfValues)
    {
        return;
    }

    CalculateStandardDeviation();
    CalculateConfidenceInterval();
}

/**
 * @brief Convert RSSI (Received Signal Strength Indication) to distance scaled by the resolution.
 */
DistanceUnits Distance::Float_To_DistanceUnits(float value)
{
#if CONFIG_USE_RSSI != FALSE
    return (DistanceUnits)round(value * resolution);
#else
    // Convert RSSI to distance if using meters
    // Calculate the power using the logarithmic signal propagation model:
    float power = (RSSI_DBM_REF_AT_1_M - static_cast<float>(value)) / (FORMULA_CONSTANT * ENV_VARIABLE);

    // Convert the power back to linear scale and scale by distance units in one meter:
    return round(pow(LOG_SCALE_BASE, power) * resolution);
#endif
}

#if CONFIG_ENABLE_MONITOR && CONFIG_ENABLE_MESSAGE_MONITOR && CONFIG_ENABLE_PEER_MONITOR && CONFIG_ENABLE_DISTANCE_MONITOR
const char* Distance::Log()
{
    // Init
    static std::string distanceLog;
    distanceLog = "";


#if CONFIG_USE_RSSI != FALSE
    const char units[] = "dBm";
#else
    const char units[] = "meters";
#endif

    // Is calculation required?
    distanceLog += "Calculation is ";
    if(!IsCalculationRequired())
    {
        distanceLog += "not";
    }
    distanceLog += "required. ";

    // Summarized data
    distanceLog += "[" + std::to_string(mostCommonCount) + "]/[" + std::to_string(valuesCount) + "]. ";
    distanceLog += "The most common is [" + std::to_string(mostCommon) + "] with a standard deviation of " + std::to_string(standardDeviation);
    distanceLog += " Minumum confidence: " + std::to_string(minimumConfidenceInterval) + "\n";

    // Result in raw form
    distanceLog += "\nWith 95% certainty, distance is " + std::to_string(mean) + " ± " + std::to_string(confidenceInterval) + " units. ";
    distanceLog += "Units are (1/" + std::to_string(resolution) + ")" + units + ".\n";

    // Results in formatted form
    distanceLog += "This is equal to " + std::to_string(mean/resolution);;
    distanceLog += " ± " + std::to_string(confidenceInterval/resolution) + units + "\n";


    // Additional information
    float failure_rate = ((float)(failedSeries * 100)) / ((float)(failedSeries + valuesCount));
    distanceLog += std::to_string(failedSeries) + " failed series.\n";
    distanceLog += std::to_string(valuesCount ) + " successful series.\n";
    distanceLog += std::to_string(failure_rate) + "% failure rate.\n";

    if (IsCalculationRequired() != 0)
    {
        uint16 truncated_counter = 0;
        for (auto value : values)
        {
            if(value.second * 8 < mostCommonCount)
            {
                truncated_counter++;
            }
            else
            {
                distanceLog += "    Value[" + std::to_string((int)value.first) + "], count: " + std::to_string(value.second) + "\n";
            }
        }
        distanceLog += std::to_string(truncated_counter) + " truncated(hidden) values.\n";
    }

    return distanceLog.c_str();
}
#endif
