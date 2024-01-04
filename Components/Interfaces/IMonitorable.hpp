#ifndef IMONITORABLE_HPP_
#define IMONITORABLE_HPP_

#include <string>

class IMonitorable
{
private:
    /* data */
public:
    virtual std::string GetMonitorData() = 0;
    virtual ~IMonitorable() = default;
};

#endif // IMONITORABLE_HPP_
