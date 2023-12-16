#ifndef TASKMANAGER_HPP
#define TASKMANAGER_HPP

#include "Common.hpp"

/**
 * @brief Structure representing the configuration of a task.
 */
typedef struct Task_cfg_struct{
    const uint8 namePointer;                /**< Pointer to the name of the task. */
    void (*MainFunction)(const void*);      /**< Pointer to the main function of the task. */
    void* mainFunctionParams;               /**< Parameters for the main function of the task. */
    uint16 period;                          /**< Period of the task in milliseconds. */
    uint8 core;                             /**< Core on which the task should run. */
    uint32 stack_size;                      /**< Stack size for the task. */
    UBaseType_t priority;                   /**< Priority of the task. */
    bool finite;                            /**< Flag indicating if the task is finite or not. */
    uint16 repetition;                      /**< Number of times the task should repeat. */
    void (*OnComplete)(const void*);        /**< Pointer to the function to be called on task completion. */
    void* onCompleteParams;                 /**< Parameters for the onComplete function. */
}Task_cfg_struct;

class TaskManager
{
private:
    /**
     * @brief The timeout value for the watchdog timer.
     */
    const static uint16 wdtTimeout = 5000;

    /**
     * @brief Executes the task specified by the given configuration.
     *
     * This function is responsible for executing the task based on the provided configuration.
     * It handles task repetition, timing, and termination conditions.
     *
     * @param config A pointer to the configuration structure for the task.
     */
    static void Task(void*);

    /**
     * @brief Checks if the given task configuration is valid.
     *
     * @param config The task configuration to be checked.
     * @return true if the task configuration is valid, false otherwise.
     */
    static bool IsTaskCfgValid(Task_cfg_struct config);
public:
    /**
     * @brief Initializes the task manager.
     *
     * This function is responsible for initializing the task manager with the provided data.
     *
     * @param data A pointer to the data needed for initialization.
     */
    static void Init(const void*);

    /**
     * @brief Requests a task with the specified configuration.
     * @param config The configuration for the task.
     * @return A pointer to the task handle.
     */
    static TaskHandle_t* RequestTask(Task_cfg_struct* config);
};


#endif