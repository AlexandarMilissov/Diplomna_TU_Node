#ifndef RSSI_MESSAGE_REQUEST_HPP
#define RSSI_MESSAGE_REQUEST_HPP

#include "OpenSeries.hpp"
#include "Payload.hpp"
#include "IEspnowMessage.hpp"

#define SUBSCRIBE true
#define UNSUBSCRIBE false

class EspnowMessageRequest : public IEspnowMessage
{
private:
    bool subscriptionStatus = UNSUBSCRIBE;
public:
    EspnowMessageRequest(std::queue<Payload>);
    EspnowMessageRequest(bool);
    bool GetSubscriptionStatus();

    std::stack<Payload> GetPayload() const;
};


#endif
