#ifndef NVSMANAGER_HPP_
#define NVSMANAGER_HPP_

#include "nvs_flash.h"
class NvsManager
{
private:
    static nvs_handle_t my_handle;
public:
    /**
     * @brief Initializes the NvsManager with the provided data.
     *
     * @param data A pointer to the data used for initialization.
     */
    static void Init(const void*);

    /**
     * @brief Retrieves the value of a non-volatile storage (NVS) variable.
     *
     * This function retrieves the value of an NVS variable identified by its name.
     * The type of the variable is specified by the template parameter `NvsValueType`.
     *
     * @param name The name of the NVS variable.
     * @return The value of the NVS variable.
     * @tparam NvsValueType The type of the NVS variable.
     */
    template <typename NvsValueType>
    static NvsValueType GetVar(const char*);

    /**
     * @brief Sets the value of a variable in the NvsManager.
     *
     * This function allows you to set the value of a variable in the NvsManager.
     * The variable is identified by its name and the new value is provided as a pointer.
     *
     * @tparam NvsValueType The type of the variable value.
     * @param name The name of the variable.
     * @param valuePtr A pointer to the new value of the variable.
     */
    template <typename NvsValueType>
    void SetVar(const char*, NvsValueType*);
};

#endif // NVSMANAGER_HPP_
