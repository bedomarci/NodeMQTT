#include "NodeMQTTConfigManager.hpp"
#include "NodeMQTTIOContainer.hpp"

#define STORED 85

NodeMQTTConfigManagerClass::NodeMQTTConfigManagerClass() {
    properties = new LinkedList<NodeMQTTProperty>();
}

uint8_t NodeMQTTConfigManagerClass::isEEPROMValid() {
    uint32_t calculatedChkSum = this->calculateEEPROMPropertyChkSum();
    uint32_t storedChkSum;
    EEPROM.get(EEPROM_PROPERTY_CHCKSUM_ADDRESS, storedChkSum);
    return calculatedChkSum == storedChkSum;
}

uint32_t NodeMQTTConfigManagerClass::calculateEEPROMPropertyChkSum() {
    uint16_t address = EEPROM_PROPERTIES_ADDRESS;
    uint32_t sum = 0;
    uint8_t length = 0;
    for (int i = 0; i < this->properties->size(); i++) {
        address += sizeof(uint16_t); //skipping property id
        EEPROM.get(address, length);
        address += sizeof(uint8_t);
        for (int j = 0; j < length; j++)
            sum += EEPROM.read(address++);
    }
    return sum;
}

void NodeMQTTConfigManagerClass::save() {
    i(F("Storing configuration in EEPROM."));
    EEPROM.begin(EEPROM_SIZE);
    if (isPropertyDirty) {
        storePropertiesInEEPROM();
    }
    commit();
    EEPROM.end();
    i(F("Node restarts. Good bye!"));
    restartNode();
}

void NodeMQTTConfigManagerClass::load() {
    EEPROM.begin(EEPROM_SIZE);
    if (!isEEPROMValid())
        Logger.logf(ERROR, F("Broken data integrity in EEPROM! Configuration is still loaded. Please review properties and resave!"));
    loadPropertiesFromEEPROM();
//    i(F("Loading configuration from EEPROM."));
    EEPROM.end();
}


void NodeMQTTConfigManagerClass::registerProperty(uint16_t propertyId, const char *propertyName, void *propertyDefaultValue, uint8_t length, NodeMQTTPropertyType type) {
    if (isLoaded) {
        e(F("Registering property after EEPROM load is prohibited! Move property registration before loading!"));
        return;
    }
    if (isIdRegistered(propertyId)) {
        Logger.logf(ERROR, F("Property ID #%d is already registered."), propertyId);
        return;
    }
    NodeMQTTProperty property = NodeMQTTProperty(propertyId, propertyName);
    property.length = length;
    property.type = type;
    memcpy(property.value, propertyDefaultValue, length);
    this->properties->add(property);
}

void NodeMQTTConfigManagerClass::registerIntProperty(uint16_t propertyId, const char *propertyName, int propertyDefaultValue) {
    this->registerProperty(propertyId, propertyName, (uint8_t *) &propertyDefaultValue, sizeof(int), INT_PROPERTY);
}

void NodeMQTTConfigManagerClass::registerStringProperty(uint16_t propertyId, const char *propertyName, const char *propertyDefaultValue) {
    this->registerProperty(propertyId, propertyName, (uint8_t *) propertyDefaultValue, strlen(propertyDefaultValue) + 1, STRING_PROPERTY);
}

void NodeMQTTConfigManagerClass::registerBoolProperty(uint16_t propertyId, const char *propertyName, bool propertyDefaultValue) {
    this->registerProperty(propertyId, propertyName, (uint8_t *) &propertyDefaultValue, sizeof(bool), BOOL_PROPERTY);
}

void NodeMQTTConfigManagerClass::registerIPProperty(uint16_t propertyId, const char *propertyName, uint8_t propertyDefaultValue[4]) {
    this->registerProperty(propertyId, propertyName, propertyDefaultValue, 4, IP_PROPERTY);
}

void NodeMQTTConfigManagerClass::registerMACProperty(uint16_t propertyId, const char *propertyName,
                                                     uint8_t propertyDefaultValue[6]) {
    this->registerProperty(propertyId, propertyName, propertyDefaultValue, 6, MAC_PROPERTY);
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
    EEPROM.put(EEPROM_PROPERTY_CHCKSUM_ADDRESS, chkSum);
}

void NodeMQTTConfigManagerClass::loadPropertiesFromEEPROM() {
    uint16_t propertyEEPROMAddress = EEPROM_PROPERTIES_ADDRESS;
    uint8_t propertyEEPROMLength;
    for (int i = 0; i < this->properties->size(); i++) {
        NodeMQTTProperty ramProperty = this->properties->get(i);
        this->getEEPROMLocationById(ramProperty.id, propertyEEPROMAddress, propertyEEPROMLength);

        if (propertyEEPROMAddress != 0) {
            memcpy(ramProperty.value, EEPROM.getDataPtr() + propertyEEPROMAddress, propertyEEPROMLength);
            ramProperty.isStored = true;
            ramProperty.length = propertyEEPROMLength;
        } else {
            Logger.logf(ERROR, F("Unable to locate %s in EEPROM. Please alter configuration to overwrite EEPROM."), ramProperty.name);
        }
        this->properties->set(i, ramProperty);
    }
    isLoaded = true;
}

