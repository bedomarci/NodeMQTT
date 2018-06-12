#ifndef HEARTBEATINTERFACE_H
#define HEARTBEATINTERFACE_H

#include <ESP.h>
#include "misc/helpers.hpp"
#include "_NodeInterface.hpp"

struct NodeStatus
{
    uint32_t uptime = 0;
    uint32_t nodeID = 0;
    uint32_t freeHeap = 0;
};

class HeartbeatInterface : public NodeInterface<NodeStatus>
{
  public:
    HeartbeatInterface(int beatIntervalSecond);
    void init();

  protected:
    NodeStatus status;
    NodeStatus sample();
    void updatePhisicalInterface(NodeStatus newValue);
    NodeStatus fromJSON(JsonObject &root);
    JsonObject &toJSON(NodeStatus value, JsonObject &root);
    int cmp(NodeStatus oldValue, NodeStatus newValue);

    const char *uptimeKey = "uptime";
    const char *nodeIdKey = "nodeid";
    const char *freeHeapKey = "freehp";
};

inline HeartbeatInterface::HeartbeatInterface(int beatIntervalMillis = DEFAULT_HEARTBEAT_RATE)
    : NodeInterface<NodeStatus>(HEARTBEAT_TOPIC, HEARTBEAT_TOPIC)
{
    setEnabled(false);
    setSamplingRate(beatIntervalMillis);
    _interfaceName = HEARTBEAT_TOPIC;
}

inline void HeartbeatInterface::init()
{
    setSamplingEnabled(true);
    setMQTTSubscribe(false);
#ifdef ESP8266
    status.nodeID = ESP.getChipId();
#endif
#ifdef ESP32
    status.nodeID = (uint32_t)ESP.getEfuseMac();
#endif
}

inline NodeStatus HeartbeatInterface::sample()
{
    status.uptime = millis();
    status.freeHeap = ESP.getFreeHeap();
    return status;
}

inline NodeStatus HeartbeatInterface::fromJSON(JsonObject &rootObject)
{
    return NodeStatus();
}

inline JsonObject &HeartbeatInterface::toJSON(NodeStatus status, JsonObject &root)
{
    root[uptimeKey] = status.uptime;
    root[freeHeapKey] = status.freeHeap;
    root[nodeIdKey] = status.nodeID;
    // root.set(uptimeKey, status.uptime);
    // root.set(freeHeapKey , status.freeHeap);
    // root.set(nodeIdKey , status.nodeID);
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