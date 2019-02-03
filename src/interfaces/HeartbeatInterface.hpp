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
    char networkAddress[16];
};

class HeartbeatInterface : public NodeInterface<NodeStatus>
{
  public:
    HeartbeatInterface(int beatIntervalSecond);
    void init();
    void valueToString(String &sValue);

  protected:
    NodeStatus status;
    virtual NodeStatus sample() override;
    virtual void updatePhisicalInterface(NodeStatus newValue) override;
    virtual NodeStatus fromJSON(JsonObject &root) override;
    virtual JsonObject &toJSON(NodeStatus value, JsonObject &root) override;
    virtual int cmp(NodeStatus oldValue, NodeStatus newValue) override;
    // virtual void valueToString(NodeStatus value, String &sValue) override;

    const char *uptimeKey = "uptime";
    const char *nodeIdKey = "nodeid";
    const char *freeHeapKey = "freehp";
    const char *networkAddressKey = "netadd";
};

inline HeartbeatInterface::HeartbeatInterface(int beatIntervalMillis = DEFAULT_HEARTBEAT_RATE)
    : NodeInterface<NodeStatus>(HEARTBEAT_TOPIC, HEARTBEAT_TOPIC)
{
    this->setEnabled(false);
    this->setSamplingEnabled(true);
    this->setMQTTSubscribe(false);
    setSamplingRate(beatIntervalMillis);
    _interfaceName = HEARTBEAT_TOPIC;
}

inline void HeartbeatInterface::init()
{
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
    strcpy(status.networkAddress, _transport->getNetworkAddressString().c_str());
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
    root[networkAddressKey] = status.networkAddress;
    return root;
}

inline int HeartbeatInterface::cmp(NodeStatus oldValue, NodeStatus newValue)
{
    return -1;
}

inline void HeartbeatInterface::updatePhisicalInterface(NodeStatus newValue)
{
}

inline void HeartbeatInterface::valueToString(String &sValue)
{
    sValue = String("<3");
}

#endif //HEARTBEATINTERFACE_H