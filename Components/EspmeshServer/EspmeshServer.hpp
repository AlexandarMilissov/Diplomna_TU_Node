#ifndef ESPMESHSERVER_HPP_
#define ESPMESHSERVER_HPP_

#include "Common.hpp"
#include "IComponent.hpp"
#include "IDriver.hpp"
#include "LogManager.hpp"
#include "IScheduler.hpp"
#include "IMonitorable.hpp"
#include "IMessageable.hpp"

class EspmeshServer : public IComponent, public IMessageable, public IMonitorable
{
private:
    IDriver& driver;
    LogManager& logManager;
    IScheduler& taskManager;

public:
    EspmeshServer(
        IDriver& driver,
        LogManager& logManager,
        IScheduler& taskManager
        ) :
        driver(driver),
        logManager(logManager),
        taskManager(taskManager)
    {}

    void Init();

    void Send(const Payload, const Payload);
    void SendBroadcast(const Payload);
    void Receive(const Payload*, const Payload*);

    std::string GetMonitorData();
};


#endif // ESPMESHSERVER_HPP_