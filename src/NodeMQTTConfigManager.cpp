#include "NodeMQTTConfigManager.hpp"

NodeMQTTConfigManagerClass::NodeMQTTConfigManagerClass()
{

    setStaticFlagDefaultValues();
    // if (!NodeMQTTConfigManagerClass::EEPROMInitialized)
    {
        EEPROM.begin(512);
        // NodeMQTTConfigManagerClass::EEPROMInitialized = true;
    }
    // Serial.println(EEPROM.length());
    // if (!EEPROM.begin(EEPROM.length()))
    // {
    //     Serial.println("Failed to initialise EEPROM");
    //     Serial.println("Restarting...");
    //     delay(1000);
    //     ESP.restart();
    // }
}

void NodeMQTTConfigManagerClass::setStaticFlagDefaultValues()
{
    // NodeMQTTConfigManagerClass::I2CInitialized = false;
    // NodeMQTTConfigManagerClass::EEPROMInitialized = false;
}

uint32_t NodeMQTTConfigManagerClass::calculateChkSum(NodeMQTTConfig &config)
{
    int i;
    int len = sizeof(NodeMQTTConfig);
    uint32_t checksum;
    char *data = (char *)&config;
    for (i = 0, checksum = 0; i < len; i++)
        checksum += data[i];
    return checksum;
}

void NodeMQTTConfigManagerClass::save(NodeMQTTConfig &configuration)
{
    i(F("Storing configuration in EEPROM."));
    configuration.configVersion = DEFAULT_CONFIGURATION_VERSION;
    uint32_t chksum = calculateChkSum(configuration);

    EEPROM.put(EEPROM_CONFIGURATION_CHCKSUM_ADDRESS, chksum);
    EEPROM.put(EEPROM_CONFIGURATION_ADDRESS, configuration);
    EEPROM.commit();
    i(F("Node restarts. Good bye!"));
    restartNode();
}

void NodeMQTTConfigManagerClass::loadInto(NodeMQTTConfig &configuration)
{
    i(F("Loading configuration from EEPROM."));
    uint32_t storedChkSum;
    EEPROM.get(EEPROM_CONFIGURATION_CHCKSUM_ADDRESS, storedChkSum);
    EEPROM.get(EEPROM_CONFIGURATION_ADDRESS, configuration);
    uint32_t calculatedChkSum = calculateChkSum(configuration);
    if (calculatedChkSum != storedChkSum || configuration.configVersion != DEFAULT_CONFIGURATION_VERSION)
    {
        i(F("Configuration is invalid in EEPROM."));
        loadDefaultsInto(configuration);
    }
}

void NodeMQTTConfigManagerClass::loadDefaultsInto(NodeMQTTConfig &configuration)
{
    i(F("Loading default configuration."));
    configuration = defaultConfig;
    save(configuration);
}

void NodeMQTTConfigManagerClass::setDefault(NodeMQTTConfig &configuration)
{
    defaultConfig = configuration;
}

void NodeMQTTConfigManagerClass::addProperty(uint8_t propertyId, const char *propertyName, uint32_t propertyDefaultValue)
{
    NodeMQTTProperty *property = new NodeMQTTProperty(propertyId, propertyName, propertyDefaultValue, false);
    properties.add(property);
}
uint32_t NodeMQTTConfigManagerClass::getProperty(uint8_t propertyId)
{
    for (int i = 0; i < properties.size(); i++)
    {
        NodeMQTTProperty *property = properties.get(i);
        if (property->id == propertyId)
            return property->value;
    }
    return 0;
}
void NodeMQTTConfigManagerClass::setProperty(uint8_t propertyId, uint32_t propertyValue)
{
    for (int i = 0; i < properties.size(); i++)
    {
        NodeMQTTProperty *property = properties.get(i);
        if (property->id == propertyId)
            property->value = propertyValue;
    }
}
void NodeMQTTConfigManagerClass::storePropertiesInEEPROM()
{
    uint16_t propertyAddress = EEPROM_PROPERTIES_ADDRESS;
    for (int i = 0; i < properties.size(); i++)
    {
        NodeMQTTProperty *property = properties.get(i);
        property->isStored = true;
        EEPROM.put(propertyAddress, property);
        propertyAddress += sizeof(NodeMQTTProperty);
    }
}
void NodeMQTTConfigManagerClass::loadPropertiesFromEEPROM()
{
    uint16_t propertyAddress = EEPROM_PROPERTIES_ADDRESS;
    for (int i = 0; i < properties.size(); i++)
    {
        NodeMQTTProperty *property = properties.get(i);
        NodeMQTTProperty *eepromProperty;
        EEPROM.get(propertyAddress, eepromProperty);

        if (eepromProperty->isStored)
            property->value = eepromProperty->value;

        free(eepromProperty);
        propertyAddress += sizeof(NodeMQTTProperty);
    }
}
void NodeMQTTConfigManagerClass::printIp(uint8_t ip[4])
{
    for (int i = 0; i < 4; i++)
    {
        Serial.print(ip[i]);
        if (i < 3)
            Serial.print('.');
    }
    Serial.println();
}

