//CONFIGURATION FILE
#ifndef CONFIG_H
#define CONFIG_H

#include "helpers.hpp"

#define DEBUG_ALL

#ifdef DEBUG_ALL
    #define DEBUG_NETWORK
    #define DEBUG_MQTT
    #define DEBUG_CONFIG
    #define DEBUG_INTERFACE
    #define DEBUG_OTHER
    #define SERVICE_MODE
#endif //define DEBUG






#define MQTT_CONNECT_ATTEMPT_WAITING 60000L
#define SERIAL_READ_INTERVAL 2000L

// if connection is established, and then lost for some reason,
// ESP will automatically reconnect to last used access point
// once it is again back on-line. This will be done automatically
// by Wi-Fi library, without any user intervention. Reconnection triggered after 60s of being offline.
#define WIFI_CONNECT_ATTEMPT_WAITING 60000L
#define DEFAULT_SAMPLE_RATE 100L
#define DEFAULT_HEARTBEAT_RATE 10000L
//#define JSON_BUFFER_SIZE 400
#define SERIAL_BUFFER_SIZE 512

//TASK SCHEDULER CONFIGURATION
#define _TASK_STD_FUNCTION
#define _TASK_SLEEP_ON_IDLE_RUN

#define DEFAULT_WIFI_SSID "nodenetwork"
#define DEFAULT_WIFI_PASSWORD "thereisnospoon"
#define DEFAULT_MQTT_SERVER "server"
#define DEFAULT_MQTT_PORT 1883
#define DEFAULT_MQTT_USER ""
#define DEFAULT_MQTT_PASSWORD ""
#define DEFAULT_MQTT_BASE_TOPIC "unnamed_device"
#define DEFAULT_CONFIGURATION_VERSION 8
#define DEFAULT_ISONLINE true
#define DEFAULT_ISSERVICEMODE true

//FIRMWARE OTA UPDATE

#define FIRMWARE_URL_BASE "http://192.168.0.1/fota/"
#ifndef FIRMWARE_VERSION
    #define FIRMWARE_VERSION -1
#endif


#ifdef DEBUG_NETWORK
    #define PNET(x)     d(x)
#else
    #define PNET(x)
#endif
#ifdef DEBUG_MQTT
    #define PMQTT(x)    d(x)
#else
    #define PMQTT(x)
#endif
#ifdef DEBUG_CONFIG
    #define PCON(x)     d(x)
#else
    #define PCON(x)
#endif
#ifdef DEBUG_INTERFACE
    #define PINT(x)     d(x)
#else
    #define PINT(x)
#endif
#ifdef DEBUG_OTHER
    #define P(x)        d(x)
#else
    #define P(x)
#endif







#endif //define CONFIG_H