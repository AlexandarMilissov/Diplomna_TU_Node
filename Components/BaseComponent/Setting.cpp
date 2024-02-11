#include "Setting.hpp"

Setting::Setting(
    std::string name,
    uint64 checksum,
    ValueType type,
    Payload value,
    bool saveToNvs,
    bool updateRequired
) :
    name(name),
    checksum(checksum),
    type(type),
    value(value),
    saveToNvs(saveToNvs),
    updateRequired(updateRequired)
{}

std::string Setting::GetName()
{
    return name;
}

uint64 Setting::GetChecksum()
{
    return checksum;
}

ValueType Setting::GetType()
{
    return type;
}

Payload Setting::GetValue()
{
    return value;
}

bool Setting::GetSaveToNvs()
{
    return saveToNvs;
}

bool Setting::GetUpdateRequired()
{
    return updateRequired;
}
