#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <Arduino.h>
#include "misc/pitches.hpp"
#include "misc/time/compile_time.hpp"

//PLATFORM SPECIFIC INCLUDES

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#define SERVER_CLASS WiFiServer
#define CLIENT_CLASS WiFiClient
#define UPDATE ESPhttpUpdate
#endif
#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <esp_wifi.h>
#define SERVER_CLASS WiFiServer
#define CLIENT_CLASS WiFiClient
#define UPDATE httpUpdate
#endif

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
#include <ESP.h>
#include <WiFiUdp.h>
#define UDP WiFiUDP
//typedef std::function<void(char *, uint8_t *, unsigned int)> NodeMQTTMessageCallback;
//typedef std::function<void()> NodeMQTTCallback;
//typedef std::function<void(const char *)> NodeMQTTStringCallback;
//#define NodeMQTTChangeCallback std::function<void(T, T)>
#else
//typedef void (*NodeMQTTMessageCallback)(char *, uint8_t *, unsigned int);
//typedef void (*NodeMQTTCallback)();
//typedef void (*NodeMQTTStringCallback)(const char *);
//#define NodeMQTTChangeCallback void (*)(T, T)
#define FPSTR(x) x
#define F(x) x
#endif


#if defined(ARDUINO_ARCH_ESP32)
#define PWMRANGE 1023
#endif

#if defined(ARDUINO_ARCH_ESP8266)
#endif

template <typename T, unsigned int LENGTH>
struct Array
{
    uint16_t length = LENGTH;
    T item[LENGTH];

    //Array subscript operator
    inline T &operator[](uint16_t idx) { return item[idx]; }
    inline const T &operator[](uint16_t idx) const { return item[idx]; }
};

struct Note
{
    uint16_t duration;
    uint16_t frequency;
    uint16_t pause;
};


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


extern const Note infoTone[1] PROGMEM;
extern const Note successTone[4] PROGMEM;
extern const Note warningTone[2] PROGMEM;
extern const Note errorTone[6] PROGMEM;
extern const Note alertTone[5] PROGMEM;
extern const Note failTone[4] PROGMEM;
extern const Note happyTone[5] PROGMEM;
extern const Note sadTone[5] PROGMEM;
extern const Note systemBoot[] PROGMEM;
extern const Note systemOnline[] PROGMEM;

//NODES
extern const char offlineMessage[] PROGMEM;
#define OFFLINE_MESSAGE FPSTR(offlineMessage)
extern const char configPubTopic[] PROGMEM;
#define CONFIG_PUB_TOPIC FPSTR(configPubTopic)
extern const char configSubTopic[] PROGMEM;
#define CONFIG_SUB_TOPIC FPSTR(configSubTopic)
extern const char logTopic[] PROGMEM;
#define LOG_TOPIC FPSTR(logTopic)
extern const char heartbeatTopic[] PROGMEM;
#define HEARTBEAT_TOPIC FPSTR(heartbeatTopic)
extern const char languageTopic[] PROGMEM;
#define LANGUAGE_TOPIC FPSTR(languageTopic)
extern const char commandTopic[] PROGMEM;
#define COMMAND_TOPIC FPSTR(commandTopic)
extern const char fsmPubTopic[] PROGMEM;
#define FSM_PUB_TOPIC FPSTR(fsmPubTopic)
extern const char fsmSubTopic[] PROGMEM;
#define FSM_SUB_TOPIC FPSTR(fsmSubTopic)
extern const char terminalTab[] PROGMEM;
#define TERMINAL_TAB FPSTR(terminalTab)

//SYSTEM
extern uint32_t UUID;
extern const char terminalHR[] PROGMEM;
#define TERMINAL_HR FPSTR(terminalHR)
// #define TERMINAL_HR terminalHR
extern const char terminalPrompt[] PROGMEM;
#define TERMINAL_PROMPT FPSTR(terminalPrompt)
extern const char terminalHeader[] PROGMEM;
#define NODEMQTT_TERMINAL_HEADER FPSTR(terminalHeader)

// INTERFACE NAMES
extern const char analogName[] PROGMEM;
#define ANALOG_NAME FPSTR(analogName)

extern const char buttonName[] PROGMEM;
#define BUTTON_NAME FPSTR(buttonName)

extern const char buttonArrayName[] PROGMEM;
#define BUTTON_ARRAY_NAME FPSTR(buttonArrayName)

extern const char relayName[] PROGMEM;
#define RELAY_NAME FPSTR(relayName)

extern const char stringName[] PROGMEM;
#define STRING_NAME FPSTR(stringName)

extern const char buzzerName[] PROGMEM;
#define BUZZER_NAME FPSTR(buzzerName)

extern const char dfplayerName[] PROGMEM;
#define DFPLAYER_NAME FPSTR(dfplayerName)

extern const char pwmName[] PROGMEM;
#define PWM_NAME FPSTR(pwmName)

extern const char arrayInterfaceName[] PROGMEM;
#define ARRAY_NAME FPSTR(arrayInterfaceName)

extern const char dataInterfaceName[] PROGMEM;
#define DATA_NAME FPSTR(dataInterfaceName)

extern const char languageInterfaceName[] PROGMEM;
#define LANGUAGE_NAME FPSTR(languageInterfaceName)

extern const char fsmInterfaceName[] PROGMEM;
#define FSM_NAME FPSTR(fsmInterfaceName)

extern const char PCF8574InterfaceName[] PROGMEM;
#define PCF8574_NAME FPSTR(PCF8574InterfaceName)

extern const char commandInterfaceName[] PROGMEM;
#define COMMAND_NAME FPSTR(commandInterfaceName)

extern const char configInterfaceName[] PROGMEM;
#define CONFIG_NAME FPSTR(configInterfaceName)

