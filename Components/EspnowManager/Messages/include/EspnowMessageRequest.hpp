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
    static uint8 GetElementsSize();
public:
    EspnowMessageRequest(Payload);
    EspnowMessageRequest(bool);
    ~EspnowMessageRequest();
    bool GetSubscriptionStatus();

    Payload GetPayload() const;
};


#endif
