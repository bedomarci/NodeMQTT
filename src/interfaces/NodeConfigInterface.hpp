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

    void publishCurrentConfig();

    String valueToString();

protected:
    virtual bool sample() override;

    virtual bool fromJSON(JsonObject &rootObject) override;

    virtual JsonObject &toJSON(bool value, JsonObject &root) override;

    virtual int cmp(bool oldValue, bool newValue) override;

    virtual void updatePhisicalInterface(bool newValue) override;
};

inline NodeConfigInterface::NodeConfigInterface()
        : NodeInterface<bool>(CONFIG_PUB_TOPIC, CONFIG_SUB_TOPIC) {
    setInterfaceName(CONFIG_NAME);
    setSamplingEnabled(false);
    setMQTTSubscribe(true);
    setMQTTPublish(true);
}

inline bool NodeConfigInterface::sample() {
    return currentValue;
}

inline bool NodeConfigInterface::fromJSON(JsonObject &rootObject) {

    LinkedList<NodeMQTTProperty> *properties = NodeMQTTConfigManager.getProperties();
    for (int i = 0; i < properties->size(); i++) {
        NodeMQTTProperty property = properties->get(i);
        auto name = FPSTR(property.name);
        if (rootObject.containsKey(name)) {
            switch (property.type) {
                case INT_PROPERTY:
                    NodeMQTTConfigManager.setIntProperty(property.id, rootObject[name].as<int>());
                    break;
                case STRING_PROPERTY:
                    NodeMQTTConfigManager.setStringProperty(property.id, rootObject[name].as<const char *>());
                    break;
                case BOOL_PROPERTY:
                    NodeMQTTConfigManager.setBoolProperty(property.id, rootObject[name].as<bool>());
                    break;
                case MAC_PROPERTY:
                    char sMac[18];
                    strcpy(sMac, rootObject[name].as<const char *>());
                    uint8_t mac[6];
                    parseMacAddress(sMac, mac);
                    NodeMQTTConfigManager.setMACProperty(property.id, mac);
                    break;
                case IP_PROPERTY:
                    char sIp[16];
                    strcpy(sIp, rootObject[name].as<const char *>());
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
//                root[FPSTR(property.name)] = NodeMQTTConfigManager.getProperty(property.id);
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

inline void NodeConfigInterface::publishCurrentConfig() {
    publish(true);
}

inline String NodeConfigInterface::valueToString() {
    return "";
}

#endif //NODECONFIGINTERFACE_H