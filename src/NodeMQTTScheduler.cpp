#include "NodeMQTTScheduler.hpp"
#include "misc/typedef.hpp"

NodeMQTTSchedulerClass::NodeMQTTSchedulerClass() {

    tasks = LinkedList<Task *>();
}

void NodeMQTTSchedulerClass::init(ApplicationContext *context)
{
    _context = context;
    // tCleanUp.set(TASK_IMMEDIATE, TASK_ONCE, [=]{this->cleanUp();});
    // this->_context->scheduler->addTask(tCleanUp);
}

void NodeMQTTSchedulerClass::runDelayed(NodeMQTTCallback callback, uint32_t delay)
{
    Task *thisTask = new Task(TASK_IMMEDIATE, TASK_ONCE);
//    this->tasks.add(thisTask);
    thisTask->setCallback([=](){
        callback();
        this->_context->scheduler->deleteTask(*thisTask);
        free(thisTask);
    });
    this->_context->scheduler->addTask(*thisTask);
    thisTask->restartDelayed(delay);
}

// void NodeMQTTSchedulerClass::cleanUp() {
    // for (int i = 0; i < tasksToDelete.size(); i++) {
    //     Task *thisTask = tasksToDelete.remove(i);
    //     w(thisTask->getRunCounter());
    //     this->_context->scheduler->deleteTask(*thisTask);
    //     free(thisTask);
    // }
// }

NodeMQTTSchedulerClass NodeMQTTScheduler;