extern const char interfaceName[] PROGMEM;
#define INTERFACE_NAME FPSTR(interfaceName)

extern const char configAttribute_baseTopic[] PROGMEM;
#define ATTR_BASETOPIC FPSTR(configAttribute_baseTopic)
extern const char configAttribute_mqttPassword[] PROGMEM;
#define ATTR_MQTTPASS FPSTR(configAttribute_mqttPassword)
extern const char configAttribute_mqttServer[] PROGMEM;
#define ATTR_MQTTSERV FPSTR(configAttribute_mqttServer)
extern const char configAttribute_mqttUser[] PROGMEM;
#define ATTR_MQTTUSER FPSTR(configAttribute_mqttUser)
extern const char configAttribute_wifiPassword[] PROGMEM;
#define ATTR_WIFIPASS FPSTR(configAttribute_wifiPassword)
extern const char configAttribute_wifiSsid[] PROGMEM;
#define ATTR_WIFISSID FPSTR(configAttribute_wifiSsid)
extern const char configAttribute_wifiBssid[] PROGMEM;
#define ATTR_WIFIBSSID FPSTR(configAttribute_wifiBssid)
extern const char configAttribute_ipAddress[] PROGMEM;
#define ATTR_IPADDRESS FPSTR(configAttribute_ipAddress)
extern const char configAttribute_gateway[] PROGMEM;
#define ATTR_GATEWAY FPSTR(configAttribute_gateway)
extern const char configAttribute_subnet[] PROGMEM;
#define ATTR_SUBNET FPSTR(configAttribute_subnet)
extern const char configAttribute_dns[] PROGMEM;
#define ATTR_DNS FPSTR(configAttribute_dns)
extern const char configAttribute_wifiChannel[] PROGMEM;
#define ATTR_WIFICHANNEL FPSTR(configAttribute_wifiChannel)
extern const char configAttribute_configVersion[] PROGMEM;
#define ATTR_CONFVER FPSTR(configAttribute_configVersion)
extern const char configAttribute_isOnline[] PROGMEM;
#define ATTR_ONLINE FPSTR(configAttribute_isOnline)
extern const char configAttribute_isServiceMode[] PROGMEM;
#define ATTR_SERVICEMODE FPSTR(configAttribute_isServiceMode)
extern const char configAttribute_isLogging[] PROGMEM;
#define ATTR_LOGGING FPSTR(configAttribute_isLogging)
extern const char configAttribute_mqttPort[] PROGMEM;
#define ATTR_MQTTPORT FPSTR(configAttribute_mqttPort)
extern const char configAttribute_timezone[] PROGMEM;
#define ATTR_TIMEZONE FPSTR(configAttribute_timezone)
extern const char configAttribute_firmwareUrl[] PROGMEM;
#define ATTR_FWURL FPSTR(configAttribute_firmwareUrl)



//MESSAGES
extern const char message_Introduction[] PROGMEM;
#define MSG_INTRODUCTION FPSTR(message_Introduction)
extern const char message_ValueChanged[] PROGMEM;
#define MSG_VALUE_CHANGED FPSTR(message_ValueChanged)
extern const char message_Publishing[] PROGMEM;
#define MSG_PUBLISHING FPSTR(message_Publishing)
extern const char message_FailedToParse[] PROGMEM;
#define MSG_FAILED_TO_PARSE FPSTR(message_FailedToParse)
extern const char message_UnknownState[] PROGMEM;
#define MSG_UNKNOWN_STATE FPSTR(message_UnknownState)
extern const char message_MQTTBrokerConnectionAttempt[] PROGMEM;
#define MSG_BROKER_CONNECTION_ATTEMPT FPSTR(message_MQTTBrokerConnectionAttempt)
extern const char message_MQTTBrokerCouldNotConnect[] PROGMEM;
#define MSG_BROKER_COULD_NOT_CONNECT FPSTR(message_MQTTBrokerCouldNotConnect)
extern const char message_MQTTBrokerDisconnected[] PROGMEM;
#define MSG_BROKER_DISCONNECTED FPSTR(message_MQTTBrokerDisconnected)
extern const char message_ContextIsMissing[] PROGMEM;
#define MSG_CONTEXT_IS_MISSING FPSTR(message_ContextIsMissing)
extern const char message_ConnetctToWifi[] PROGMEM;
#define MSG_CONNECT_TO_WIFI FPSTR(message_ConnetctToWifi)
extern const char message_ConnetctToWifiWithBSSID[] PROGMEM;
#define MSG_CONNECT_TO_WIFI_BSSID FPSTR(message_ConnetctToWifiWithBSSID)


//CONFIGURATION IDS
#define PROP_SYS_SERVICEMODE 1000
#define PROP_SYS_ONLINE 1001
#define PROP_SYS_LOGGING 1002
#define PROP_SYS_TIMEZONE 1003
#define PROP_SYS_FWURL 1004

#define PROP_WIFI_SSID 2000
#define PROP_WIFI_BSSID 2001
#define PROP_WIFI_PASSWORD 2002
#define PROP_WIFI_CHANNEL 2003
#define PROP_WIFI_IPADDRESS 2004
#define PROP_WIFI_SUBNET 2005
#define PROP_WIFI_GATEWAY 2006
#define PROP_WIFI_DNS 2007


#define PROP_MQTT_SERVER 3000
#define PROP_MQTT_PORT 3001
#define PROP_MQTT_USER 3002
#define PROP_MQTT_PASSWORD 3003
#define PROP_SYS_BASETOPIC 3004


extern const long FIRMWARE_BUILD_TIME PROGMEM;
extern const char LOG_FORMAT_NTP[] PROGMEM;
extern const char LOG_FORMAT_MILLIS[] PROGMEM;



#endif //CONSTANTS_H
