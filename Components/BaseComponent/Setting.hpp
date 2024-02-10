#ifndef SETTING_HPP_
#define SETTING_HPP_

#include <string>
#include "ValueTypes.hpp"

class Setting
{
private:
    std::string name;
    void* id;
    ValueType type;
    bool saveToNvs;
    bool updateRequired;
public:
    Setting(
        std::string,
        void*,
        ValueType,
        bool,
        bool
    );

    std::string GetName();
    void* GetId();
    ValueType GetType();
    bool GetSaveToNvs();
    bool GetUpdateRequired();
};

#endif // SETTING_HPP_
