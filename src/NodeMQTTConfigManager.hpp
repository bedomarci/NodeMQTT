#ifndef NODECONFIGURATION_H
#define NODECONFIGURATION_H

#include "misc/config.hpp"
#include "misc/typedefDeclaration.hpp"
#include "constants.hpp"
#include "misc/helpers.hpp"
#include "LinkedList.h"
#include <Arduino.h>
#include <EEPROM.h>



class NodeMQTTConfigManagerClass
{
private:
  uint32_t calculateChkSum(NodeMQTTConfig *configuration);
  void EEPROMWriteChkSum(uint32_t chksum);
  void setStaticFlagDefaultValues();
  uint32_t EEPROMReadChkSum();
  void printIp(uint8_t ip[4]);
  void printAttributeWithTab(String s);
  bool isValid();
  void commit();

  const int EEPROM_CONFIGURATION_CHCKSUM_ADDRESS = 32;
  int EEPROM_CONFIGURATION_ADDRESS;
  int EEPROM_PROPERTIES_ADDRESS;
  bool isPropertyDirty = false;
  NodeMQTTConfig * defaultConfig;
  LinkedList<NodeMQTTProperty> * properties;

public:
  NodeMQTTConfigManagerClass();
  void save(NodeMQTTConfig *configuration);
  void loadInto(NodeMQTTConfig *configuration);
  void loadDefaultsInto(NodeMQTTConfig *configuration);
  void print(NodeMQTTConfig *configuration);
  void registerProperty(uint8_t propertyId, const char *propertyName, int32 propertyDefaultValue);
    int32 getProperty(uint8_t propertyId);
  LinkedList<NodeMQTTProperty> * getProperties();
  void setProperty(uint8_t propertyId, int32 propertyValue);
  void storePropertiesInEEPROM();
  void loadPropertiesFromEEPROM();
  void factoryReset();
};

extern NodeMQTTConfigManagerClass NodeMQTTConfigManager;
#endif //NODECONFIGURATION_H