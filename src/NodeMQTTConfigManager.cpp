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
#ifdef ESP8266
    ESP.reset();
#endif
#ifdef ESP32
    ESP.restart();
#endif
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

void NodeMQTTConfigManagerClass::print(NodeMQTTConfig &configuration)
{
    Serial.println(TERMINAL_HR);
    Serial.print(ATTR_CONFVER);
    Serial.print(TERMINAL_TAB);
    Serial.println(configuration.configVersion);
    Serial.print(ATTR_WIFISSID);
    Serial.print(TERMINAL_TAB);
    Serial.println(configuration.wifiSsid);
    Serial.print(ATTR_WIFIPASS);
    Serial.print(TERMINAL_TAB);
    Serial.println(configuration.wifiPassword);
    Serial.print(ATTR_WIFIBSSID);
    Serial.print(TERMINAL_TAB);
    for (int i = 0; i < 6; i++)
        Serial.print(configuration.wifiBssid[i], HEX);
    Serial.println();
    Serial.print(ATTR_WIFICHANNEL);
    Serial.print(TERMINAL_TAB);
    Serial.println(configuration.wifiChannel);
    Serial.print(ATTR_MQTTSERV);
    Serial.print(TERMINAL_TAB);
    Serial.println(configuration.mqttServer);
    Serial.print(ATTR_MQTTPORT);
    Serial.print(TERMINAL_TAB);
    Serial.println(configuration.mqttPort);
    Serial.print(ATTR_MQTTUSER);
    Serial.print(TERMINAL_TAB);
    Serial.println(configuration.mqttUser);
    Serial.print(ATTR_MQTTPASS);
    Serial.print(TERMINAL_TAB);
    Serial.println(configuration.mqttPassword);
    Serial.print(ATTR_BASETOPIC);
    Serial.print(TERMINAL_TAB);
    Serial.println(configuration.baseTopic);
    Serial.print(ATTR_ONLINE);
    Serial.print(TERMINAL_TAB);
    Serial.println(configuration.isOnline);
    Serial.print(ATTR_SERVICEMODE);
    Serial.print(TERMINAL_TAB);
    Serial.println(configuration.isServiceMode);
    Serial.print(ATTR_LOGGING);
    Serial.print(TERMINAL_TAB);
    Serial.println(configuration.isLogging);
    Serial.println(TERMINAL_HR);
}
NodeMQTTConfigManagerClass NodeMQTTConfigManager;