#ifndef CPUMONITOR_HPP_
#define CPUMONITOR_HPP_

#include "IMonitorable.hpp"
#include "IComponent.hpp"
#include <string>

class CpuMonitor : public IComponent , public IMonitorable
{
private:
    /* data */
public:
    CpuMonitor(/* args */);
    ~CpuMonitor();

    void Init();

    std::string GetMonitorData();
};


#endif // CPUMONITOR_HPP_