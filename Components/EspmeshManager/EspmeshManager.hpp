#ifndef ESPMESHMANAGER_HPP_
#define ESPMESHMANAGER_HPP_

#include "Common.hpp"

#include "IScheduler.hpp"
#include "IComponent.hpp"
#include "IEspnowController.hpp"
#include "IMessageable.hpp"
#include "IDriver.hpp"
#include "IMonitorable.hpp"
#include "IScheduler.hpp"
#include "LogManager.hpp"

#include <string>

class EspmeshManager : public IComponent, public IMessageable, public IMonitorable
{
private:
    IDriver& driver;
    IEspnowController& espnowController;
    LogManager& logManager;
    IScheduler& taskManager;
    void MainFunction();
public:
    EspmeshManager(
        IDriver& driver,
        IEspnowController& espnowController,
        LogManager& logManager,
        IScheduler& taskManager
        ) :
        driver(driver),
        espnowController(espnowController),
        logManager(logManager),
        taskManager(taskManager)
        {}
    ~EspmeshManager() {}
    void Init();

    void Send(const Payload, const Payload);
    void Receive(const Payload*, const Payload*);

    std::string GetMonitorData();
};

#endif // ESPMESHMANAGER_HPP_
