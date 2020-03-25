//
// Created by bedom on 2/1/2020.
//

#ifndef NODEMQTT_NODEMQTTCRON_HPP
#define NODEMQTT_NODEMQTTCRON_HPP

#include "misc/NodeMQTTComponent.hpp"
#include "constants.hpp"
#include "misc/config.hpp"
#include "LinkedList.h"
#include <ctime>

extern "C" {
#include "ccronexpr.h"
}

#include <TaskSchedulerDeclarations.h>

class NodeMQTTCronClass : public NodeMQTTComponent {
public:
    NodeMQTTCronClass();

    void init() override;

    void boot() override;

    void create(const char *cronString, NodeMQTTCallback job);

protected:
    LinkedList<NodeMQTTCronJob> *jobs;

    time_t calculateNextExecution(NodeMQTTCronJob job);

    cron_expr expressionBuffer;

    void loop();

    Task _tLoop;
};

extern NodeMQTTCronClass NodeMQTTCron;

#endif //NODEMQTT_NODEMQTTCRON_HPP

