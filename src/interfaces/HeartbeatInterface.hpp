#ifndef HEARTBEATINTERFACE_H
#define HEARTBEATINTERFACE_H

#include <ESP.h>
#include "misc/helpers.hpp"
#include "_NodeInterface.hpp"

struct NodeStatus
{
    unsigned long uptime;
    uint32_t nodeID;
    uint32_t freeHeap = 0;
};

class HeartbeatInterface : public NodeInterface<NodeStatus>
{
  public:
    HeartbeatInterface(int beatIntervalSecond);

  private:
    NodeStatus sample();
    void updatePhisicalInterface(NodeStatus newValue);
    NodeStatus fromJSON(JsonObject &rootObject);
    JsonObject &toJSON(NodeStatus value);
    int cmp(NodeStatus oldValue, NodeStatus newValue);
};

inline HeartbeatInterface::HeartbeatInterface(int beatIntervalMillis = DEFAULT_HEARTBEAT_RATE) : NodeInterface<NodeStatus>(HEARTBEAT_TOPIC, HEARTBEAT_TOPIC)
{
    setSamplingEnabled(true);
    setMQTTSubscribe(false);
    setEnabled(false);
    setSampleRate(beatIntervalMillis);
    interfaceName = HEARTBEAT_TOPIC;
}

inline NodeStatus HeartbeatInterface::sample()
{
    NodeStatus status;
    status.uptime = millis();
    status.nodeID = ESP.getChipId();
    status.freeHeap = ESP.getFreeHeap();
    return status;
}

inline NodeStatus HeartbeatInterface::fromJSON(JsonObject &rootObject)
{
    return NodeStatus();
}

inline JsonObject &HeartbeatInterface::toJSON(NodeStatus status)
{
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["uptime"] = status.uptime;
    root["nodeid"] = status.nodeID;
    root["freehp"] = status.freeHeap;
    return root;
}

inline int HeartbeatInterface::cmp(NodeStatus oldValue, NodeStatus newValue)
{
    return -1;
}

inline void HeartbeatInterface::updatePhisicalInterface(NodeStatus newValue)
{
}

#endif //HEARTBEATINTERFACE_H