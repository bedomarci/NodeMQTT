#include "NodeMQTTScheduler.hpp"
#include <misc/typedef.hpp>
#include <TaskScheduler.h>
#include <limits.h>

NodeMQTTSchedulerClass::NodeMQTTSchedulerClass() {

    tasks = new LinkedList<NodeMQTTScheduledTask>();
}

void NodeMQTTSchedulerClass::init(ApplicationContext *context) {
    _context = context;
    executor = new Task(TASK_IMMEDIATE, TASK_ONCE);
    executor->setCallback([=]() {
        if (this->nextTaskCb) {
            this->nextTaskCb();
        }
        this->tasks->remove(nextTaskId);
        if (this->tasks->size() > 0) {
            this->prepareNextTask();
        }
    });
    this->_context->scheduler->addTask(*executor);
}

void NodeMQTTSchedulerClass::runDelayed(NodeMQTTCallback callback, uint32_t delay) {
    NodeMQTTScheduledTask thisTask = NodeMQTTScheduledTask();
    thisTask.cb = callback;
    thisTask.nextExecution = millis() + delay;
    this->tasks->add(thisTask);
    this->prepareNextTask();
}

void NodeMQTTSchedulerClass::prepareNextTask() {
    nextTaskId = getNextTaskId();
    NodeMQTTScheduledTask nextTask = tasks->get(nextTaskId);
    nextTaskCb = nextTask.cb;
    nextTaskExecution = nextTask.nextExecution;
    executor->restartDelayed(nextTask.nextExecution - millis());
}

uint8_t NodeMQTTSchedulerClass::getNextTaskId() {
    time_t closest = LONG_MIN;
    uint8_t closestId = 0;
    time_t now = millis();
    NodeMQTTScheduledTask current;
    for (int i = 0; i < this->tasks->size(); i++) {
        current = tasks->get(i);
        if ((now - closest) > (now - current.nextExecution)) {
            closest = current.nextExecution;
            closestId = i;
        }
    }
    return closestId;
}

NodeMQTTScheduledTask NodeMQTTSchedulerClass::getNextTask() {
    return this->tasks->get(getNextTaskId());
}


NodeMQTTSchedulerClass NodeMQTTScheduler;