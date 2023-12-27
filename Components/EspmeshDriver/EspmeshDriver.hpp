#ifndef ESPMESHDRIVER_HPP_
#define ESPMESHDRIVER_HPP_

#include "Common.hpp"
#include "IComponent.hpp"
#include "IDriver.hpp"
#include "LogManager.hpp"

class EspmeshDriver : public IComponent, public IDriver
{
private:
    LogManager& logManager;
public:
    EspmeshDriver(
        LogManager& logManager
        ) :
        logManager(logManager)
        {}

    void Init();

    void Subscribe(IMessageable& component);
    void Send(const Payload, const Payload);
    void Receive(const Payload*, const Payload*);

};


#endif // ESPMESHDRIVER_HPP_
