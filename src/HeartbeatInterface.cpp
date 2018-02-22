#include "HeartbeatInterface.hpp"
#include <ESP.h>
#include "NodeMQTT.hpp"
#include "misc/constants.hpp"

HeartbeatInterface::HeartbeatInterface(int beatIntervalMillis): NodeInterface<NodeStatus>(HEARTBEAT_TOPIC, HEARTBEAT_TOPIC) {
    samplingEnabled = true;
    hasMQTTSubscribe = false;
    _enabled=false;
    setSampleRate(beatIntervalMillis);
    interfaceName = HEARTBEAT_TOPIC;
}

NodeStatus HeartbeatInterface::sample() {
    NodeStatus status;
    status.uptime = millis();
    status.nodeID = NodeMQTT::getMQTTDeviceName();
    status.freeHeap = ESP.getFreeHeap();
    return status;
}

NodeStatus HeartbeatInterface::fromJSON (JsonObject& rootObject){
    return NodeStatus();
}

JsonObject& HeartbeatInterface::toJSON (NodeStatus status){
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root[F("uptime")] = status.uptime;
    root[F("nodeID")] = status.nodeID;
    root[F("freeHeap")] = status.freeHeap;
    return root;
}

int HeartbeatInterface::cmp(NodeStatus oldValue, NodeStatus newValue){
    return -1;
}

void HeartbeatInterface::updatePhisicalInterface(NodeStatus newValue)
{

}