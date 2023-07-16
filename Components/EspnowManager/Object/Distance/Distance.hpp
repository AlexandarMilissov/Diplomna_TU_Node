#ifndef DISTANCE_HPP_
#define DISTANCE_HPP_

#include "Common.h"

#define NUM_OF_KEPT_RECEIVED 10

typedef enum ConnectionStatus
{
    CONNECTION_OK,
    CONNECTION_NOT_OK
}ConnectionStatus;

class Distance
{
    private:
        RSSI_Type RSSI = 0;
        double RSSI_average = 0;
        uint16 received_counter = 0;
    public:
        RSSI_Type lastReceived[NUM_OF_KEPT_RECEIVED];
        Distance();
        ~Distance();
        void Receive(RSSI_Type);
        RSSI_Type GetRSSI();
        uint16 GetRequestedRepetiotions();
        void TheyFinishedSending();
        ConnectionStatus EvaluateConnection();
};

#endif // DISTANCE_HPP_