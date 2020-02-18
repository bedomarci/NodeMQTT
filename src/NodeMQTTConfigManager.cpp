#include "NodeMQTTConfigManager.hpp"
#include "NodeMQTTIOContainer.hpp"
#include "misc/typedef.hpp"

#define STORED 85

NodeMQTTConfigManagerClass::NodeMQTTConfigManagerClass() {
    EEPROM_PROPERTY_CHCKSUM_ADDRESS = EEPROM_CONFIGURATION_CHCKSUM_ADDRESS + sizeof(uint32_t);
    EEPROM_CONFIGURATION_ADDRESS = EEPROM_PROPERTY_CHCKSUM_ADDRESS + sizeof(uint32_t);
    EEPROM_PROPERTIES_ADDRESS = EEPROM_CONFIGURATION_ADDRESS + sizeof(NodeMQTTConfig);
    setStaticFlagDefaultValues();
    // if (!NodeMQTTConfigManagerClass::EEPROMInitialized)
    {
        EEPROM.begin(EEPROM_SIZE);
        // NodeMQTTConfigManagerClass::EEPROMInitialized = true;
    }
    // NodeMQTTIO.println(EEPROM.length());
    // if (!EEPROM.begin(EEPROM.length()))
    // {
    //     NodeMQTTIO.println("Failed to initialise EEPROM");
    //     NodeMQTTIO.println("Restarting...");
    //     delay(1000);
    //     ESP.restart();
    // }

    properties = new LinkedList<NodeMQTTProperty>();

}

void NodeMQTTConfigManagerClass::setStaticFlagDefaultValues() {
    // NodeMQTTConfigManagerClass::I2CInitialized = false;
    // NodeMQTTConfigManagerClass::EEPROMInitialized = false;
}

uint32_t NodeMQTTConfigManagerClass::calculateChkSum(NodeMQTTConfig *config) {
    int i;
    int len = sizeof(NodeMQTTConfig);
    uint32_t checksum;
    char *data = (char *) config;
    for (i = 0, checksum = 0; i < len; i++)
        checksum += data[i];
    return checksum;
}

uint8_t NodeMQTTConfigManagerClass::isEEPROMValid() {
    Serial.println("CHKSUM");
    uint32_t calculatedChkSum = this->calculateEEPROMPropertyChkSum();
    uint32_t storedChkSum;
    EEPROM.get(EEPROM_PROPERTY_CHCKSUM_ADDRESS, storedChkSum);
    Serial.println(calculatedChkSum);
    Serial.println(storedChkSum);
    return calculatedChkSum == storedChkSum;
}

uint32_t NodeMQTTConfigManagerClass::calculateRAMPropertyChkSum() {
    uint32_t sum = 0;
    for (int i = 0; i < this->properties->size(); i++) {
        NodeMQTTProperty property = this->properties->get(i);
        for (int j = 0; j < property.length; j++)
            sum += property.value[j];
    }
    return sum;
}

uint32_t NodeMQTTConfigManagerClass::calculateEEPROMPropertyChkSum() {
    uint32_t sum = 0;
    uint16_t address;
    uint8_t length;
    address = EEPROM_PROPERTIES_ADDRESS;
    for (int i = 0; i < this->properties->size(); i++) {
        address += sizeof(uint16_t); //skipping property id
        EEPROM.get(address, length);
        address += sizeof(uint8_t);
        for (int i = 0; i < length; i++)
            sum += EEPROM.read(address++);
    }
    return sum;
}

void NodeMQTTConfigManagerClass::save(NodeMQTTConfig *configuration) {
    i(F("Storing configuration in EEPROM."));
    configuration->configVersion = DEFAULT_CONFIGURATION_VERSION;
    uint32_t chksum = calculateChkSum(configuration);

    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(EEPROM_CONFIGURATION_CHCKSUM_ADDRESS, chksum);
    EEPROM.put(EEPROM_CONFIGURATION_ADDRESS, *configuration);
    if (isPropertyDirty) {
        storePropertiesInEEPROM();
    }
    commit();
    EEPROM.end();
    i(F("Node restarts. Good bye!"));
    restartNode();
}

