#ifndef ISCHEDULER_HPP_
#define ISCHEDULER_HPP_

#include "Common.hpp"
#include "TaskConfig.hpp"
#include <functional>

class IScheduler {
public:
    virtual ~IScheduler() {}

    virtual TaskHandle_t* RequestTask(TaskConfig) = 0;
};

#endif // ISCHEDULER_HPP_
