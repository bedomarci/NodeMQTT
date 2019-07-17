#ifndef NODESCHEDULER_H
#define NODESCHEDULER_H

#include <Arduino.h>
#include "misc/helpers.hpp"
#include "misc/config.hpp"
#include "constants.hpp"
#include <TaskScheduler.h>

class NodeMQTTSchedulerClass
{
public:
    NodeMQTTSchedulerClass();
    void init(ApplicationContext *context);
    void runDelayed(NodeMQTTCallback callback, uint16_t delay);

protected:
    LinkedList<Task *> tasksToDelete;
    ApplicationContext *_context;
    // void cleanUp();
    // Task tCleanUp;
};

extern NodeMQTTSchedulerClass NodeMQTTScheduler;

#endif //NODESCHEDULER_H