void NodeMQTTConfigManagerClass::printAttributeWithTab(String s) {
    NodeMQTTIO.print(s);
    NodeMQTTIO.print(TERMINAL_TAB);
    NodeMQTTIO.print(TERMINAL_TAB);
    if (s.length() < 8) {
        NodeMQTTIO.print(TERMINAL_TAB);
    }
}

void NodeMQTTConfigManagerClass::factoryReset() {
    EEPROM.begin(EEPROM_SIZE);
    for (int i = 0; i < EEPROM_SIZE; i++) {
        EEPROM.write(i, 0xFF);
    }
    commit();
    EEPROM.end();
    restartNode();
}

void NodeMQTTConfigManagerClass::print() {
    NodeMQTTIO.println(TERMINAL_HR);
    for (int i = 0; i < this->properties->size(); i++) {
        NodeMQTTProperty property = this->properties->get(i);
        printAttributeWithTab(FPSTR(property.name));
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
                NodeMQTTIO.println((bool) *property.value);
                break;
            case IP_PROPERTY:
                printIp(NodeMQTTIO, property.value);
                NodeMQTTIO.println();
                break;
            case MAC_PROPERTY:
                printMac(NodeMQTTIO, property.value);
                NodeMQTTIO.println();
                break;
            case BYTE_PROPERTY:
            default:
                this->printArraySeparated(property.value, property.length, '\t');
                NodeMQTTIO.println();
                break;
        }
    }
    NodeMQTTIO.println(TERMINAL_HR);
}

void NodeMQTTConfigManagerClass::printArraySeparated(uint8_t *array, uint8_t length, char separator) {
    for (int i = 0; i < length - 1; i++) {
        NodeMQTTIO.print(array[i]);
        NodeMQTTIO.print(separator);
    }
    NodeMQTTIO.print(array[length - 1]);
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
    if (!isLoaded) {
        Logger.logf(ERROR, F("Getting property [id: %d] before EEPROM load! Move getters after loading!"), propertyId);
    }
    for (int i = 0; i < this->properties->size(); i++) {
        NodeMQTTProperty property = this->properties->get(i);
        if (property.id == propertyId) return property;
    }
    return NodeMQTTProperty();
//    return nullptr;
}

void NodeMQTTConfigManagerClass::setIntProperty(uint16_t propertyId, int propertyValue) {
    int *valuePtr = &propertyValue;
    this->setProperty(propertyId, (uint8_t *) valuePtr, sizeof(int));
}

void NodeMQTTConfigManagerClass::setStringProperty(uint16_t propertyId, const char *propertyValue) {
    this->setProperty(propertyId, (uint8_t *) propertyValue, strlen(propertyValue) + 1);
}

void NodeMQTTConfigManagerClass::setBoolProperty(uint16_t propertyId, bool propertyValue) {
    this->setProperty(propertyId, &propertyValue, sizeof(bool));
}

void NodeMQTTConfigManagerClass::setIPProperty(uint16_t propertyId, uint8_t propertyValue[4]) {
    this->setProperty(propertyId, propertyValue, 4);
}

void NodeMQTTConfigManagerClass::setMACProperty(uint16_t propertyId, uint8_t propertyValue[6]) {
    this->setProperty(propertyId, propertyValue, 6);
}

int NodeMQTTConfigManagerClass::getIntProperty(uint16_t propertyId) {
    uint8_t *ptr = this->getRAMPropertyById(propertyId).value;
    int value = 0;
    memcpy(&value, ptr, sizeof(int));
    return value;
}

String NodeMQTTConfigManagerClass::getStringProperty(uint16_t propertyId) {
    auto property = this->getRAMPropertyById(propertyId);
    return String((char *) property.value);
}

bool NodeMQTTConfigManagerClass::getBoolProperty(uint16_t propertyId) {
    auto property = this->getRAMPropertyById(propertyId);
    return (bool) *property.value;
}

void NodeMQTTConfigManagerClass::getIPProperty(uint16_t propertyId, uint8_t ipArray[4]) {
    auto property = this->getRAMPropertyById(propertyId);
    memcpy(ipArray, property.value, 4);
}

void NodeMQTTConfigManagerClass::getMACProperty(uint16_t propertyId, uint8_t ipArray[6]) {
    auto property = this->getRAMPropertyById(propertyId);
    memcpy(ipArray, property.value, 4);
}

void NodeMQTTConfigManagerClass::getProperty(uint16_t propertyId, void *propertyValue, uint8_t length) {
    NodeMQTTProperty property = this->getRAMPropertyById(propertyId);
    memcpy(propertyValue, property.value, length);
}

void NodeMQTTConfigManagerClass::setProperty(uint16_t propertyId, const void *propertyValue, uint8_t length) {
    for (int i = 0; i < this->properties->size(); i++) {
        NodeMQTTProperty property = this->properties->get(i);
        if (property.id == propertyId) {
            memcpy(property.value, propertyValue, length);
            property.length = length;
            isPropertyDirty = true;
            this->properties->set(i, property);
        }
    }
}

bool NodeMQTTConfigManagerClass::isIdRegistered(uint16_t id) {
    bool registered = false;
    for (int i = 0; i < this->properties->size(); i++) {
        NodeMQTTProperty property = this->properties->get(i);
        if (property.id == id) registered = true;
    }
    return registered;
}

//TODO duplikalt ID ellenorzes es hibauzenet

NodeMQTTConfigManagerClass NodeMQTTConfigManager;
