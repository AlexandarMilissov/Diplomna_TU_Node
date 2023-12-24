#include "TaskConfig.hpp"
#include <stdexcept>

TaskConfig::TaskConfig(
        const char* name,
        const std::function<void()> CyclicFunction,
        const uint16 period,
        const uint8 core,
        const uint32 stack_size,
        const UBaseType_t priority,
        const bool finite,
        const uint16 repetition,
        const std::function<void()> OnCompletion
    ) :
        name(name),
        CyclicFunction(CyclicFunction),
        period(period),
        core(core),
        stack_size(stack_size),
        priority(priority),
        OnCompletion(OnCompletion),
        finite(finite),
        repetition(repetition)
{
    bool validation = true;
    if(256 > stack_size)
    {
        validation = false;
    }
    if(NULL == CyclicFunction)
    {
        validation = false;
    }

    if(portTICK_PERIOD_MS > period)
    {
        validation = false;
    }
    if(wdtTimeout <= period)
    {
        validation = false;
    }

    if(!validation)
    {
        throw std::invalid_argument("Invalid task configuration.");
    }
}

const char* TaskConfig::GetName()
{
    return name;
}

uint16 TaskConfig::GetPeriod()
{
    return period;
}

uint8 TaskConfig::GetCore()
{
    return core;
}

uint32 TaskConfig::GetStackSize()
{
    return stack_size;
}

UBaseType_t TaskConfig::GetPriority()
{
    return priority;
}

std::string TaskConfig::ToString()
{
    std::string taskConfigString = "";

    taskConfigString += "Name: " + std::string(name) + "\n";
    taskConfigString += "CyclicFunction: " + std::to_string((uint64)CyclicFunction.target<void()>()) + "\n";
    taskConfigString += "Period: " + std::to_string(period) + "\n";
    taskConfigString += "Core: " + std::to_string(core) + "\n";
    taskConfigString += "Stack size: " + std::to_string(stack_size) + "\n";
    taskConfigString += "Priority: " + std::to_string(priority) + "\n";
    taskConfigString += "Finite: " + std::to_string(finite) + "\n";
    taskConfigString += "Repetition: " + std::to_string(repetition) + "\n";
    taskConfigString += "OnCompletion: " + std::to_string((uint64)OnCompletion.target<void()>()) + "\n";

    return taskConfigString;
}

bool TaskConfig::Run()
{
    bool shouldExit = false;

    if(finite && (0 == repetition))
    {
        shouldExit = true;
        OnCompletion();
    }
    else
    {
        CyclicFunction();
    }

    repetition--;
    return shouldExit;
}
