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
    LogManager& logManager;
    IScheduler& taskManager;
    IMessageSender& lowerLayer;

public:
    EspmeshServer(
        LogManager& logManager,
        IScheduler& taskManager,
        IMessageSender& lowerLayer
        ) :
        logManager(logManager),
        taskManager(taskManager),
        lowerLayer(lowerLayer)
    {}

    void Init();

    void Receive(const MacAddress, const std::queue<Payload>);

    std::string GetMonitorData();
};


#endif // ESPMESHSERVER_HPP_