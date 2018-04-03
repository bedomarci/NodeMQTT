#ifndef HEARTBEATINTERFACE_H
#define HEARTBEATINTERFACE_H

#include <ESP.h>
#include "misc/helpers.hpp"
#include "_NodeInterface.hpp"

struct NodeStatus {
  unsigned long uptime;
  const char * nodeID;
  uint32_t freeHeap = 0;
};

class HeartbeatInterface : public NodeInterface<NodeStatus> {
    public:
        HeartbeatInterface(int beatIntervalSecond);
    private:
        NodeStatus sample();
        void updatePhisicalInterface(NodeStatus newValue);
        NodeStatus fromJSON (JsonObject& rootObject);
        JsonObject& toJSON (NodeStatus value);
        int cmp(NodeStatus oldValue, NodeStatus newValue);
};


inline
HeartbeatInterface::HeartbeatInterface(int beatIntervalMillis): NodeInterface<NodeStatus>(HEARTBEAT_TOPIC, HEARTBEAT_TOPIC) {
    setSamplingEnabled(true);
    setMQTTSubscribe(false);
    enable(false);
    setSampleRate(beatIntervalMillis);
    interfaceName = HEARTBEAT_TOPIC;
}

inline
NodeStatus HeartbeatInterface::sample() {
    NodeStatus status;
    status.uptime = millis();
    status.nodeID = DEVICE_NAME.c_str(); 
    status.freeHeap = ESP.getFreeHeap();
    return status;
}

inline
NodeStatus HeartbeatInterface::fromJSON (JsonObject& rootObject){
    return NodeStatus();
}

inline
JsonObject& HeartbeatInterface::toJSON (NodeStatus status){
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["uptime"] = status.uptime;
    root["nodeid"] = status.nodeID;
    root["freehp"] = status.freeHeap;
    return root;
}

inline
int HeartbeatInterface::cmp(NodeStatus oldValue, NodeStatus newValue){
    return -1;
}

inline
void HeartbeatInterface::updatePhisicalInterface(NodeStatus newValue)
{

}


#endif //HEARTBEATINTERFACE_H