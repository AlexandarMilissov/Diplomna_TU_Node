#include "ComponentSettings.hpp"

ComponentSettings::ComponentSettings(std::string name, std::vector<Setting> settings)
{
    this->name = name;
    this->settings = settings;
}

std::string ComponentSettings::GetName()
{
    return name;
}

std::vector<Setting> ComponentSettings::GetSettings()
{
    return settings;
}
