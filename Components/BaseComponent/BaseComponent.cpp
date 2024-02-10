#include "BaseComponent.hpp"

std::vector<BaseComponent*> BaseComponent::components;

std::vector<BaseComponent*> BaseComponent::GetComponents()
{
    return components;
}

BaseComponent::BaseComponent()
{
    components.push_back(this);
}

BaseComponent::~BaseComponent()
{
    for (std::vector<BaseComponent*>::iterator it = components.begin(); it != components.end(); ++it)
    {
        if (*it == this)
        {
            components.erase(it);
            break;
        }
    }
}

std::vector<ComponentSettings> BaseComponent::GetAllGlobalSettings()
{
    std::vector<ComponentSettings> allGlobalSettings;
    for (std::vector<BaseComponent*>::iterator it = components.begin(); it != components.end(); ++it)
    {
        BaseComponent* component = *it;
        ComponentSettings componentSettings(component->GetName(), component->GetComponentGlobalSettings());
        allGlobalSettings.push_back(componentSettings);
    }
    return allGlobalSettings;
}

std::vector<ComponentSettings> BaseComponent::GetAllLocalSettings()
{
    std::vector<ComponentSettings> allLocalSettings;
    for (std::vector<BaseComponent*>::iterator it = components.begin(); it != components.end(); ++it)
    {
        BaseComponent* component = *it;
        ComponentSettings componentSettings(component->GetName(), component->GetComponentLocalSettings());
        allLocalSettings.push_back(componentSettings);
    }
    return allLocalSettings;
}
