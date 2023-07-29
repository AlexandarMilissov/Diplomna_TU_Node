#ifndef RSSI_Message_Interface_HPP
#define RSSI_Message_Interface_HPP

#include "Common.h"
#include "To_C_Encapsulation.h"
#include <exception>

class RSSI_Message_Interface
{
    private:
        virtual uint8 GetElementsSize() = 0;
    public:
        virtual void Send() = 0;
};

class ValidationFailedException : public std::exception
{

};

#endif