void NodeMQTTConfigManagerClass::printAttributeWithTab(String s) {
    Serial.print(s);
    Serial.print(TERMINAL_TAB);
    if (s.length()<8)
        Serial.print(TERMINAL_TAB);
}

void NodeMQTTConfigManagerClass::factoryReset()
{
    uint32_t storedChkSum;
    EEPROM.get(EEPROM_CONFIGURATION_CHCKSUM_ADDRESS, storedChkSum);
    EEPROM.put(EEPROM_CONFIGURATION_CHCKSUM_ADDRESS, storedChkSum + 1);
    EEPROM.commit();
    restartNode();
}

void NodeMQTTConfigManagerClass::print(NodeMQTTConfig &configuration)
{
    Serial.println(TERMINAL_HR);
    printAttributeWithTab(ATTR_CONFVER);    
    Serial.println(configuration.configVersion);

    printAttributeWithTab(ATTR_WIFISSID);
    Serial.println(configuration.wifiSsid);

    printAttributeWithTab(ATTR_WIFIPASS);
    Serial.println(configuration.wifiPassword);

    printAttributeWithTab(ATTR_WIFIBSSID);    
    for (int i = 0; i < 6; i++)
    {
        Serial.print(configuration.wifiBssid[i], HEX);
        if (i < 5)
            Serial.print(':');
    }
    Serial.println();

    printAttributeWithTab(ATTR_IPADDRESS);    
    this->printIp(configuration.ipAddress);

    printAttributeWithTab(ATTR_GATEWAY);    
    this->printIp(configuration.gateway);

    printAttributeWithTab(ATTR_SUBNET);
    this->printIp(configuration.subnetMask);

    printAttributeWithTab(ATTR_DNS);
    this->printIp(configuration.dns);

    printAttributeWithTab(ATTR_WIFICHANNEL);
    Serial.println(configuration.wifiChannel);

    printAttributeWithTab(ATTR_MQTTSERV);
    Serial.println(configuration.mqttServer);

    printAttributeWithTab(ATTR_MQTTPORT);    
    Serial.println(configuration.mqttPort);
    
    printAttributeWithTab(ATTR_MQTTUSER);
    Serial.println(configuration.mqttUser);
    
    printAttributeWithTab(ATTR_MQTTPASS);
    Serial.println(configuration.mqttPassword);

    printAttributeWithTab(ATTR_BASETOPIC);    
    Serial.println(configuration.baseTopic);
    
    printAttributeWithTab(ATTR_ONLINE);
    Serial.println(configuration.isOnline);
    
    printAttributeWithTab(ATTR_SERVICEMODE);
    Serial.println(configuration.isServiceMode);
    
    printAttributeWithTab(ATTR_LOGGING);
    Serial.println(configuration.isLogging);
    
    Serial.println(TERMINAL_HR);
    for (int i = 0; i < properties.size(); i++)
    {
        NodeMQTTProperty *property = properties.get(i);
        String sPropertyName = String(property->name);
        printAttributeWithTab(sPropertyName);
        Serial.println(property->value);
    }
    if (properties.size() > 0)
        Serial.println(TERMINAL_HR);
}
NodeMQTTConfigManagerClass NodeMQTTConfigManager;