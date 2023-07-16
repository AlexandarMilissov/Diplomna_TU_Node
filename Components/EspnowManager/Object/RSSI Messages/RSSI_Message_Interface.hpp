#ifndef RSSI_Message_Interface_HPP
#define RSSI_Message_Interface_HPP

#include "Common.h"
#include "To_C_Encapsulation.h"
#include <exception>

typedef enum RSSI_Calculation_Request_Status
{
    ONGOING,
    NOT_REQUESTED
}RSSI_Calculation_Request_Status;
 
class RSSI_Message_Interface
{
    private:
        virtual uint8 GetElementsSize() = 0;
    public:
        virtual void Send() = 0;
        static RSSI_Calculation_Request_Status rssi_Calculation_Request_Status;
};

class ValidationFailedException : public std::exception
{

};

#endif