#ifndef NODECONFIGINTERFACE_H
#define NODECONFIGINTERFACE_H

#include "_NodeInterface.hpp"
#include "NodeMQTTConfigManager.hpp"
#include <ESP.h>
#include "NodeMQTT.hpp"
#include "constants.hpp"
#include <string.h>

class NodeConfigInterface : public NodeInterface<NodeMQTTConfig>
{
  public:
    NodeConfigInterface();
    void init();
    void publishCurrentConfig(NodeMQTTConfig config);
    void valueToString(String &sValue);

  protected:
    virtual NodeMQTTConfig sample() override;
    virtual NodeMQTTConfig fromJSON(JsonObject &rootObject) override;
    virtual JsonObject &toJSON(NodeMQTTConfig value, JsonObject &root) override;
    virtual int cmp(NodeMQTTConfig oldValue, NodeMQTTConfig newValue) override;
    virtual void updatePhisicalInterface(NodeMQTTConfig newValue) override;
    void parseIpAddress(char *strIp, uint8_t ip[4]);
    void parseMacAddress(char *strMac, uint8_t mac[6]);
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
        char ipaddress[16];
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
        char bssidHex[18];
        strcpy(bssidHex, rootObject[ATTR_WIFIBSSID].as<const char *>());
        uint8_t mac[6];
        this->parseMacAddress(bssidHex, mac);
        memcpy(nodeConfig.wifiBssid, mac, sizeof(mac));

    }
    if (rootObject.containsKey(ATTR_IPADDRESS))
    {
        strcpy(ipaddress, rootObject[ATTR_IPADDRESS].as<const char *>());
        this->parseIpAddress(ipaddress, nodeConfig.ipAddress);
    }
    if (rootObject.containsKey(ATTR_GATEWAY))
    {
        strcpy(ipaddress, rootObject[ATTR_GATEWAY].as<const char *>());
        this->parseIpAddress(ipaddress, nodeConfig.gateway);
    }
    if (rootObject.containsKey(ATTR_SUBNET))
    {
        strcpy(ipaddress, rootObject[ATTR_SUBNET].as<const char *>());
        this->parseIpAddress(ipaddress, nodeConfig.subnetMask);
    }
    if (rootObject.containsKey(ATTR_DNS))
    {
        strcpy(ipaddress, rootObject[ATTR_DNS].as<const char *>());
        this->parseIpAddress(ipaddress, nodeConfig.dns);
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
    if (hasBSSID(status.wifiBssid))
    {
        String bssid;
        for (int i = 0; i < 6; i++)
            bssid += String(status.wifiBssid[i], HEX);
        root[ATTR_WIFIBSSID] = bssid;
    }
    else
    {
        root[ATTR_WIFISSID] = status.wifiSsid;
    }
    root[ATTR_MQTTUSER] = status.mqttUser;
    root[ATTR_WIFIPASS] = status.wifiPassword;
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

inline void NodeConfigInterface::valueToString(String &sValue)
{
    sValue = "";
}

inline void NodeConfigInterface::parseIpAddress(char *strIp, uint8_t ip[4])
{
    char *token;
    int i = 0;
    char *strSplit = strIp;
    while ((token = strtok_r(strSplit, ".", &strSplit)))
    {
        uint8_t octet = atoi(token);
        ip[i++] = octet;
    }
}

inline void NodeConfigInterface::parseMacAddress(char *strMac, uint8_t mac[6])
{
    char *token;
    int i = 0;
    char *strSplit = strMac;
    while ((token = strtok_r(strSplit, ":", &strSplit)))
    {
        uint8_t octet = (int)strtol(&(token[0]), NULL, 16);
        mac[i++] = octet;
    }
}

#endif //NODECONFIGINTERFACE_H