void NodeMQTTConfigManagerClass::loadInto(NodeMQTTConfig *configuration) {
    i(F("Loading configuration from EEPROM."));
    uint32_t storedChkSum;
    EEPROM.get(EEPROM_CONFIGURATION_CHCKSUM_ADDRESS, storedChkSum);
    EEPROM.get(EEPROM_CONFIGURATION_ADDRESS, *configuration);
    uint32_t calculatedChkSum = calculateChkSum(configuration);
    if (calculatedChkSum != storedChkSum || configuration->configVersion != DEFAULT_CONFIGURATION_VERSION) {
        i(F("Configuration is invalid in EEPROM."));
        loadDefaultsInto(configuration);
    }
}

//
void NodeMQTTConfigManagerClass::loadDefaultsInto(NodeMQTTConfig *configuration) {
    i(F("Loading default configuration."));
    configuration = new NodeMQTTConfig();
    save(configuration);
}

void NodeMQTTConfigManagerClass::registerProperty(uint16_t propertyId, const char *propertyName,
                                                  uint8_t *propertyDefaultValue, uint8_t length,
                                                  NodeMQTTPropertyType type) {
    NodeMQTTProperty property = NodeMQTTProperty(propertyId, propertyName);
    property.length = length;
    property.type = type;
    memcpy(property.value, propertyDefaultValue, length);
    this->properties->add(property);
}

void NodeMQTTConfigManagerClass::registerIntProperty(uint16_t propertyId, const char *propertyName,
                                                     int propertyDefaultValue) {
    this->registerProperty(propertyId, propertyName, (uint8_t *) &propertyDefaultValue, sizeof(int), INT_PROPERTY);
}

void NodeMQTTConfigManagerClass::registerStringProperty(uint16_t propertyId, const char *propertyName,
                                                        const char *propertyDefaultValue) {
    this->registerProperty(propertyId, propertyName, (uint8_t *) propertyDefaultValue, strlen(propertyDefaultValue) + 1,
                           STRING_PROPERTY);
}

void NodeMQTTConfigManagerClass::registerBoolProperty(uint16_t propertyId, const char *propertyName,
                                                      uint8_t propertyDefaultValue) {
    this->registerProperty(propertyId, propertyName, &propertyDefaultValue, sizeof(uint8_t), BOOL_PROPERTY);
}

void NodeMQTTConfigManagerClass::storePropertiesInEEPROM() {
    properties->sort([](NodeMQTTProperty &a, NodeMQTTProperty &b) -> int { return (int) a.id - (int) b.id; });
    uint16_t propertyAddress = EEPROM_PROPERTIES_ADDRESS;
    for (int i = 0; i < this->properties->size(); i++) {
        NodeMQTTProperty property = this->properties->get(i);
        property.isStored = STORED;
        EEPROM.put(propertyAddress, property.id);
        propertyAddress += sizeof(uint16_t);
        EEPROM.put(propertyAddress, property.length);
        propertyAddress += sizeof(uint8_t);
        memcpy(EEPROM.getDataPtr() + propertyAddress, property.value, property.length);
        propertyAddress += property.length;
    }
    uint32_t chkSum = this->calculateEEPROMPropertyChkSum();
    Serial.println(chkSum);
    EEPROM.put(EEPROM_PROPERTY_CHCKSUM_ADDRESS, chkSum);
}

void NodeMQTTConfigManagerClass::loadPropertiesFromEEPROM() {
    uint16_t propertyEEPROMAddress = EEPROM_PROPERTIES_ADDRESS;
    uint8_t propertyEEPROMLength;
    for (int i = 0; i < this->properties->size(); i++) {
        NodeMQTTProperty ramProperty = this->properties->get(i);
        this->getEEPROMLocationById(ramProperty.id, propertyEEPROMAddress, propertyEEPROMLength);
        if (propertyEEPROMAddress != 0) {
            memcpy(ramProperty.value, EEPROM.getConstDataPtr() + propertyEEPROMAddress, propertyEEPROMLength);
            ramProperty.isStored = true;
        } else {
            Logger.logf(ERROR, F("Unable to locate %s in EEPROM. Please alter configuration to overwrite EEPROM."), ramProperty.name);
        }
        Serial.println();
        this->properties->set(i, ramProperty);
    }
    Serial.println("CHKSUM");
    Serial.println(isEEPROMValid());

}

