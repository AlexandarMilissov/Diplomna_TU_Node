#ifndef RAMMONITOR_HPP_
#define RAMMONITOR_HPP_

#include "IMonitorable.hpp"
#include "IComponent.hpp"

class RamMonitor : public IMonitorable, public IComponent
{
private:
    size_t maximum_heap_regions;
public:
    RamMonitor(/* args */) {}
    ~RamMonitor() {}

    void Init();
    std::string GetMonitorData();
};

#endif // RAMMONITOR_HPP_
