#ifndef RSSI_MESSAGE_INTERFACE_HPP_
#define RSSI_MESSAGE_INTERFACE_HPP_

class EspnowMessageInteface
{
    public:
        virtual void Send(uint8*) = 0;
        virtual ~EspnowMessageInteface() = default;
};

#endif // RSSI_MESSAGE_INTERFACE_HPP_
