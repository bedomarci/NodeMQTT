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
static const char configTopic[] PROGMEM = "config";
#define CONFIG_TOPIC FPSTR(configTopic)
static const char heartbeatTopic[] PROGMEM = "heartbeat";
#define HEARTBEAT_TOPIC FPSTR(heartbeatTopic)
static const char terminalHR[] PROGMEM = "\t============================\t";
#define TERMINAL_HR FPSTR(terminalHR)

//INTERFACE NAMES
#define ANALOG_NAME F("an")
#define BUTTON_NAME F("btn")
#define RELAY_NAME F("rly")
#define STRING_NAME F("str")
#define BUZZER_NAME F("buz")

//CONFIGURATION
static const char configAttribute_baseTopic[] PROGMEM = "basetpc";
#define ATTR_BASETOPIC FPSTR(configAttribute_baseTopic)
static const char configAttribute_mqttPassword[] PROGMEM = "mqttpwd";
#define ATTR_MQTTPASS FPSTR(configAttribute_mqttPassword)
static const char configAttribute_mqttServer[] PROGMEM = "mqttsrv";
#define ATTR_MQTTSERV FPSTR(configAttribute_mqttServer)
static const char configAttribute_mqttUser[] PROGMEM = "mqttusr";
#define ATTR_MQTTUSER FPSTR(configAttribute_mqttUser)
static const char configAttribute_wifiPassword[] PROGMEM = "wifipwd";
#define ATTR_WIFIPASS FPSTR(configAttribute_wifiPassword)
static const char configAttribute_wifiSsid[] PROGMEM = "wifissi";
#define ATTR_WIFISSID FPSTR(configAttribute_wifiSsid)
static const char configAttribute_configVersion[] PROGMEM = "confver";
#define ATTR_CONFVER FPSTR(configAttribute_configVersion)
static const char configAttribute_isOnline[] PROGMEM = "on-line";
#define ATTR_ONLINE FPSTR(configAttribute_isOnline)
static const char configAttribute_isServiceMode[] PROGMEM = "service";
#define ATTR_SERVICEMODE FPSTR(configAttribute_isServiceMode)
static const char configAttribute_mqttPort[] PROGMEM = "mqttprt";
#define ATTR_MQTTPORT FPSTR(configAttribute_mqttPort)

#endif