#ifndef BASECOMPONENT_HPP_
#define BASECOMPONENT_HPP_

#include <vector>
#include "Setting.hpp"
#include "ComponentSettings.hpp"

class BaseComponent
{
private:
    static std::vector<BaseComponent*> components;
public:

    static std::vector<BaseComponent*> GetComponents();

    BaseComponent();
    ~BaseComponent();

    virtual void Init(){};
    virtual std::string GetName(){ return "Unkown Component";};
    virtual std::vector<Setting> GetComponentGlobalSettings(){ return std::vector<Setting>();};
    virtual std::vector<Setting> GetComponentLocalSettings(){ return std::vector<Setting>();};

    static std::vector<ComponentSettings> GetAllGlobalSettings();
    static std::vector<ComponentSettings> GetAllLocalSettings();
};

#endif // BASECOMPONENT_HPP_
