//
// Created by bedom on 2/1/2020.
//

#include "NodeMQTTCron.hpp"
#include "misc/typedef.hpp"

extern "C" {
#include <ccronexpr.h>
}


NodeMQTTCronClass::NodeMQTTCronClass() {
    jobs = new LinkedList<NodeMQTTCronJob>();
}

void NodeMQTTCronClass::init() {
    getContext()->scheduler->addTask(_tLoop);
    _tLoop.set(TASK_SECOND, TASK_FOREVER, [this]() { this->loop(); });
    _tLoop.enable();
}

void NodeMQTTCronClass::boot() {

}

void NodeMQTTCronClass::create(const char *cronString, NodeMQTTCallback cb) {
    NodeMQTTCronJob job;
    job.cronString = cronString;
    job.cb = cb;
    job.enabled = true;
    const char *err = NULL;
    memset(&(job.cronExpression), 0, sizeof(job.cronExpression));
    cron_expr *expression = cron_parse_expr(job.cronString, &err);
    job.cronExpression = *expression;

    if (err) {
        e(err);
        job.enabled = false;
        memset(&(job.cronExpression), 0, sizeof(job.cronExpression));
    }
    job.nextExecution = this->calculateNextExecution(job);
    jobs->add(job);
}

void NodeMQTTCronClass::loop() {
    for (int i = 0; i < this->jobs->size(); i++) {

        NodeMQTTCronJob job = this->jobs->get(i);
        if (this->getContext()->currentTime == job.nextExecution && job.enabled) {
            job.cb();
        }

        if (this->getContext()->currentTime >= job.nextExecution) {
            job.nextExecution = this->calculateNextExecution(job);
            this->jobs->set(i, job);
        }
    }
}

time_t NodeMQTTCronClass::calculateNextExecution(NodeMQTTCronJob job) {
    cron_expr expression = job.cronExpression;
    time_t next = cron_next(&expression, this->getContext()->currentTime);
    return next;
}

NodeMQTTCronClass NodeMQTTCron;