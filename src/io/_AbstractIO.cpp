#ifndef ABSTRACTIO_CPP
#define ABSTRACTIO_CPP

#include "_AbstractIO.hpp"
#include "../misc/typedef.hpp"

void AbstractIO::onReceive(const NodeMQTTStringCallback &cb) {
    _cb = cb;
}

void AbstractIO::receive(const char *message) {
    if (_cb) _cb(message);
}

void AbstractIO::boot(ApplicationContext *context) {
    _context = context;
    getContext()->scheduler->addTask(_task);
    _task.set(TASK_MILLISECOND, TASK_FOREVER, [this]() { this->handle(); });
    _task.enable();
}

ApplicationContext *AbstractIO::getContext() {
    return _context;
}

void AbstractIO::printPrompt() {
    print(TERMINAL_PROMPT);
}

#endif //ABSTRACTIO_CPP