#include "Distance.hpp"
#include "Distance_ref.hpp"

const float maximumConfidenceInterval = (float)CONFIG_MINIMUM_CONFIDENCE_INTERVAL / 2.0;

void Distance::AddSeriesSafe(ClosedSeries* cs)
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
    if(0 == mostCommonCount)
    {
        mostCommon = cs;
        mostCommonCount++;
    }
    // If this is the same value as the most most common
    // Increase the mostCommonCount
    else if(cs == mostCommon)
    {
        mostCommonCount = values[cs];
    }
    // If this series has more values than the most common
    // Assign it as the most common
    else if(values[cs] > mostCommonCount)
    {
        mostCommon = cs;
        mostCommonCount = values[cs];
    }
}

void Distance::CalculateStandardDeviation()
{
    // Find the mean
    mean = ((double)sumOfAll / (double)valuesCount);

    // Calculate standard deviation (σ)
    double tempStandardDeviation = 0;
    double standardDeviationInstance = 0;

    for(auto& valueSet : values)
    {
        standardDeviationInstance = mean - valueSet.first;
        tempStandardDeviation += (standardDeviationInstance * standardDeviationInstance) * valueSet.second;
    }
    tempStandardDeviation /= (valuesCount - 1);


    standardDeviation = sqrt(tempStandardDeviation);
}

void Distance::CalculateConfidenceInterval()
{
    confidenceInterval = standardDeviation / sqrt(valuesCount);
    confidenceInterval *= z95;
}

Distance::Distance()
{
}

Distance::~Distance()
{
}

uint8 Distance::GetRequestedRepetitions()
{
    return OpenSeries::numberOfMessagesPerSeries;
}

bool Distance::IsCalculationRequired()
{
    bool ret = false;
    if(minNumberOfValues > valuesCount)
    {
        ret = true;
    }
    else if(valuesCount > maxNumberOfValues)
    {
        ret = false;
    }
    else if(confidenceInterval > maximumConfidenceInterval)
    {
        ret = true;
    }
    
    return ret;
}

void Distance::AddSeries(ClosedSeries* cs)
{
    // Check if the added value is valid
    // If it's not then discard it
    if(ClosedSeries::defaultValue == *cs)
    {
        // ESP_LOGW("Distance", "Series with default value received");
    }
    else
    {
        AddSeriesSafe(cs);
    }
    delete(cs);
}

uint32 Distance::GetSeriesCount()
{
    return valuesCount;
}

void Distance::Recalculate()
{
    // No new values have been added so
    // There is no need to recalculate
    if(valuesCount == valuesCountOld)
    {
        return;
    }
    valuesCountOld = valuesCount;

    
    // There are not enough values to calculate reliably
    // End the function and safe computation time
    if(valuesCount < minNumberOfValues)
    {
        return;
        // ESP_LOGI("Distance", "Not enough values for calculation");
    }
    
    CalculateStandardDeviation();
    CalculateConfidenceInterval();
}

/**
 * @brief Convert RSSI (Received Signal Strength Indication) to distance in specified units.
 *
 * This function uses a logarithmic signal propagation model to estimate the distance
 * based on the RSSI value and environmental factors.
 * 
 * @example If we have -30dBm as reference at 1 meter and 1 distance units in a meter
 * and we receive -30dBm we will return 1m because that is the distance to the target.
 * 
 * @example If we have -30dBm as reference at 1 meter and 100 distance units in a meter (100 cm)
 * and we receive -30dBm we will return 100cm because that is the distance to the target.
 *
 * @example If we have -30dBm as reference at 1 meter and 1 distance units in a meter
 * and we receive -25dBm we will return 0.5m because that is the distance to the target.
 * Since this DistanceUnits are whole numbers, the result will be rounded to either 0m or 1m. 
 * 
 * @example If we have -30dBm as reference at 1 meter and 100 distance units in a meter (100 cm)
 * and we receive -25dBm we will return 50cm because that is the distance to the target.
 * 
 * @param value The RSSI value to be converted.
 * @return The distance in the specified units.
 */
DistanceUnits Distance::RSSI_To_DistanceUnits(double value)
{
    // Calculate the power using the logarithmic signal propagation model:
    double power = (rssi_dbm_ref_at_1_m - value) / (10.0 * env_variable);

    // Convert the power back to linear scale and scale by distance units in one meter:
    return (pow(10.0, power) * distanceUnitsInAMeter);
}

void Distance::LogInfo()
{
    ESP_LOGI("Distance", "Calculation is [%d]; [%d]/[%d]; most common is %d with standard deviation %f",
            IsCalculationRequired(), mostCommonCount, valuesCount, (int)mostCommon, standardDeviation);
    ESP_LOGI("Distance", "With 95%% certainty, distance is %f ± %f units (minumum confidence: %f)",
            mean, confidenceInterval, maximumConfidenceInterval);
    ESP_LOGI("Distance", "A unit is defined (1/%d) meters", distanceUnitsInAMeter);
    
    if(IsCalculationRequired() != 0)
    {
        for(auto value : values)
        {
            ESP_LOGI("Distance", "Value[%d], count: %d", (int)value.first, value.second);
        }
    }
}
