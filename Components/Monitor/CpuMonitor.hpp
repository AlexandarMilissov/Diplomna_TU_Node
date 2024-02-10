#ifndef CPUMONITOR_HPP_
#define CPUMONITOR_HPP_

#include "IMonitorable.hpp"
#include "BaseComponent.hpp"
#include <string>

class CpuMonitor : public BaseComponent , public IMonitorable
{
public:
    std::string GetMonitorData();
};


#endif // CPUMONITOR_HPP_