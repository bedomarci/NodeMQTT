#ifndef NODESCHEDULER_H
#define NODESCHEDULER_H

#include <Arduino.h>
#include "misc/helpers.hpp"
#include "misc/config.hpp"
#include "constants.hpp"
#include "misc/typedefDeclaration.hpp"
#include "misc/NodeMQTTComponent.hpp"
#include <TaskSchedulerDeclarations.h>

class NodeMQTTSchedulerClass : public NodeMQTTComponent
{
public:
    NodeMQTTSchedulerClass();

    void init() override;

    void boot() override;
    void runDelayed(NodeMQTTCallback callback, uint32_t delay);

protected:
    uint8_t getNextTaskId();
    NodeMQTTScheduledTask getNextTask();

    void prepareNextTask();

    LinkedList<NodeMQTTScheduledTask> *tasks;
    NodeMQTTCallback nextTaskCb = nullptr;
    time_t nextTaskExecution;
    uint8_t nextTaskId;
    Task *executor;
};

extern NodeMQTTSchedulerClass NodeMQTTScheduler;

#endif //NODESCHEDULER_H