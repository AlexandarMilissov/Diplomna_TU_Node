#ifndef ESPMESHSERVER_HPP_
#define ESPMESHSERVER_HPP_

#include "Common.hpp"
#include "IComponent.hpp"
#include "LogManager.hpp"
#include "IScheduler.hpp"
#include "IMonitorable.hpp"
#include "IMessageSender.hpp"
#include "IMessageReceiver.hpp"

class EspmeshServer : public IComponent, public IMessageReceiver, public IMonitorable
{
private:
    IMessageSender& lowerLayer;
    LogManager& logManager;
    IScheduler& taskManager;

public:
    EspmeshServer(
        IMessageSender& lowerLayer,
        LogManager& logManager,
        IScheduler& taskManager
        ) :
        lowerLayer(lowerLayer),
        logManager(logManager),
        taskManager(taskManager)
    {}

    void Init();

    void Receive(const MacAddress, const Payload);

    std::string GetMonitorData();
};


#endif // ESPMESHSERVER_HPP_