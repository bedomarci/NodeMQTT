#ifndef NODEMQTT_NODEMQTTEVENTHANDLER_HPP
#define NODEMQTT_NODEMQTTEVENTHANDLER_HPP


#include <misc/NodeMQTTComponent.hpp>
#include <LinkedList.h>


struct NodeMQTTEvent {
    uint16_t eventId;
    NodeMQTTCallback cb;
};

class NodeMQTTEventHandlerClass : public NodeMQTTComponent {
public:
    NodeMQTTEventHandlerClass();

    void init() override;

    void boot() override;

    void triggerEvent(uint16_t eventId);

    void addListener(uint16_t eventId, const NodeMQTTCallback &cb);

private:
    LinkedList<NodeMQTTEvent> *listeners;
};


extern NodeMQTTEventHandlerClass NodeMQTTEventHandler;

#endif //NODEMQTT_NODEMQTTEVENTHANDLER_HPP
