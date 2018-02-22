#include "NodeConfigInterface.hpp"
#include <ESP.h>
#include "NodeMQTT.hpp"
#include "misc/constants.hpp"

NodeConfigInterface::NodeConfigInterface(): NodeInterface<NodeMQTTConfig>(CONFIG_TOPIC, CONFIG_TOPIC) {
    samplingEnabled = false;
    hasMQTTSubscribe = true;
    interfaceName = CONFIG_TOPIC;
}

NodeMQTTConfig NodeConfigInterface::sample() {
    return currentValue;
}

NodeMQTTConfig NodeConfigInterface::fromJSON (JsonObject& rootObject){
    NodeMQTTConfig nodeConfig = NodeMQTTConfig();
    NodeMQTTConfigManager.loadInto(nodeConfig);
    if (rootObject.containsKey(ATTR_BASETOPIC))
        strcpy(nodeConfig.baseTopic, rootObject[ATTR_BASETOPIC].as<const char*>());
    if (rootObject.containsKey(ATTR_MQTTPASS)) 
        strcpy(nodeConfig.mqttPassword, rootObject[ATTR_MQTTPASS].as<const char*>());
    if (rootObject.containsKey(ATTR_MQTTSERV)) 
        strcpy(nodeConfig.mqttServer, rootObject[ATTR_MQTTSERV].as<const char*>());
    if (rootObject.containsKey(ATTR_MQTTUSER)) 
        strcpy(nodeConfig.mqttUser, rootObject[ATTR_MQTTUSER].as<const char*>());
    if (rootObject.containsKey(ATTR_WIFIPASS))
        strcpy(nodeConfig.wifiPassword, rootObject[ATTR_WIFIPASS].as<const char*>());
    if (rootObject.containsKey(ATTR_WIFISSID))
        strcpy(nodeConfig.wifiSsid, rootObject[ATTR_WIFISSID].as<const char*>());
    if (rootObject.containsKey(ATTR_CONFVER))
        nodeConfig.configVersion= rootObject[ATTR_CONFVER].as<unsigned char>();
    if (rootObject.containsKey(ATTR_ONLINE))
        nodeConfig.isOnline= rootObject[ATTR_ONLINE].as<bool>();
    if (rootObject.containsKey(ATTR_SERVICEMODE))
        nodeConfig.isServiceMode= rootObject[ATTR_SERVICEMODE].as<bool>();
    if (rootObject.containsKey(ATTR_MQTTPORT))
        nodeConfig.mqttPort= rootObject[ATTR_MQTTPORT].as<unsigned short>();
    return nodeConfig;
}

JsonObject& NodeConfigInterface::toJSON (NodeMQTTConfig status){
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    return root;   
}

int NodeConfigInterface::cmp(NodeMQTTConfig oldValue, NodeMQTTConfig newValue){
    return -1;
}

void NodeConfigInterface::updatePhisicalInterface(NodeMQTTConfig newValue)
{
    NodeMQTTConfigManager.save(newValue);
}