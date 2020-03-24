#ifndef NODECONFIGURATION_H
#define NODECONFIGURATION_H

#include "misc/config.hpp"
#include "misc/typedefDeclaration.hpp"
#include "constants.hpp"
#include "misc/helpers.hpp"
#include "LinkedList.h"
#include <Arduino.h>
#include <EEPROM.h>

enum NodeMQTTPropertyType {
    BYTE_PROPERTY,
    INT_PROPERTY,
    STRING_PROPERTY,
    BOOL_PROPERTY,
    IP_PROPERTY,
    MAC_PROPERTY,
};

struct NodeMQTTProperty {
    uint16_t id = 0;
    const char *name;
    uint8_t value[EEPROM_MAX_WORD_LENGTH]{};
    uint8_t length{};
    uint8_t isStored;
    NodeMQTTPropertyType type = BYTE_PROPERTY;

    NodeMQTTProperty(uint16_t propertyId, const char *const propertyName)
            : id(propertyId),
              name(propertyName),
              isStored(0) {};

    NodeMQTTProperty()
            : id(0),
              name(nullptr),
              isStored(0) {};
};

class NodeMQTTConfigManagerClass {
private:

    void EEPROMWriteChkSum(uint32_t chksum);

//    void setStaticFlagDefaultValues();

    uint32_t EEPROMReadChkSum();

    void printAttributeWithTab(String s);

    bool isValid();

    void commit();

    uint32_t calculateEEPROMPropertyChkSum();

    uint8_t isEEPROMValid();

    void getEEPROMLocationById(uint16_t propertyId, uint16_t &propertyAddress, uint8_t &propertyLength);

    NodeMQTTProperty getRAMPropertyById(uint16_t propertyId);

    const int EEPROM_PROPERTY_CHCKSUM_ADDRESS = 32;
    const int EEPROM_PROPERTIES_ADDRESS = EEPROM_PROPERTY_CHCKSUM_ADDRESS + sizeof(uint32_t);
    bool isPropertyDirty = false;
    bool isLoaded = false;
    LinkedList<NodeMQTTProperty> *properties;

    void storePropertiesInEEPROM();

    void loadPropertiesFromEEPROM();

    bool isIdRegistered(uint16_t id);

public:
    NodeMQTTConfigManagerClass();

    void save();

    void load();

    void print();

    void printArraySeparated(uint8_t *array, uint8_t length, char separator);

    void registerProperty(uint16_t propertyId, const char *propertyName, void *propertyDefaultValue, uint8_t length, NodeMQTTPropertyType type = BYTE_PROPERTY);

    void registerIntProperty(uint16_t propertyId, const char *propertyName, int propertyDefaultValue);

    void registerStringProperty(uint16_t propertyId, const char *propertyName, const char *propertyDefaultValue);

    void registerBoolProperty(uint16_t propertyId, const char *propertyName, bool propertyDefaultValue);

    void registerIPProperty(uint16_t propertyId, const char *propertyName, uint8_t propertyDefaultValue[4]);

    void registerMACProperty(uint16_t propertyId, const char *propertyName, uint8_t propertyDefaultValue[6]);

    void getProperty(uint16_t propertyId, void *propertyValue, uint8_t length);

    int getIntProperty(uint16_t propertyId);

    String getStringProperty(uint16_t propertyId);

    bool getBoolProperty(uint16_t propertyId);

    void getIPProperty(uint16_t propertyId, uint8_t ipArray[4]);

    void getMACProperty(uint16_t propertyId, uint8_t ipArray[6]);

    LinkedList<NodeMQTTProperty> *getProperties();

    void setProperty(uint16_t propertyId, const void *propertyValue, uint8_t length);

    void setIntProperty(uint16_t propertyId, int propertyValue);

    void setStringProperty(uint16_t propertyId, const char *propertyValue);

    void setBoolProperty(uint16_t propertyId, bool propertyValue);

    void setIPProperty(uint16_t propertyId, uint8_t propertyValue[4]);

    void setMACProperty(uint16_t propertyId, uint8_t propertyValue[6]);

    void factoryReset();


};

extern NodeMQTTConfigManagerClass NodeMQTTConfigManager;
#endif //NODECONFIGURATION_H