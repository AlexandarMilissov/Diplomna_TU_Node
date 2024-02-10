#include "Setting.hpp"

Setting::Setting(
    std::string name,
    void* id,
    ValueType type,
    bool saveToNvs,
    bool updateRequired
)
{
    this->name = name;
    this->id = id;
    this->type = type;
    this->saveToNvs = saveToNvs;
    this->updateRequired = updateRequired;
}

std::string Setting::GetName()
{
    return name;
}

void* Setting::GetId()
{
    return id;
}

ValueType Setting::GetType()
{
    return type;
}

bool Setting::GetSaveToNvs()
{
    return saveToNvs;
}

bool Setting::GetUpdateRequired()
{
    return updateRequired;
}
