#ifndef ABSTRACTIO_H
#define ABSTRACTIO_H

#include <Arduino.h>
#include <Print.h>
#include "../misc/typedef.hpp"

#include <TaskSchedulerDeclarations.h>

class AbstractIO : public Print
{
public:
    void onReceive(const NodeMQTTStringCallback& cb);
    size_t write(uint8_t c) override = 0;
    virtual void init(ApplicationContext * context);
protected:
    virtual void handle() = 0;
    virtual void printHeader() = 0;

    void printPrompt();
    ApplicationContext * getContext();
    void receive(const char * message);
    Task _task;
private:
    ApplicationContext *_context;
    NodeMQTTStringCallback _cb = nullptr;
};

inline void AbstractIO::onReceive(const NodeMQTTStringCallback& cb) {
    _cb = cb;
}

inline void AbstractIO::receive(const char * message) {
    if (_cb) _cb(message);
}

inline void AbstractIO::init(ApplicationContext * context) {
    _context = context;
    getContext()->scheduler->addTask(_task);
    _task.set(TASK_IMMEDIATE, TASK_FOREVER, [this]() { this->handle(); });
    _task.enable();
}

inline ApplicationContext * AbstractIO::getContext() {
    return _context;
}

inline void AbstractIO::printPrompt() {
    print(TERMINAL_PROMPT);
}

#endif //ABSTRACTIO_H