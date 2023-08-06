#include "Distance.hpp"
#include "Distance_ref.hpp"

const float maximumConfidenceInterval = (float)CONFIG_MINIMUM_CONFIDANCE_INTERVAL / 2.0;

void Distance::AddSeriesSafe(ClosedSeries* cs)
{
    // Add the value to the map
    // If the value has been added before
    // Increace the number of occurances by 1
    // If its the first time we see the value
    // Add it and set the numbcer of occurences to 1
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
    // Increace the mostCommonCount
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

    // Calculate standart diviation (σ)
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

void Distance::CalculateConfindenceInterval()
{
    confindenceInterval = standardDeviation / sqrt(valuesCount);
    confindenceInterval *= z95;
}

Distance::Distance()
{
}

Distance::~Distance()
{
}

uint8 Distance::GetRequestedRepetiotions()
{
    return OpenSeries::numberOfMessagesPerSeries;
}

bool Distance::IsCalculationRequired()
{
    if(minNumberOfValues > valuesCount)
    {
        return true;
    }
    if(valuesCount > maxNumberOfValues)
    {
        return true;
    }
    if(confindenceInterval > maximumConfidenceInterval)
    {
        return true;
    }
    
    return false;
}

void Distance::AddSeries(ClosedSeries* cs)
{
    // Check if the added value is valid
    // If it's not then discard it
    if(ClosedSeries::defaltValue == *cs)
    {
        ESP_LOGW("Distance", "Series with default value received");
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
        ESP_LOGI("Distance", "Not enough values for calculation");
    }
    
    CalculateStandardDeviation();
    CalculateConfindenceInterval();
}

DistanceUnits Distance::RSSI_To_DistanceUnits(double value)
{
    double power = (rssi_dbm_ref_at_1_m - value) / (10.0 * env_variable);
    return (pow(10.0, power) * distanceUnitsInAMeter);
}

void Distance::LogInfo()
{
    ESP_LOGI("Distance", "=====================");
    ESP_LOGI("Distance", "Recalculation; [%d]/[%d]; most common is %d with standard deviation %f",
            mostCommonCount, valuesCount, (int)mostCommon, standardDeviation);
    ESP_LOGI("Distance", "With 95%% certainty, distance is %f ± %funits (minumum confidance: %f)",
            mean, confindenceInterval, maximumConfidenceInterval);
    for(auto value : values)
    {
        ESP_LOGI("Distance", "Value[%d], count: %d", (int)value.first, value.second);
    }
    ESP_LOGI("Distance", "=====================");
}
