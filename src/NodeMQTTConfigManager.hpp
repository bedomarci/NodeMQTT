#ifndef NODECONFIGURATION_H
#define NODECONFIGURATION_H

#include "misc/config.hpp"
#include "misc/typedef.hpp"
#include "constants.hpp"
#include "misc/helpers.hpp"
#include "LinkedList.h"
#include <Arduino.h>
#include <EEPROM.h>



class NodeMQTTConfigManagerClass
{
private:
  uint32_t calculateChkSum(NodeMQTTConfig &config);
  void EEPROMWriteChkSum(uint32_t chksum);
  void setStaticFlagDefaultValues();
  uint32_t EEPROMReadChkSum();
  void printIp(uint8_t ip[4]);
  bool isValid();

  const int EEPROM_CONFIGURATION_CHCKSUM_ADDRESS = 32;
  const int EEPROM_CONFIGURATION_ADDRESS = EEPROM_CONFIGURATION_CHCKSUM_ADDRESS + sizeof(uint32_t);
  const int EEPROM_PROPERTIES_ADDRESS = EEPROM_CONFIGURATION_ADDRESS + sizeof(NodeMQTTConfig);
  NodeMQTTConfig defaultConfig;
  LinkedList<NodeMQTTProperty *> properties;

public:
  NodeMQTTConfigManagerClass();
  void save(NodeMQTTConfig &configuration);
  void setDefault(NodeMQTTConfig &configuration);
  void loadInto(NodeMQTTConfig &configuration);
  void loadDefaultsInto(NodeMQTTConfig &configuration);
  void print(NodeMQTTConfig &configuration);
  void addProperty(uint8_t propertyId, const char *propertyName, uint32_t propertyDefaultValue);
  uint32_t getProperty(uint8_t propertyId);
  void setProperty(uint8_t propertyId, uint32_t propertyValue);
  void storePropertiesInEEPROM();
  void loadPropertiesFromEEPROM();
  void factoryReset();
  // static volatile bool I2CInitialized;
  // static volatile bool EEPROMInitialized;
};

extern NodeMQTTConfigManagerClass NodeMQTTConfigManager;
#endif //NODECONFIGURATION_H