#include "NodeMQTTEventHandler.hpp"


NodeMQTTEventHandlerClass::NodeMQTTEventHandlerClass() {
    listeners = new LinkedList<NodeMQTTEvent>();
}

void NodeMQTTEventHandlerClass::init() {

}

void NodeMQTTEventHandlerClass::boot() {

}

void NodeMQTTEventHandlerClass::triggerEvent(uint16_t eventId) {
    for (int i = 0; i < listeners->size(); i++) {
        NodeMQTTEvent event = listeners->get(i);
        if (event.eventId == eventId)
            event.cb();
    }
}

void NodeMQTTEventHandlerClass::addListener(uint16_t eventId, const NodeMQTTCallback &cb) {
    NodeMQTTEvent event;
    event.eventId = eventId;
    event.cb = cb;
    listeners->add(event);
}

NodeMQTTEventHandlerClass NodeMQTTEventHandler;