void NodeMQTTConfigManagerClass::printIp(uint8_t ip[4]) {
    for (int i = 0; i < 4; i++) {
        NodeMQTTIO.print(ip[i]);
        if (i < 3)
            NodeMQTTIO.print('.');
    }
    NodeMQTTIO.println();
}

void NodeMQTTConfigManagerClass::printAttributeWithTab(String s) {
    NodeMQTTIO.print(s);
    NodeMQTTIO.print(TERMINAL_TAB);
    NodeMQTTIO.print(TERMINAL_TAB);
    if (s.length() <= 8) {
        NodeMQTTIO.print(TERMINAL_TAB);
    }
}

void NodeMQTTConfigManagerClass::factoryReset() {
    uint32_t storedChkSum;
    EEPROM.get(EEPROM_CONFIGURATION_CHCKSUM_ADDRESS, storedChkSum);
    EEPROM.put(EEPROM_CONFIGURATION_CHCKSUM_ADDRESS, storedChkSum + 1);

    commit();
    EEPROM.end();
    restartNode();
}

void NodeMQTTConfigManagerClass::print(NodeMQTTConfig *configuration) {
    NodeMQTTIO.println(TERMINAL_HR);
    printAttributeWithTab(ATTR_CONFVER);
    NodeMQTTIO.println(configuration->configVersion);

    printAttributeWithTab(ATTR_WIFISSID);
    NodeMQTTIO.println(configuration->wifiSsid);

    printAttributeWithTab(ATTR_WIFIPASS);
    NodeMQTTIO.println(configuration->wifiPassword);

    printAttributeWithTab(ATTR_WIFIBSSID);
    for (int i = 0; i < 6; i++) {
        NodeMQTTIO.print(configuration->wifiBssid[i], HEX);
        if (i < 5)
            NodeMQTTIO.print(':');
    }
    NodeMQTTIO.println();

    printAttributeWithTab(ATTR_IPADDRESS);
    this->printIp(configuration->ipAddress);

    printAttributeWithTab(ATTR_GATEWAY);
    this->printIp(configuration->gateway);

    printAttributeWithTab(ATTR_SUBNET);
    this->printIp(configuration->subnetMask);

    printAttributeWithTab(ATTR_DNS);
    this->printIp(configuration->dns);

    printAttributeWithTab(ATTR_WIFICHANNEL);
    NodeMQTTIO.println(configuration->wifiChannel);

    printAttributeWithTab(ATTR_MQTTSERV);
    NodeMQTTIO.println(configuration->mqttServer);

    printAttributeWithTab(ATTR_MQTTPORT);
    NodeMQTTIO.println(configuration->mqttPort);

    printAttributeWithTab(ATTR_MQTTUSER);
    NodeMQTTIO.println(configuration->mqttUser);

    printAttributeWithTab(ATTR_MQTTPASS);
    NodeMQTTIO.println(configuration->mqttPassword);

    printAttributeWithTab(ATTR_BASETOPIC);
    NodeMQTTIO.println(configuration->baseTopic);

    printAttributeWithTab(ATTR_ONLINE);
    NodeMQTTIO.println(configuration->isOnline);

    printAttributeWithTab(ATTR_SERVICEMODE);
    NodeMQTTIO.println(configuration->isServiceMode);

    printAttributeWithTab(ATTR_LOGGING);
    NodeMQTTIO.println(configuration->isLogging);

    NodeMQTTIO.println(TERMINAL_HR);
    for (int i = 0; i < this->properties->size(); i++) {
        NodeMQTTProperty property = this->properties->get(i);
        printAttributeWithTab(String(property.name));
        switch (property.type) {
            case INT_PROPERTY: {
                int *intValuePtr = (int *) property.value;
                NodeMQTTIO.println(*intValuePtr);
                break;
            }
            case STRING_PROPERTY:
                NodeMQTTIO.println((char *) property.value);
                break;
            case BOOL_PROPERTY:
                NodeMQTTIO.println(*property.value);
                break;
            case BYTE_PROPERTY:
            default:
                for (int i = 0; i < property.length; i++) {
                    NodeMQTTIO.print(property.value[i]);
                    NodeMQTTIO.print('\t');
                }
                NodeMQTTIO.println();
                break;
        }
    }
    if (this->properties->size() > 0)
        NodeMQTTIO.println(TERMINAL_HR);
}

