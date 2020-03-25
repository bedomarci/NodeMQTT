#ifndef HEARTBEATINTERFACE_H
#define HEARTBEATINTERFACE_H

#include "misc/helpers.hpp"
#include "_NodeInterface.hpp"
#include "../transports/WifiTransport.hpp"

struct NodeStatus
{
    uint32_t uptime = 0;
    uint32_t freeHeap = 0;
    char nodeID[9] = "";
    char networkAddress[16];

};

class HeartbeatInterface : public NodeInterface<NodeStatus>
{
  public:
    HeartbeatInterface(int beatIntervalSecond = DEFAULT_HEARTBEAT_RATE);
    void init();
    String valueToString() override;

  protected:
    NodeStatus status;
    virtual NodeStatus sample() override;
    virtual void updatePhisicalInterface(NodeStatus newValue) override;
    NodeStatus fromJSON(JsonObject &root) override;
    JsonObject &toJSON(NodeStatus value, JsonObject &root) override;
    int cmp(NodeStatus oldValue, NodeStatus newValue) override;

    const char *uptimeKey = "uptime";
    const char *nodeIdKey = "nodeid";
    const char *freeHeapKey = "freehp";
    const char *networkAddressKey = "netadd";
};

inline HeartbeatInterface::HeartbeatInterface(int beatIntervalMillis)
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
    char sUUID[9];
    formatUUID(sUUID);
    strncpy(status.nodeID, sUUID, sizeof(status.nodeID));
}

inline NodeStatus HeartbeatInterface::sample()
{
    status.uptime = millis();
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
        status.freeHeap = ESP.getFreeHeap();
    #endif
    strcpy(status.networkAddress, this->getContext()->transport->getNetworkAddressString().c_str());
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
    root["RSSI"] = WiFi.RSSI();
    return root;
}

inline int HeartbeatInterface::cmp(NodeStatus oldValue, NodeStatus newValue)
{
    return -1;
}

inline void HeartbeatInterface::updatePhisicalInterface(NodeStatus newValue)
{
}

inline String HeartbeatInterface::valueToString()
{
    return String(F("<3"));
}

#endif //HEARTBEATINTERFACE_H