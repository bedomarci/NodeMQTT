//
// Created by bedom on 2/1/2020.
//

#ifndef NODEMQTT_NODEMQTTCRON_HPP
#define NODEMQTT_NODEMQTTCRON_HPP

#include "misc/typedef.hpp"
#include "constants.hpp"
#include "LinkedList.h"
#include <ctime>
extern "C" {
    #include "ccronexpr.h"
}

#include <TaskSchedulerDeclarations.h>

class NodeMQTTCronClass {
public:
    NodeMQTTCronClass();
    void init(ApplicationContext* context);
    void create(const char * cronString, NodeMQTTCallback job);

protected:
    ApplicationContext *_context;
    LinkedList<NodeMQTTCronJob> *jobs;
    time_t calculateNextExecution(NodeMQTTCronJob job);
    cron_expr expressionBuffer;
    void loop();
    Task _tLoop;
};

extern NodeMQTTCronClass NodeMQTTCron;

#endif //NODEMQTT_NODEMQTTCRON_HPP
