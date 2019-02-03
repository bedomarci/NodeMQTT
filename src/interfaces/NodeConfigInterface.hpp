#ifndef NODECONFIGINTERFACE_H
#define NODECONFIGINTERFACE_H

#include "_NodeInterface.hpp"
#include "NodeMQTTConfigManager.hpp"
#include <ESP.h>
#include "NodeMQTT.hpp"
#include "constants.hpp"

class NodeConfigInterface : public NodeInterface<NodeMQTTConfig>
{
  public:
    NodeConfigInterface();
    void init();
    void publishCurrentConfig(NodeMQTTConfig config);
    void valueToString(String &sValue);

  protected:
    void parseBssid(int *bssidBytes, char *bssidHex);
    virtual NodeMQTTConfig sample() override;
    virtual NodeMQTTConfig fromJSON(JsonObject &rootObject) override;
    virtual JsonObject &toJSON(NodeMQTTConfig value, JsonObject &root) override;
    virtual int cmp(NodeMQTTConfig oldValue, NodeMQTTConfig newValue) override;
    virtual void updatePhisicalInterface(NodeMQTTConfig newValue) override;
    // virtual void valueToString(NodeMQTTConfig value, String &sValue) override;
};

inline NodeConfigInterface::NodeConfigInterface()
    : NodeInterface<NodeMQTTConfig>(CONFIG_TOPIC_ECHO, CONFIG_TOPIC)
{
    _interfaceName = CONFIG_TOPIC;
    setSamplingEnabled(false);
    setMQTTSubscribe(true);
    setMQTTPublish(true);
}

inline NodeMQTTConfig NodeConfigInterface::sample()
{
    return currentValue;
}

inline NodeMQTTConfig NodeConfigInterface::fromJSON(JsonObject &rootObject)
{
    NodeMQTTConfig nodeConfig = NodeMQTTConfig();
    NodeMQTTConfigManager.loadInto(nodeConfig);
    if (rootObject.containsKey(ATTR_BASETOPIC))
        strcpy(nodeConfig.baseTopic, rootObject[ATTR_BASETOPIC].as<const char *>());
    if (rootObject.containsKey(ATTR_MQTTPASS))
        strcpy(nodeConfig.mqttPassword, rootObject[ATTR_MQTTPASS].as<const char *>());
    if (rootObject.containsKey(ATTR_MQTTSERV))
        strcpy(nodeConfig.mqttServer, rootObject[ATTR_MQTTSERV].as<const char *>());
    if (rootObject.containsKey(ATTR_MQTTUSER))
        strcpy(nodeConfig.mqttUser, rootObject[ATTR_MQTTUSER].as<const char *>());
    if (rootObject.containsKey(ATTR_WIFIPASS))
        strcpy(nodeConfig.wifiPassword, rootObject[ATTR_WIFIPASS].as<const char *>());
    if (rootObject.containsKey(ATTR_WIFISSID))
        strcpy(nodeConfig.wifiSsid, rootObject[ATTR_WIFISSID].as<const char *>());
    if (rootObject.containsKey(ATTR_WIFIBSSID))
    {
        char bssidHex[12];
        strcpy(bssidHex, rootObject[ATTR_WIFIBSSID].as<const char *>());
        char buff[2];
        for (int i = 0; i < 6; i++)
        {
            memcpy(buff, &bssidHex[i * 2], 2);
            nodeConfig.wifiBssid[i] = (int)strtol(&(buff[0]), NULL, 16);
        }
    }
    if (rootObject.containsKey(ATTR_WIFICHANNEL))
        nodeConfig.wifiChannel = rootObject[ATTR_WIFICHANNEL].as<unsigned char>();
    if (rootObject.containsKey(ATTR_CONFVER))
        nodeConfig.configVersion = rootObject[ATTR_CONFVER].as<unsigned char>();
    if (rootObject.containsKey(ATTR_ONLINE))
        nodeConfig.isOnline = rootObject[ATTR_ONLINE].as<bool>();
    if (rootObject.containsKey(ATTR_SERVICEMODE))
        nodeConfig.isServiceMode = rootObject[ATTR_SERVICEMODE].as<bool>();
    if (rootObject.containsKey(ATTR_MQTTPORT))
        nodeConfig.mqttPort = rootObject[ATTR_MQTTPORT].as<unsigned short>();
    return nodeConfig;
}

inline JsonObject &NodeConfigInterface::toJSON(NodeMQTTConfig status, JsonObject &root)
{
    root[ATTR_BASETOPIC] = status.baseTopic;
    // root[ATTR_CONFVER] = status.configVersion;
    // root[ATTR_ONLINE] = status.isOnline;
    // root[ATTR_SERVICEMODE] = status.isServiceMode;
    // root[ATTR_MQTTPASS] = status.mqttPassword;
    // root[ATTR_MQTTPORT] = status.mqttPort;
    // root[ATTR_MQTTSERV] = status.mqttServer;
    root[ATTR_MQTTUSER] = status.mqttUser;
    root[ATTR_WIFIPASS] = status.wifiPassword;
    root[ATTR_WIFISSID] = status.wifiSsid;
    return root;
}

inline int NodeConfigInterface::cmp(NodeMQTTConfig oldValue, NodeMQTTConfig newValue)
{
    return -1;
}

inline void NodeConfigInterface::updatePhisicalInterface(NodeMQTTConfig newValue)
{
    NodeMQTTConfigManager.save(newValue);
}

inline void NodeConfigInterface::init()
{
}

inline void NodeConfigInterface::publishCurrentConfig(NodeMQTTConfig config)
{
    publish(config);
}

inline void NodeConfigInterface::parseBssid(int *bssidBytes, char *bssidHex)
{
}

inline void NodeConfigInterface::valueToString(String &sValue)
{
    sValue = "";
}

#endif //NODECONFIGINTERFACE_H