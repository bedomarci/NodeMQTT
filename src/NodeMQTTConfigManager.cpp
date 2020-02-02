#include "NodeMQTTConfigManager.hpp"
#include "NodeMQTTIOContainer.hpp"
#include "misc/typedef.hpp"

#define STORED 85

NodeMQTTConfigManagerClass::NodeMQTTConfigManagerClass()
{
    EEPROM_CONFIGURATION_ADDRESS = EEPROM_CONFIGURATION_CHCKSUM_ADDRESS + sizeof(uint32_t);
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

void NodeMQTTConfigManagerClass::setStaticFlagDefaultValues()
{
    // NodeMQTTConfigManagerClass::I2CInitialized = false;
    // NodeMQTTConfigManagerClass::EEPROMInitialized = false;
}

uint32_t NodeMQTTConfigManagerClass::calculateChkSum(NodeMQTTConfig *config)
{
    int i;
    int len = sizeof(NodeMQTTConfig);
    uint32_t checksum;
    char *data = (char *)config;
    for (i = 0, checksum = 0; i < len; i++)
        checksum += data[i];
    return checksum;
}

void NodeMQTTConfigManagerClass::save(NodeMQTTConfig *configuration)
{
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

void NodeMQTTConfigManagerClass::loadInto(NodeMQTTConfig *configuration)
{
    i(F("Loading configuration from EEPROM."));
    uint32_t storedChkSum;
    EEPROM.get(EEPROM_CONFIGURATION_CHCKSUM_ADDRESS, storedChkSum);
    EEPROM.get(EEPROM_CONFIGURATION_ADDRESS, *configuration);
    uint32_t calculatedChkSum = calculateChkSum(configuration);
    if (calculatedChkSum != storedChkSum || configuration->configVersion != DEFAULT_CONFIGURATION_VERSION)
    {
        i(F("Configuration is invalid in EEPROM."));
        loadDefaultsInto(configuration);
    }
}
//
void NodeMQTTConfigManagerClass::loadDefaultsInto(NodeMQTTConfig *configuration)
{
    i(F("Loading default configuration."));
    configuration = new NodeMQTTConfig();
    save(configuration);
}

void NodeMQTTConfigManagerClass::registerProperty(uint8_t propertyId, const char *propertyName, int32 propertyDefaultValue)
{
    this->properties->add(NodeMQTTProperty(propertyId, propertyName, propertyDefaultValue, 0));
}

int32 NodeMQTTConfigManagerClass::getProperty(uint8_t propertyId)
{
    for (int i = 0; i < this->properties->size(); i++)
    {
        NodeMQTTProperty property = this->properties->get(i);
        if (property.id == propertyId)
            return property.value;
    }
    return 0;
}
void NodeMQTTConfigManagerClass::setProperty(uint8_t propertyId, int32 propertyValue)
{
    for (int i = 0; i < this->properties->size(); i++)
    {
        NodeMQTTProperty property = this->properties->get(i);
        if (property.id == propertyId) {
            property.value = propertyValue;
            isPropertyDirty = true;
            this->properties->set(i, property);
        }
    }
}
void NodeMQTTConfigManagerClass::storePropertiesInEEPROM()
{
    uint16_t propertyAddress = EEPROM_PROPERTIES_ADDRESS;
    for (int i = 0; i < this->properties->size(); i++)
    {
        NodeMQTTProperty property = this->properties->get(i);
        property.isStored = STORED;
        EEPROM.put(propertyAddress, property);
        propertyAddress += sizeof(NodeMQTTProperty);
    }
}
void NodeMQTTConfigManagerClass::loadPropertiesFromEEPROM()
{
    uint16_t propertyAddress = EEPROM_PROPERTIES_ADDRESS;
    for (int i = 0; i < this->properties->size(); i++)
    {
        NodeMQTTProperty eepromProperty;
        EEPROM.get(propertyAddress, eepromProperty);
        if (eepromProperty.isStored == STORED){
            NodeMQTTProperty property = this->properties->get(i);
            property.value = eepromProperty.value;
            this->properties->set(i, property);
        }
        propertyAddress += sizeof(NodeMQTTProperty);
    }
}
void NodeMQTTConfigManagerClass::printIp(uint8_t ip[4])
{
    for (int i = 0; i < 4; i++)
    {
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

void NodeMQTTConfigManagerClass::factoryReset()
{
    uint32_t storedChkSum;
    EEPROM.get(EEPROM_CONFIGURATION_CHCKSUM_ADDRESS, storedChkSum);
    EEPROM.put(EEPROM_CONFIGURATION_CHCKSUM_ADDRESS, storedChkSum + 1);
    
    commit();
    EEPROM.end();
    restartNode();
}

void NodeMQTTConfigManagerClass::print(NodeMQTTConfig *configuration)
{
    NodeMQTTIO.println(TERMINAL_HR);
    printAttributeWithTab(ATTR_CONFVER);
    NodeMQTTIO.println(configuration->configVersion);

    printAttributeWithTab(ATTR_WIFISSID);
    NodeMQTTIO.println(configuration->wifiSsid);

    printAttributeWithTab(ATTR_WIFIPASS);
    NodeMQTTIO.println(configuration->wifiPassword);

    printAttributeWithTab(ATTR_WIFIBSSID);
    for (int i = 0; i < 6; i++)
    {
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
    for (int i = 0; i < this->properties->size(); i++)
    {
        NodeMQTTProperty property = this->properties->get(i);
        printAttributeWithTab(String(property.name));
        NodeMQTTIO.println(property.value);
    }
    if (this->properties->size() > 0)
        NodeMQTTIO.println(TERMINAL_HR);
}

LinkedList<NodeMQTTProperty>* NodeMQTTConfigManagerClass::getProperties(){
    return this->properties;
}

void NodeMQTTConfigManagerClass::commit () {
#if defined(ESP8266) || defined(ESP32)
  EEPROM.commit();
#endif
}

NodeMQTTConfigManagerClass NodeMQTTConfigManager;