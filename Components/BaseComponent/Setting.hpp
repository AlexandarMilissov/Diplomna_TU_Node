#ifndef SETTING_HPP_
#define SETTING_HPP_

#include "Common.hpp"
#include <string>
#include "ValueTypes.hpp"
#include "Payload.hpp"

class Setting
{
private:
    std::string name;
    uint64 checksum;
    ValueType type;
    Payload value;

    bool saveToNvs;
    bool updateRequired;
public:
    Setting(
        std::string,
        uint64,
        ValueType,
        Payload,
        bool,
        bool
    );

    std::string GetName();
    uint64 GetChecksum();
    ValueType GetType();
    Payload GetValue();
    bool GetSaveToNvs();
    bool GetUpdateRequired();
};

#endif // SETTING_HPP_
