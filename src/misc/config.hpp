//CONFIGURATION FILE
#ifndef CONFIG_H
#define CONFIG_H

#define DEBUG_ALL

// #include "helpers.hpp"

#define MQTT_MAX_PACKET_SIZE 140 //140
#define SERIAL_BUFFER_SIZE 512

#define MQTT_CONNECT_ATTEMPT_WAITING 60000L
#define SERIAL_READ_INTERVAL 2000L

#define LOG_MAX_MESSAGE_LENGTH 256
#define LOG_MAX_QUEUE_LENGTH 50

// if connection is established, and then lost for some reason,
// ESP will automatically reconnect to last used access point
// once it is again back on-line. This will be done automatically
// by Wi-Fi library, without any user intervention. Reconnection triggered after 60s of being offline.
#define WIFI_CONNECT_ATTEMPT_WAITING 60000L
#define DEFAULT_SAMPLE_RATE 100L
#define DEFAULT_HEARTBEAT_RATE 9500L

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
#define DEFAULT_CONFIGURATION_VERSION 9
#define DEFAULT_ISONLINE true
#define DEFAULT_ISSERVICEMODE true

//FIRMWARE OTA UPDATE

#define FIRMWARE_URL_BASE "http://192.168.0.1/fota/"
#ifndef FIRMWARE_VERSION
    #define FIRMWARE_VERSION -1
#endif


#endif //define CONFIG_H