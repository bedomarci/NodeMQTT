#ifndef NODECONFIGINTERFACE_H
#define NODECONFIGINTERFACE_H

#include "_NodeInterface.hpp"
#include "NodeMQTTConfigManager.hpp"
#include "NodeMQTT.hpp"
#include "constants.hpp"
#include <string.h>

class NodeConfigInterface : public NodeInterface<bool> {
public:
    NodeConfigInterface();

    void init();

    void publishCurrentConfig(bool config);

    String valueToString();

protected:
    virtual bool sample();

    virtual bool fromJSON(JsonObject &rootObject);

    virtual JsonObject &toJSON(bool value, JsonObject &root);

    virtual int cmp(bool oldValue, bool newValue);

    virtual void updatePhisicalInterface(bool newValue);
};

inline NodeConfigInterface::NodeConfigInterface()
        : NodeInterface<bool>(CONFIG_TOPIC_ECHO, CONFIG_TOPIC) {
    _interfaceName = CONFIG_TOPIC;
    setSamplingEnabled(false);
    setMQTTSubscribe(true);
    setMQTTPublish(true);
}

inline bool NodeConfigInterface::sample() {
    return currentValue;
}

inline bool NodeConfigInterface::fromJSON(JsonObject &rootObject) {
//    NodeMQTTConfig nodeConfig = NodeMQTTConfig();
//    NodeMQTTConfigManager.loadInto(&nodeConfig);
//    char ipaddress[16];
//    if (rootObject.containsKey(ATTR_BASETOPIC))
//        strcpy(nodeConfig.baseTopic, rootObject[ATTR_BASETOPIC].as<const char *>());
//    if (rootObject.containsKey(ATTR_MQTTPASS))
//        strcpy(nodeConfig.mqttPassword, rootObject[ATTR_MQTTPASS].as<const char *>());
//    if (rootObject.containsKey(ATTR_MQTTSERV))
//        strcpy(nodeConfig.mqttServer, rootObject[ATTR_MQTTSERV].as<const char *>());
//    if (rootObject.containsKey(ATTR_MQTTUSER))
//        strcpy(nodeConfig.mqttUser, rootObject[ATTR_MQTTUSER].as<const char *>());
//    if (rootObject.containsKey(ATTR_WIFIPASS))
//        strcpy(nodeConfig.wifiPassword, rootObject[ATTR_WIFIPASS].as<const char *>());
//    if (rootObject.containsKey(ATTR_WIFISSID))
//        strcpy(nodeConfig.wifiSsid, rootObject[ATTR_WIFISSID].as<const char *>());
//    if (rootObject.containsKey(ATTR_WIFIBSSID)) {
//        char bssidHex[18];
//        strcpy(bssidHex, rootObject[ATTR_WIFIBSSID].as<const char *>());
//        uint8_t mac[6];
//        parseMacAddress(bssidHex, mac);
//        memcpy(nodeConfig.wifiBssid, mac, sizeof(mac));
//
//    }
//    if (rootObject.containsKey(ATTR_IPADDRESS)) {
//        strcpy(ipaddress, rootObject[ATTR_IPADDRESS].as<const char *>());
//        parseIpAddress(ipaddress, nodeConfig.ipAddress);
//    }
//    if (rootObject.containsKey(ATTR_GATEWAY)) {
//        strcpy(ipaddress, rootObject[ATTR_GATEWAY].as<const char *>());
//        parseIpAddress(ipaddress, nodeConfig.gateway);
//    }
//    if (rootObject.containsKey(ATTR_SUBNET)) {
//        strcpy(ipaddress, rootObject[ATTR_SUBNET].as<const char *>());
//        parseIpAddress(ipaddress, nodeConfig.subnetMask);
//    }
//    if (rootObject.containsKey(ATTR_DNS)) {
//        strcpy(ipaddress, rootObject[ATTR_DNS].as<const char *>());
//        parseIpAddress(ipaddress, nodeConfig.dns);
//    }
//    if (rootObject.containsKey(ATTR_WIFICHANNEL))
//        nodeConfig.wifiChannel   = rootObject[ATTR_WIFICHANNEL].as<unsigned char>();
//    if (rootObject.containsKey(ATTR_CONFVER))
//        nodeConfig.configVersion = rootObject[ATTR_CONFVER].as<unsigned char>();
//    if (rootObject.containsKey(ATTR_ONLINE))
//        nodeConfig.isOnline      = rootObject[ATTR_ONLINE].as<bool>();
//    if (rootObject.containsKey(ATTR_SERVICEMODE))
//        nodeConfig.isServiceMode = rootObject[ATTR_SERVICEMODE].as<bool>();
//    if (rootObject.containsKey(ATTR_MQTTPORT))
//        nodeConfig.mqttPort      = rootObject[ATTR_MQTTPORT].as<unsigned short>();

    LinkedList<NodeMQTTProperty> *properties = NodeMQTTConfigManager.getProperties();
    Serial.print(21);
    for (int i = 0; i < properties->size(); i++) {
        Serial.print(22);
        NodeMQTTProperty property = properties->get(i);
        Serial.print(23);
        Serial.println(FPSTR(property.name));
        //Wrap key in a string to avoid exception because of keys stored in PROGMEM
        if (rootObject.containsKey(FPSTR(property.name))) {
            switch (property.type) {
                case INT_PROPERTY:
                    NodeMQTTConfigManager.setIntProperty(property.id, rootObject[FPSTR(property.name)].as<int>());
                    break;
                case STRING_PROPERTY:
                    NodeMQTTConfigManager.setStringProperty(property.id, rootObject[FPSTR(property.name)].as<const char *>());
                    break;
                case BOOL_PROPERTY:
                    Serial.println("serboolproperty");
                    NodeMQTTConfigManager.setBoolProperty(property.id, rootObject[FPSTR(property.name)].as<bool>());
                    break;
                case MAC_PROPERTY:
                    char sMac[18];
                    strcpy(sMac, rootObject[FPSTR(property.name)].as<const char *>());
                    uint8_t mac[6];
                    parseMacAddress(sMac, mac);
                    NodeMQTTConfigManager.setMACProperty(property.id, mac);
                    break;
                case IP_PROPERTY:
                    char sIp[16];
                    strcpy(sIp, rootObject[FPSTR(property.name)].as<const char *>());
                    uint8_t ip[4];
                    parseIpAddress(sIp, ip);
                    NodeMQTTConfigManager.setIPProperty(property.id, ip);
                    break;
                case BYTE_PROPERTY:
                default:
                    break;
            }
        }
    }
    return true;
}

