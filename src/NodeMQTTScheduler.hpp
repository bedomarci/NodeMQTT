#ifndef NODESCHEDULER_H
#define NODESCHEDULER_H

#include <Arduino.h>
#include "misc/helpers.hpp"
#include "misc/config.hpp"
#include "constants.hpp"
#include "misc/typedefDeclaration.hpp"
#include <TaskSchedulerDeclarations.h>

class NodeMQTTSchedulerClass
{
public:
    NodeMQTTSchedulerClass();
    void init(ApplicationContext *context);
    void runDelayed(NodeMQTTCallback callback, uint32_t delay);

protected:
    uint8_t getNextTaskId();
    NodeMQTTScheduledTask getNextTask();

    void prepareNextTask();

    LinkedList<NodeMQTTScheduledTask> *tasks;
    ApplicationContext *_context;
    NodeMQTTCallback nextTaskCb = nullptr;
    time_t nextTaskExecution;
    uint8_t nextTaskId;
    Task *executor;
};

extern NodeMQTTSchedulerClass NodeMQTTScheduler;

#endif //NODESCHEDULER_H