LinkedList<NodeMQTTProperty> *NodeMQTTConfigManagerClass::getProperties() {
    return this->properties;
}

void NodeMQTTConfigManagerClass::commit() {
#if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
#endif
}

void NodeMQTTConfigManagerClass::getEEPROMLocationById(uint16_t propertyId, uint16_t &propertyAddress,
                                                       uint8_t &propertyLength) {
    uint16_t propertyIdBuffer;
    propertyAddress = EEPROM_PROPERTIES_ADDRESS;
    for (int i = 0; i < this->properties->size(); i++) {
        EEPROM.get(propertyAddress, propertyIdBuffer);
        propertyAddress += sizeof(uint16_t);
        EEPROM.get(propertyAddress, propertyLength);
        propertyAddress += sizeof(uint8_t);
        if (propertyIdBuffer == propertyId) return;
        propertyAddress += propertyLength;
    }
    propertyAddress = 0;
    return;
}

NodeMQTTProperty NodeMQTTConfigManagerClass::getRAMPropertyById(uint16_t propertyId) {
    for (int i = 0; i < this->properties->size(); i++) {
        NodeMQTTProperty property = this->properties->get(i);
        if (property.id == propertyId) return property;
    }
    return NodeMQTTProperty();
}

void NodeMQTTConfigManagerClass::setIntProperty(uint16_t propertyId, int propertyValue) {
    int *valuePtr = &propertyValue;
    this->setProperty(propertyId, (uint8_t *) valuePtr, sizeof(int));
}

void NodeMQTTConfigManagerClass::setStringProperty(uint16_t propertyId, const char *propertyValue) {
    this->setProperty(propertyId, (uint8_t *) propertyValue, strlen(propertyValue) + 1);
}

void NodeMQTTConfigManagerClass::setBoolProperty(uint16_t propertyId, uint8_t propertyValue) {
    this->setProperty(propertyId, &propertyValue, 1);
}

int NodeMQTTConfigManagerClass::getIntProperty(uint16_t propertyId) {
    uint8_t *ptr = this->getProperty(propertyId);
    int value = 0;
    memcpy(&value, ptr, sizeof(int));
    return value;
}

String NodeMQTTConfigManagerClass::getStringProperty(uint16_t propertyId) {
    NodeMQTTProperty property = this->getRAMPropertyById(propertyId);
    return String((char *) property.value);
}

uint8_t NodeMQTTConfigManagerClass::getBoolProperty(uint16_t propertyId) {
    uint8_t *ptr = this->getProperty(propertyId);
    uint8_t value = 0;
    memcpy(&value, ptr, sizeof(int));
    return value;
}

uint8_t *NodeMQTTConfigManagerClass::getProperty(uint16_t propertyId) {
    return this->getRAMPropertyById(propertyId).value;
}

void NodeMQTTConfigManagerClass::setProperty(uint16_t propertyId, uint8_t *propertyValue, uint8_t length) {
    for (int i = 0; i < this->properties->size(); i++) {
        NodeMQTTProperty property = this->properties->get(i);
        if (property.id == propertyId) {
            memcpy(property.value, propertyValue, length);
            isPropertyDirty = true;
            this->properties->set(i, property);
        }
    }
}

//TODO duplikalt ID ellenorzes es hibauzenet

NodeMQTTConfigManagerClass NodeMQTTConfigManager;
