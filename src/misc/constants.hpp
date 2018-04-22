//CONSTANTS FILE
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

//BUZZER
#define TONE_INFO 0
#define TONE_SUCCESS 1
#define TONE_WARNING 2
#define TONE_ERROR 3
#define TONE_ALERT 4
#define TONE_FAIL 5
#define TONE_HAPPY 6
#define TONE_SAD 7
#define TONE_SYSTEM_BOOT 8
#define TONE_SYSTEM_ONLINE 9

//NODES
// static const char configTopic[] PROGMEM = "config";
// #define CONFIG_TOPIC FPSTR(configTopic)
// static const char logTopic[] PROGMEM = "log";
// #define LOG_TOPIC FPSTR(logTopic)
// static const char PROGMEM heartbeatTopic[] = "hrtbt";
// #define HEARTBEAT_TOPIC FPSTR(heartbeatTopic)
// static const char terminalHR[] PROGMEM = "\t============================\t";
// #define TERMINAL_HR FPSTR(terminalHR)

#define WILL_MESSAGE "OFFLINE"
#define CONFIG_TOPIC ("config")
#define CONFIG_TOPIC_ECHO F("configecho")
#define LOG_TOPIC ("log")
#define HEARTBEAT_TOPIC ("hrtbt")
#define TERMINAL_HR ("\t============================\t")

//INTERFACE NAMES
// static constexpr char analogName[] PROGMEM = "nlg";
// static constexpr char buttonName[] PROGMEM = "btn";
// static constexpr char relayName[] PROGMEM = "rly";
// static constexpr char stringName[] PROGMEM = "str";
// static constexpr char buzzerName[] PROGMEM = "buz";
// static constexpr char dfplayerName[] PROGMEM = "dfp";
// #define ANALOG_NAME FPSTR(analogName)
// #define BUTTON_NAME FPSTR(buttonName)
// #define RELAY_NAME FPSTR(relayName)
// #define STRING_NAME FPSTR(stringName)
// #define BUZZER_NAME FPSTR(buzzerName)
// #define DFPLAYER_NAME FPSTR(dfplayerName)

#define ANALOG_NAME F("nlg")
#define BUTTON_NAME F("btn")
#define RELAY_NAME F("rly")
#define STRING_NAME F("str")
#define BUZZER_NAME F("buz")
#define DFPLAYER_NAME F("dfp")

//CONFIGURATION
static const char ATTR_BASETOPIC[] = "basetpc";
static const char ATTR_MQTTPASS[] = "mqttpwd";
static const char ATTR_MQTTSERV[] = "mqttsrv";
static const char ATTR_MQTTUSER[] = "mqttusr";
static const char ATTR_WIFIPASS[] = "wifipwd";
static const char ATTR_WIFISSID[] = "wifissi";
static const char ATTR_CONFVER[] = "confver";
static const char ATTR_ONLINE[] = "on_line";
static const char ATTR_SERVICEMODE[] = "service";
static const char ATTR_MQTTPORT[] = "mqttprt";

// static const char configAttribute_baseTopic[] PROGMEM = "basetpc";
// #define ATTR_BASETOPIC FPSTR(configAttribute_baseTopic)
// static const char configAttribute_mqttPassword[] PROGMEM = "mqttpwd";
// #define ATTR_MQTTPASS FPSTR(configAttribute_mqttPassword)
// static const char configAttribute_mqttServer[] PROGMEM = "mqttsrv";
// #define ATTR_MQTTSERV FPSTR(configAttribute_mqttServer)
// static const char configAttribute_mqttUser[] PROGMEM = "mqttusr";
// #define ATTR_MQTTUSER FPSTR(configAttribute_mqttUser)
// static const char configAttribute_wifiPassword[] PROGMEM = "wifipwd";
// #define ATTR_WIFIPASS FPSTR(configAttribute_wifiPassword)
// static const char configAttribute_wifiSsid[] PROGMEM = "wifissi";
// #define ATTR_WIFISSID FPSTR(configAttribute_wifiSsid)
// static const char configAttribute_configVersion[] PROGMEM = "confver";
// #define ATTR_CONFVER FPSTR(configAttribute_configVersion)
// static const char configAttribute_isOnline[] PROGMEM = "on-line";
// #define ATTR_ONLINE FPSTR(configAttribute_isOnline)
// static const char configAttribute_isServiceMode[] PROGMEM = "service";
// #define ATTR_SERVICEMODE FPSTR(configAttribute_isServiceMode)
// static const char configAttribute_mqttPort[] PROGMEM = "mqttprt";
// #define ATTR_MQTTPORT FPSTR(configAttribute_mqttPort)

#endif