inline JsonObject &NodeConfigInterface::toJSON(bool status, JsonObject &root) {
    root[ATTR_BASETOPIC] = NodeMQTTConfigManager.getStringProperty(PROP_SYS_BASETOPIC);
    // root[ATTR_CONFVER] = status.configVersion;
    // root[ATTR_ONLINE] = status.isOnline;
    // root[ATTR_SERVICEMODE] = status.isServiceMode;
    // root[ATTR_MQTTPASS] = status.mqttPassword;
    // root[ATTR_MQTTPORT] = status.mqttPort;
    // root[ATTR_MQTTSERV] = status.mqttServer;
//    uint8_t
//    if (hasBSSID(status.wifiBssid)) {
//        String   bssid;
//        for (int i = 0; i < 6; i++)
//            bssid += String(status.wifiBssid[i], HEX);
//        root[ATTR_WIFIBSSID] = bssid;
//    } else {
//        root[ATTR_WIFISSID] = NodeMQTTConfigManager.getStringProperty(PROP_WIFI_SSID);
////    }
//    root[ATTR_MQTTUSER]  = NodeMQTTConfigManager.getStringProperty(PROP_MQTT_USER);;
//    root[ATTR_WIFIPASS]  = NodeMQTTConfigManager.getStringProperty(PROP_WIFI_PASSWORD);;
    LinkedList<NodeMQTTProperty> *properties = NodeMQTTConfigManager.getProperties();
    for (int i = 0; i < properties->size(); i++) {
        NodeMQTTProperty property = properties->get(i);
        switch (property.type) {
            case STRING_PROPERTY:
                root[FPSTR(property.name)] = NodeMQTTConfigManager.getStringProperty(property.id);
                break;
            case INT_PROPERTY:
                root[FPSTR(property.name)] = NodeMQTTConfigManager.getIntProperty(property.id);
                break;
            case BOOL_PROPERTY:
                root[FPSTR(property.name)] = NodeMQTTConfigManager.getBoolProperty(property.id);
                break;
            default:
                root[FPSTR(property.name)] = NodeMQTTConfigManager.getProperty(property.id);
                break;
        }
    }
    return root;
}

inline int NodeConfigInterface::cmp(bool oldValue, bool newValue) {
    return -1;
}

inline void NodeConfigInterface::updatePhisicalInterface(bool newValue) {
    NodeMQTTConfigManager.save();
}

inline void NodeConfigInterface::init() {
}

inline void NodeConfigInterface::publishCurrentConfig(bool config) {
    publish(config);
}

inline String NodeConfigInterface::valueToString() {
    return "";
}

//inline void NodeConfigInterface::parseIpAddress(char *strIp, uint8_t ip[4]) {
//    char *token;
//    int  i         = 0;
//    char *strSplit = strIp;
//    while ((token = strtok_r(strSplit, ".", &strSplit))) {
//        uint8_t octet = atoi(token);
//        ip[i++] = octet;
//    }
//}
//
//inline void NodeConfigInterface::parseMacAddress(char *strMac, uint8_t mac[6]) {
//    char *token;
//    int  i         = 0;
//    char *strSplit = strMac;
//    while ((token = strtok_r(strSplit, ":", &strSplit))) {
//        uint8_t octet = (int) strtol(&(token[0]), NULL, 16);
//        mac[i++] = octet;
//    }
//}

#endif //NODECONFIGINTERFACE_H