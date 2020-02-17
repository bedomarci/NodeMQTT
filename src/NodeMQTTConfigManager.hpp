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

    int compareProperty(NodeMQTTProperty &a, NodeMQTTProperty &b);

    void getEEPROMLocationById(uint16_t propertyId, uint16_t &propertyAddress , uint8_t &propertyLength);

    const int EEPROM_CONFIGURATION_CHCKSUM_ADDRESS = 32;
    int EEPROM_CONFIGURATION_ADDRESS;
    int EEPROM_PROPERTIES_ADDRESS;
    bool isPropertyDirty = false;
    NodeMQTTConfig *defaultConfig;
    LinkedList<NodeMQTTProperty> *properties;

public:
    NodeMQTTConfigManagerClass();

    void save(NodeMQTTConfig *configuration);

    void loadInto(NodeMQTTConfig *configuration);

    void loadDefaultsInto(NodeMQTTConfig *configuration);

    void print(NodeMQTTConfig *configuration);

    void registerProperty(uint16_t propertyId, const char *propertyName,
                          uint8_t *propertyDefaultValue, uint8_t length);

    uint8_t *getProperty(uint16_t propertyId);
    int getIntProperty(uint16_t propertyId);
    String getStringProperty(uint16_t propertyId);
    uint8_t getBoolProperty(uint16_t propertyId);

    LinkedList<NodeMQTTProperty> *getProperties();

    void setProperty(uint16_t propertyId, uint8_t *propertyValue, uint8_t length);
    void setIntProperty(uint16_t propertyId, int propertyValue);
    void setStringProperty(uint16_t propertyId, char * propertyValue);
    void setBoolProperty(uint16_t propertyId, uint8_t propertyValue);

    void storePropertiesInEEPROM();

    void loadPropertiesFromEEPROM();

    void factoryReset();


};

extern NodeMQTTConfigManagerClass NodeMQTTConfigManager;
#endif //NODECONFIGURATION_H