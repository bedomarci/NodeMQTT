#ifndef NODECONFIGURATION_H
#define NODECONFIGURATION_H

#include "misc/config.hpp"
#include "misc/constants.hpp"
#include "misc/helpers.hpp"
#include <Arduino.h>
#include <EEPROM.h>

struct NodeMQTTConfig
{
  uint8_t configVersion = DEFAULT_CONFIGURATION_VERSION;
  char wifiSsid[32] = DEFAULT_WIFI_SSID;
  uint8_t wifiBssid[6] = DEFAULT_WIFI_BSSID;
  uint8_t wifiChannel = DEFAULT_WIFI_CHANNEL;
  char wifiPassword[32] = DEFAULT_WIFI_PASSWORD;
  char mqttServer[64] = DEFAULT_MQTT_SERVER;
  char mqttUser[32] = DEFAULT_MQTT_USER;
  char mqttPassword[32] = DEFAULT_MQTT_PASSWORD;
  char baseTopic[32] = DEFAULT_MQTT_BASE_TOPIC;
  bool isOnline = DEFAULT_ISONLINE;
  bool isServiceMode = DEFAULT_ISSERVICEMODE;
  uint16_t mqttPort = DEFAULT_MQTT_PORT;
};

class NodeMQTTConfigManagerClass
{
private:
  uint32_t calculateChkSum(NodeMQTTConfig &config);
  void EEPROMWriteChkSum(uint32_t chksum);
  uint32_t EEPROMReadChkSum();
  bool isValid();

  const int EEPROM_CONFIGURATION_CHCKSUM_ADDRESS = 32;
  const int EEPROM_CONFIGURATION_ADDRESS = EEPROM_CONFIGURATION_CHCKSUM_ADDRESS + sizeof(uint32_t);
  NodeMQTTConfig defaultConfig;

public:
  NodeMQTTConfigManagerClass();
  // void begin();
  void save(NodeMQTTConfig &configuration);
  void setDefault(NodeMQTTConfig &configuration);
  void loadInto(NodeMQTTConfig &configuration);
  void loadDefaultsInto(NodeMQTTConfig &configuration);
  void print(NodeMQTTConfig &configuration);
};

extern NodeMQTTConfigManagerClass NodeMQTTConfigManager;

#endif //NODECONFIGURATION_H