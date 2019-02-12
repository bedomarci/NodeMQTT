#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <Arduino.h>
#include "misc/pitches.hpp"

#if defined(ESP8266) || defined(ESP32)
#include <functional>
typedef std::function<void(char *, uint8_t *, unsigned int)> NodeMQTTMessageCallback;
typedef std::function<void()> NodeMQTTCallback;
#define NodeMQTTChangeCallback std::function<void(T, T)>
#else
typedef void (*NodeMQTTMessageCallback)(char *, uint8_t *, unsigned int);
typedef void (*NodeMQTTCallback)();
#define NodeMQTTChangeCallback void (*)(T, T)
#endif

#ifdef WIFI_TRANSPORT
#define TRANSPORT_CLASS WifiTransport
#endif
#ifdef RF_TRANSPORT
#define TRANSPORT_CLASS RF24Transport
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
extern const char configTopic[] PROGMEM;
#define CONFIG_TOPIC FPSTR(configTopic)
extern const char configEchoTopic[] PROGMEM;
#define CONFIG_TOPIC_ECHO FPSTR(configEchoTopic)
extern const char logTopic[] PROGMEM;
#define LOG_TOPIC FPSTR(logTopic)
extern const char heartbeatTopic[] PROGMEM;
#define HEARTBEAT_TOPIC FPSTR(heartbeatTopic)
extern const char terminalTab[] PROGMEM;
#define TERMINAL_TAB FPSTR(terminalTab)
extern const char terminalHR[] PROGMEM;
#define TERMINAL_HR FPSTR(terminalHR)
extern const char terminalHeader[] PROGMEM;
#define NODEMQTT_TERMINAL_HEADER FPSTR(terminalHeader)

// INTERFACE NAMES
extern const char analogName[] PROGMEM;
extern const char buttonName[] PROGMEM;
extern const char buttonArrayName[] PROGMEM;
extern const char relayName[] PROGMEM;
extern const char stringName[] PROGMEM;
extern const char buzzerName[] PROGMEM;
extern const char dfplayerName[] PROGMEM;
extern const char pwmName[] PROGMEM;
extern const char arrayInterfaceName[] PROGMEM;
extern const char dataInterfaceName[] PROGMEM;
extern const char interfaceName[] PROGMEM;
#define ANALOG_NAME FPSTR(analogName)
#define BUTTON_NAME FPSTR(buttonName)
#define BUTTON_ARRAY_NAME FPSTR(buttonArrayName)
#define RELAY_NAME FPSTR(relayName)
#define STRING_NAME FPSTR(stringName)
#define BUZZER_NAME FPSTR(buzzerName)
#define DFPLAYER_NAME FPSTR(dfplayerName)
#define PWM_NAME FPSTR(pwmName)
#define DATA_NAME FPSTR(dataInterfaceName)
#define ARRAY_NAME FPSTR(arrayInterfaceName)
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

//MESSAGES
extern const char message_ValueChanged[] PROGMEM;
#define MSG_VALUE_CHANGED FPSTR(message_ValueChanged)
extern const char message_Publishing[] PROGMEM;
#define MSG_PUBLISHING FPSTR(message_Publishing)
extern const char message_FailedToParse[] PROGMEM;
#define MSG_FAILED_TO_PARSE FPSTR(message_FailedToParse)
extern const char message_UnknownState[] PROGMEM;
#define MSG_UNKNOWN_STATE FPSTR(message_UnknownState)

#endif //CONSTANTS_H