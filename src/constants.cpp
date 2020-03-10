#include "constants.hpp"

//MELODY
const Note infoTone[1] PROGMEM = {{300, NOTE_C5, 0}};
const Note successTone[4] PROGMEM = {{150, NOTE_C5, 30}, {150, NOTE_E5, 30}, {150, NOTE_G5, 30}, {500, NOTE_C6, 0}};
const Note warningTone[2] PROGMEM = {{100, NOTE_C6, 0}, {100, NOTE_F6, 0}};
const Note errorTone[6] PROGMEM = {{100, NOTE_C4, 0}, {100, NOTE_F4, 0}, {100, NOTE_C4, 0}, {100, NOTE_F4, 0}, {100, NOTE_C4, 0}, {100, NOTE_F4, 0}};
const Note alertTone[5] PROGMEM = {{100, NOTE_C6, 0}, {100, NOTE_F6, 0}, {100, NOTE_C6, 0}, {100, NOTE_F6, 0}, {100, NOTE_C6, 0}};
const Note failTone[4] PROGMEM = {{300, NOTE_C6, 30}, {300, NOTE_G5, 30}, {300, NOTE_E5, 30}, {500, NOTE_C5, 0}};
const Note happyTone[5] PROGMEM = {{100, NOTE_C4, 0}, {100, NOTE_F4, 0}, {100, NOTE_C4, 0}, {100, NOTE_F4, 0}, {100, NOTE_C4, 0}};
const Note sadTone[5] PROGMEM = {{100, NOTE_C4, 0}, {100, NOTE_F4, 0}, {100, NOTE_C4, 0}, {100, NOTE_F4, 0}, {100, NOTE_C4, 0}};
const Note systemBoot[] PROGMEM = {{100, NOTE_C6, 0}};   // 1 ,{100,NOTE_E5,0}}; //2
const Note systemOnline[] PROGMEM = {{100, NOTE_G6, 0}}; // 1 ,{100,NOTE_C6,0}}; //2

//SYSTEM
#ifdef ESP8266
// #define DEVICE_NAME String(ESP.getChipId(), HEX)
uint32_t UUID = ESP.getChipId();
#endif
#ifdef ESP32
// #define DEVICE_NAME String((uint32_t)ESP.getEfuseMac(), HEX)
uint32_t UUID = (uint32_t)ESP.getEfuseMac();
#endif
const char terminalHR[] PROGMEM = "==============================================";
const char terminalPrompt[] PROGMEM = "> ";

const char terminalHeader[] PROGMEM = "\
  _  _         _     __  __  ___ _____ _____ \n\r\
 | \\| |___  __| |___|  \\/  |/ _ \\_   _|_   _|\n\r\
 | .` / _ \\/ _` / -_) |\\/| | (_) || |   | |  \n\r\
 |_|\\_\\___/\\__,_\\___|_|  |_|\\__\\_\\|_|   |_|  \n\r";

//TOPICS
const char offlineMessage[] PROGMEM = "OFFLINE";
const char configTopic[] PROGMEM = "cfg";
const char configEchoTopic[] PROGMEM = "cfgecho";
const char logTopic[] PROGMEM = "log";
const char heartbeatTopic[] PROGMEM = "hrtbt";
const char languageTopic[] PROGMEM = "lang";
const char commandTopic[] PROGMEM = "cmd";
const char terminalTab[] PROGMEM = "\t";

//INTERFACE NAMES
const char analogName[] PROGMEM = "nlg";
const char buttonName[] PROGMEM = "btn";
const char buttonArrayName[] PROGMEM = "btns";
const char relayName[] PROGMEM = "rly";
const char stringName[] PROGMEM = "str";
const char buzzerName[] PROGMEM = "buz";
const char dfplayerName[] PROGMEM = "dfp";
const char pwmName[] PROGMEM = "pwm";
const char dataInterfaceName[] PROGMEM = "data";
const char arrayInterfaceName[] PROGMEM = "array";
const char languageInterfaceName[] PROGMEM = "lang";
const char commandInterfaceName[] PROGMEM = "cmd";
const char fsmInterfaceName[] PROGMEM = "FSM";
const char PCF8574InterfaceName[] PROGMEM = "PCF8574";
const char PCA9685InterfaceName[] PROGMEM = "PCA9685";
const char interfaceName[] PROGMEM = "INTERFACE";

//CONFIGURATION
const char configAttribute_baseTopic[] PROGMEM = "basetpc";
const char configAttribute_mqttPassword[] PROGMEM = "mqttpwd";
const char configAttribute_mqttServer[] PROGMEM = "mqttsrv";
const char configAttribute_mqttUser[] PROGMEM = "mqttusr";
const char configAttribute_wifiPassword[] PROGMEM = "wifipwd";
const char configAttribute_wifiSsid[] PROGMEM = "wifissid";
const char configAttribute_wifiBssid[] PROGMEM = "wifibssid";
const char configAttribute_ipAddress[] PROGMEM = "ipaddr";
const char configAttribute_gateway[] PROGMEM = "gateway";
const char configAttribute_subnet[] PROGMEM = "subnet";
const char configAttribute_dns[] PROGMEM = "dns";
const char configAttribute_wifiChannel[] PROGMEM = "wifichn";
const char configAttribute_configVersion[] PROGMEM = "confver";
const char configAttribute_isOnline[] PROGMEM = "on-line";
const char configAttribute_isServiceMode[] PROGMEM = "service";
const char configAttribute_mqttPort[] PROGMEM = "mqttprt";
const char configAttribute_isLogging[] PROGMEM = "logging";

//MESSAGES
const char message_Introduction[] PROGMEM = "Hello! I am %s. (build time: %s, unix: %d)";
const char message_ValueChanged[] PROGMEM = "Value changed on %s (%s, pub:%d) - %s";
const char message_Publishing[] PROGMEM = "Publishing: %s";
const char message_FailedToParse[] PROGMEM = "Failed to parse JSON data @ %s";
const char message_UnknownState[] PROGMEM = "There is no state like [%s]";
const char message_MQTTBrokerConnectionAttempt[] PROGMEM = "%s is attempting to connect to broker. #%d";
const char message_MQTTBrokerDisconnected[] PROGMEM = "Connection to the server has been lost. Trying to reconnect! RC=[%d]";
const char message_MQTTBrokerCouldNotConnect[] PROGMEM = "Failed to connect broker. RC=[%d]";
const char message_ContextIsMissing[] PROGMEM = "Context is missing. Did you register %s interface? (@%s)";
const char message_ConnetctToWifi[] PROGMEM = "Connecting to %s. #%d";
const char message_ConnetctToWifiWithBSSID[] PROGMEM = "Connecting to %s with BSSID. #%d";

//FIRMWARE
const long FIRMWARE_BUILD_TIME PROGMEM = UNIX_TIMESTAMP;
const char LOG_FORMAT_MILLIS[] PROGMEM = "[%c|%012lu] %s";
const char LOG_FORMAT_NTP[]    PROGMEM = "[%c|%s] %s";


// const char logPrefixNTP[] PROGMEM     = "[%c|%12lu] %" TOSTRING(LOG_MAX_PRINT_LENGTH)  "s";
