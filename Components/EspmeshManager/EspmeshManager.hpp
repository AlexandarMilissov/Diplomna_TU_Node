#ifndef ESPMESHMANAGER_HPP_
#define ESPMESHMANAGER_HPP_

#include "IScheduler.hpp"
#include "IComponent.hpp"
#include "IEspnowController.hpp"

class EspmeshManager : public IComponent
{
private:
    IEspnowController& espnowController;
    IScheduler& taskManager;
    void MainFunction();
public:
    EspmeshManager(
        IEspnowController& espnowController,
        IScheduler& taskManager
        ) :
        espnowController(espnowController),
        taskManager(taskManager)
        {}
    void Init();
};

#endif // ESPMESHMANAGER_HPP_
