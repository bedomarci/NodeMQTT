//
// Created by bedom on 2/1/2020.
//

#include "NodeMQTTCron.hpp"

NodeMQTTCronClass::NodeMQTTCronClass() {
    jobs = new LinkedList<NodeMQTTCronJob>();

}

void NodeMQTTCronClass::init(ApplicationContext *context) {
    _context = context;
    _context->scheduler->addTask(_tLoop);
    _tLoop.set(TASK_SECOND, TASK_FOREVER, [this]() { this->loop(); });
    _tLoop.enable();
}

void NodeMQTTCronClass::create(const char *cronString, NodeMQTTCallback cb) {
    NodeMQTTCronJob job;
    job.cronString = cronString;
    job.cb = cb;
    job.enabled = true;
    memset(&(job.cronExpression), 0, sizeof(job.cronExpression));
    const char *err = NULL;
    cron_parse_expr(cronString, &(job.cronExpression), &err);
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
        if (this->_context->currentTime == job.nextExecution && job.enabled) {
            job.cb();
        }
        if (this->_context->currentTime >= job.nextExecution) {
            job.nextExecution = this->calculateNextExecution(job);
            this->jobs->set(i, job);
        }
    }
}

time_t NodeMQTTCronClass::calculateNextExecution(NodeMQTTCronJob job) {
    cron_expr expression = job.cronExpression;
    time_t next = cron_next(&expression, this->_context->currentTime);
    return next;
}

NodeMQTTCronClass NodeMQTTCron;