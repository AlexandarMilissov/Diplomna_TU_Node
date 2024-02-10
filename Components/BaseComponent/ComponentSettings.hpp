#ifndef COMPONENTSETTINGS_HPP_
#define COMPONENTSETTINGS_HPP_

#include <vector>
#include <string>
#include "Setting.hpp"

class ComponentSettings
{
private:
    std::string name;
    std::vector<Setting> settings;
public:
    ComponentSettings(std::string, std::vector<Setting>);

    std::string GetName();
    std::vector<Setting> GetSettings();
};


#endif // COMPONENTSETTINGS_HPP_
