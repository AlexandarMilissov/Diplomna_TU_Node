#ifndef TASKMANAGER_HPP
#define TASKMANAGER_HPP

#include "Common.hpp"
#include "IComponent.hpp"
#include "LogManager.hpp"
#include "NvsManager.hpp"

#include "IScheduler.hpp"

class TaskManager : public IScheduler, public IComponent
{
private:

    static void TaskWrapper(void*);

    /**
     * @brief Executes the task specified by the given configuration.
     *
     * This function is responsible for executing the task based on the provided configuration.
     * It handles task repetition, timing, and termination conditions.
     *
     * @param config A pointer to the configuration structure for the task.
     */
    void Task(TaskConfig);

    LogManager& logManager;
public:
    TaskManager(LogManager& logManager) : logManager(logManager) {}

    void Init();

    /**
     * @brief Requests a task with the specified configuration.
     * @param config The configuration for the task.
     * @return A pointer to the task handle.
     */
    TaskHandle_t* RequestTask(TaskConfig);
};


#endif