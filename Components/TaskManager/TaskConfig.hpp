#ifndef TASKCONFIG_HPP_
#define TASKCONFIG_HPP_

#include "Common.hpp"
#include <functional>
#include <string>

class TaskConfig
{
private:
    const static uint16 wdtTimeout = 5000;

    const char* name;                           /**< Name of the task. */
    const std::function<void()> CyclicFunction; /**< The cyclic function the task calls. */
    const uint16 period;                        /**< Period of the task in milliseconds. */
    const uint8 core;                           /**< Core on which the task should run. */
    const uint32 stack_size;                    /**< Stack size for the task. */
    const UBaseType_t priority;                 /**< Priority of the task. */
    const std::function<void()> OnCompletion;   /**< The function that should be called upon completion. */
    const bool finite;                          /**< Flag indicating if the task is finite or not. */
    uint16 repetition;                          /**< Number of times the task should repeat. */
public:
    TaskConfig(
        const char *,
        const std::function<void()>,
        const uint16,
        const uint8,
        const uint32,
        const UBaseType_t,
        const bool = false,
        const uint16 = 0,
        const std::function<void()> = [](){}
    );

    const char* GetName();
    uint16 GetPeriod();
    uint8 GetCore();
    uint32 GetStackSize();
    UBaseType_t GetPriority();
    std::string ToString();

    bool Run();
};



#endif // TASKCONFIG_HPP_