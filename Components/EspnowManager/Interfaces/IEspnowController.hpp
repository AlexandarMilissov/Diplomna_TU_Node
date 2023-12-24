#ifndef IESPNOWCONTROLLER_HPP_
#define IESPNOWCONTROLLER_HPP_

class IEspnowController
{
private:
public:
    virtual void ActivateNetwork() = 0;
    virtual void DeactivateNetwork() = 0;
    virtual void Subscribe() = 0;
    virtual void Unsubscribe() = 0;
};


#endif // IESPNOWCONTROLLER_HPP_