#ifndef TYPEDEF_H
#define TYPEDEF_H

#include "config.hpp"
#include "../constants.hpp"
#include <LinkedList.h>

// class AbstractTransport;
// class AbstractParser;
class Scheduler;
struct ApplicationContext;

struct NodeMQTTConfig
{
  uint8_t configVersion = DEFAULT_CONFIGURATION_VERSION;
  char wifiSsid[32] = DEFAULT_WIFI_SSID;
  uint8_t wifiBssid[6] = DEFAULT_WIFI_BSSID;
  uint8_t ipAddress[4] = DEFAULT_IP_ADDRESS;
  uint8_t gateway[4] = DEFAULT_GATEWAY;
  uint8_t subnetMask[4] = DEFAULT_SUBNET;
  uint8_t dns[4] = DEFAULT_GATEWAY;
  uint8_t wifiChannel = DEFAULT_WIFI_CHANNEL;
  char wifiPassword[32] = DEFAULT_WIFI_PASSWORD;
  char mqttServer[64] = DEFAULT_MQTT_SERVER;
  char mqttUser[32] = DEFAULT_MQTT_USER;
  char mqttPassword[32] = DEFAULT_MQTT_PASSWORD;
  char baseTopic[32] = DEFAULT_MQTT_BASE_TOPIC;
  bool isOnline = DEFAULT_ISONLINE;
  bool isServiceMode = DEFAULT_ISSERVICEMODE;
  bool isLogging = DEFAULT_ISLOGGING;
  uint16_t mqttPort = DEFAULT_MQTT_PORT;
};

struct NodeMQTTProperty
{
  uint8_t id = 0;
  const char *name;
  uint32_t value;
  bool isStored;
  NodeMQTTProperty(uint8_t propertyId, const char *propertyName, uint32_t propertyValue, bool isStored)
      : id(propertyId),
        name(propertyName),
        value(propertyValue),
        isStored(isStored){};
  NodeMQTTProperty()
      : id(0),
        name(0),
        value(0),
        isStored(0){};
};

class NodeInterfaceBase
{
  public:
    virtual void setContext(ApplicationContext *context) = 0;

    virtual void writeRaw(String value, bool publishable = true) = 0;
    virtual void handle() = 0;
    virtual void setSamplingRate(unsigned long samplingRate) = 0;
    virtual void init() = 0;
    virtual void setEnabled(bool enabled) = 0;
    virtual bool isEnabled() = 0;
    virtual String valueToString() = 0;

    virtual bool hasMQTTPublish() = 0;
    virtual bool hasMQTTSubscribe() = 0;
    virtual bool hasInitializedValue() = 0;
    virtual void setMQTTPublish(bool publish) = 0;
    virtual void setMQTTSubscribe(bool subscribe) = 0;
    virtual void setBaseTopic(String baseTopic) = 0;
    virtual String getBaseTopic() = 0;
    virtual String getSubscribeTopic() = 0;
    virtual String getPublishTopic() = 0;

    virtual void republish() = 0;
};

#include "../transports/_AbstractTransport.hpp"
#include "../parsers/_AbstractParser.hpp"

struct ApplicationContext
{
    Scheduler *scheduler;
    NodeMQTTConfig *configuration;
    LinkedList<NodeInterfaceBase *> *interfaces;
    time_t currentTime = 0;

    AbstractTransport *transport;
    AbstractParser *parser;
};


#endif //TYPEDEF_H