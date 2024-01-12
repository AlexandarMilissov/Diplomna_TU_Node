#include "Distance.hpp"
#include "Distance_ref.hpp"

#include <string>
#include <cmath>

#define LOG_SCALE_BASE 10
#define FORMULA_CONSTANT 10
const float minimumConfidenceInterval = (float)CONFIG_MINIMUM_CONFIDENCE_INTERVAL / 2.0;

void Distance::AddValue(DistanceUnits distance)
{
    // Add the value to the map
    // If the value has been added before
    // Increase the number of occurrences by 1
    // If its the first time we see the value
    // Add it and set the number of occurrences to 1
    if (values.count(distance))
    {
        values[distance]++;
    }
    else
    {
        values[distance] = 1;
    }
    valuesCount++;
    sumOfAll += distance;

    AssignMostCommon(distance);
}

void Distance::AssignMostCommon(DistanceUnits distance)
{
    // If this is the first value - assign it
    if (0 == mostCommonCount)
    {
        mostCommon = distance;
        mostCommonCount++;
    }
    // If this is the same value as the most most common
    // Increase the mostCommonCount
    else if (distance == mostCommon)
    {
        mostCommonCount = values[distance];
    }
    // If this series has more values than the most common
    // Assign it as the most common
    else if (values[distance] > mostCommonCount)
    {
        mostCommon = distance;
        mostCommonCount = values[distance];
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

uint32 Distance::GetValuesCount()
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

    float ret = round(pow(LOG_SCALE_BASE, power) * resolution);

    return ret;
#endif
}

std::string Distance::GetMonitorData()
{
    std::string distanceLog;
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
        distanceLog += "not ";
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
    distanceLog += std::to_string(valuesCount ) + " total values.\n";

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

    return distanceLog;
}
