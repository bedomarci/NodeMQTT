#include "NodeMQTTConfigManager.hpp"

NodeMQTTConfigManagerClass::NodeMQTTConfigManagerClass()
{
    EEPROM.begin(512);
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
    D_CONF(F("Storing configuration in EEPROM."));
    configuration.configVersion = DEFAULT_CONFIGURATION_VERSION;
    uint32_t chksum = calculateChkSum(configuration);

    EEPROM.put(EEPROM_CONFIGURATION_CHCKSUM_ADDRESS, chksum);
    EEPROM.put(EEPROM_CONFIGURATION_ADDRESS, configuration);
    EEPROM.commit();
    D_CONF(F("Node restarts. Good bye!"));
    ESP.reset();
}

void NodeMQTTConfigManagerClass::loadInto(NodeMQTTConfig &configuration)
{
    D_CONF(F("Loading configuration from EEPROM."));
    uint32_t storedChkSum;
    EEPROM.get(EEPROM_CONFIGURATION_CHCKSUM_ADDRESS, storedChkSum);
    EEPROM.get(EEPROM_CONFIGURATION_ADDRESS, configuration);
    uint32_t calculatedChkSum = calculateChkSum(configuration);
    if (calculatedChkSum != storedChkSum || configuration.configVersion != DEFAULT_CONFIGURATION_VERSION)
    {
        D_CONF(F("Configuration is invalid in EEPROM."));
        loadDefaultsInto(configuration);
    }
}

void NodeMQTTConfigManagerClass::loadDefaultsInto(NodeMQTTConfig &configuration)
{
    D_CONF(F("Loading default configuration."));
    configuration = defaultConfig;
    save(configuration);
}

void NodeMQTTConfigManagerClass::setDefault(NodeMQTTConfig &configuration)
{
    defaultConfig = configuration;
}

void NodeMQTTConfigManagerClass::print(NodeMQTTConfig &configuration)
{
    Serial.println(TERMINAL_HR);
    Serial.print(ATTR_CONFVER);
    Serial.print("\t");
    Serial.println(configuration.configVersion);
    Serial.print(ATTR_WIFISSID);
    Serial.print("\t");
    Serial.println(configuration.wifiSsid);
    Serial.print(ATTR_WIFIPASS);
    Serial.print("\t");
    Serial.println(configuration.wifiPassword);
    Serial.print(ATTR_MQTTSERV);
    Serial.print("\t");
    Serial.println(configuration.mqttServer);
    Serial.print(ATTR_MQTTPORT);
    Serial.print("\t");
    Serial.println(configuration.mqttPort);
    Serial.print(ATTR_MQTTUSER);
    Serial.print("\t");
    Serial.println(configuration.mqttUser);
    Serial.print(ATTR_MQTTPASS);
    Serial.print("\t");
    Serial.println(configuration.mqttPassword);
    Serial.print(ATTR_BASETOPIC);
    Serial.print("\t");
    Serial.println(configuration.baseTopic);
    Serial.print(ATTR_ONLINE);
    Serial.print("\t");
    Serial.println(configuration.isOnline);
    Serial.print(ATTR_SERVICEMODE);
    Serial.print("\t");
    Serial.println(configuration.isServiceMode);
    Serial.println(TERMINAL_HR);
}
NodeMQTTConfigManagerClass NodeMQTTConfigManager;