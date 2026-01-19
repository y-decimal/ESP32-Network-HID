#ifndef ITASK_H
#define ITASK_H

#include <FreeRTOS.h>

class ITask
{
public:
    struct TaskParameters
    {
        uint32_t stackSize;
        UBaseType_t priority;
        BaseType_t coreAffinity;
    };

    virtual ~ITask() {}
    virtual void start(TaskParameters params) = 0;
    virtual void stop() = 0;
    virtual void restart(TaskParameters params) = 0;
};

#endif