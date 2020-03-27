// #error "ITT EGY KIS ERROR VAN"
#include "_AbstractTransport.hpp"
#include "../misc/typedef.hpp"

void AbstractTransport::setContext(ApplicationContext *context) {
    _context = context;
}

Scheduler *AbstractTransport::getScheduler() {
    return _context->scheduler;
}

void AbstractTransport::setMessageCallback(NodeMQTTMessageCallback cb) {
    messageCallback = cb;
}

void AbstractTransport::onMessage(char *topic, uint8_t *payload, unsigned int length) {
    if (messageCallback != nullptr)
        messageCallback(topic, payload, length);
}

String AbstractTransport::getState() {
    return "TRANSPORT STATE N/A";
}