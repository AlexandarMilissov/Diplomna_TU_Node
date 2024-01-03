#ifndef IDRIVER_HPP_
#define IDRIVER_HPP_

#include "IMessageable.hpp"

class IDriver : virtual public IMessageable
{
private:
public:
    virtual void Subscribe(IMessageable&) = 0;
};


#endif // IDRIVER_HPP_
