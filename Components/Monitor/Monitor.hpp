#ifndef MONITOR_HPP_
#define MONITOR_HPP_

#include <vector>
#include <string>
#include "Common.hpp"
#include "IComponent.hpp"
#include "LogManager.hpp"
#include "IScheduler.hpp"
#include "IMonitorable.hpp"

class Monitor : public IComponent
{
private:
    std::vector<IMonitorable*> monitorables;
    static size_t maximum_heap_regions;
    static size_t free_heap_regions;
    LogManager& logManager;
    IScheduler& scheduler;

    uint16 counter = 0;
public:
    Monitor(
        LogManager& logManager,
        IScheduler& scheduler
        ) :
        logManager(logManager),
        scheduler(scheduler)
        {}

    void Init();
    void MainFunction();
    void Subscribe(IMonitorable*);
    void Unsubscribe(IMonitorable*);
};

#endif // MONITOR_HPP_
