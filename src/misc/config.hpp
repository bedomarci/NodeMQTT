//CONFIGURATION FILE
#ifndef CONFIG_H
#define CONFIG_H

//TASK SCHEDULER CONFIGURATION

#ifdef _TASKSCHEDULER_H_
#error Must include NodeMQTT.hpp before TaskScheduler.h!
#include <stophere>
#endif
#define _TASK_STD_FUNCTION
// #define _TASK_MICRO_RES
// #include <TaskScheduler.h>

#define NODEMQTT_SERIAL_SPEED 115200
#define NODEMQTT_MAX_PACKET_SIZE 1024 //140
#define MQTT_MAX_PACKET_SIZE NODEMQTT_MAX_PACKET_SIZE
#define SERIAL_BUFFER_SIZE 1024
#define JSON_DOCUMENT_SIZE 2048
#define PWM_FREQ 5000
#define PWM_RESOLUTION 10

#define MQTT_CONNECT_ATTEMPT_WAITING 15 * TASK_SECOND //
#define MQTT_SOCKET_TIMEOUT 1                         // MQTT_SOCKET_TIMEOUT: socket timeout interval in Seconds
#define SERIAL_READ_INTERVAL 2 * TASK_SECOND

#define LOG_MAX_MESSAGE_LENGTH 128
#define LOG_MAX_QUEUE_LENGTH 30

// if connection is established, and then lost for some reason,
// ESP will automatically reconnect to last used access point
// once it is again back on-line. This will be done automatically
// by Wi-Fi library, without any user intervention. Reconnection triggered after 60s of being offline.
#define WIFI_CONNECT_ATTEMPT_WAITING 5 * TASK_SECOND //60000L
#define DEFAULT_SAMPLE_RATE (TASK_SECOND / 10)
#define DEFAULT_FADE_INTERVAL (TASK_SECOND / 25)
#define DEFAULT_BUTTON_DEBOUNCE 50
#define DEFAULT_EXPANDER_DEBOUNCE 25
#define DEFAULT_HEARTBEAT_RATE 10 * TASK_SECOND
#define LOG_SAMPLE_RATE TASK_SECOND / 5

#if !defined(WIFI_TRANSPORT) //&& !defined(WIFI_TRANSPORT)
#define WIFI_TRANSPORT
#endif

#ifdef RF_TRANSPORT
#define TRANSPORT_CLASS RF24Transport
#endif
#ifdef WIFI_TRANSPORT
#define TRANSPORT_CLASS WifiTransport
#endif

#if !defined(PUBSUB_PARSER)
#define PUBSUB_PARSER
#endif

#ifdef PUBSUB_PARSER
#define PARSER_CLASS PubSubParser
#endif

//RF24 CONFIGURATION
#ifndef RF24_CE_PIN
#define RF24_CE_PIN 7
#endif
#ifndef RF24_CS_PIN
#define RF24_CS_PIN 8
#endif
#define RF24_MAX_MESSAGE_SIZE 512
#define RF24_MESSAGE_DELIMITER "|||"

//I2C CONFIGURATION
#define I2C_CLOCK_SPEED 100000L

#ifndef DEFAULT_WIFI_SSID
#define DEFAULT_WIFI_SSID "nodenetwork"
#endif

#ifndef DEFAULT_WIFI_BSSID
#define DEFAULT_WIFI_BSSID \
    {                      \
        0, 0, 0, 0, 0, 0   \
    }
#endif

#ifndef DEFAULT_IP_ADDRESS
#define DEFAULT_IP_ADDRESS \
    {                      \
        0, 0, 0, 0         \
    }
#endif

#ifndef DEFAULT_GATEWAY
#define DEFAULT_GATEWAY \
    {                   \
        192, 168, 0, 1  \
    }
#endif

#ifndef DEFAULT_SUBNET
#define DEFAULT_SUBNET   \
    {                    \
        255, 255, 255, 0 \
    }
#endif

#ifndef DEFAULT_WIFI_CHANNEL
#define DEFAULT_WIFI_CHANNEL 0
#endif

#ifndef DEFAULT_WIFI_PASSWORD
#define DEFAULT_WIFI_PASSWORD "thereisnospoon"
#endif

#ifndef DEFAULT_MQTT_SERVER
#define DEFAULT_MQTT_SERVER "broker.orangepixel.hu"
#endif

#ifndef DEFAULT_MQTT_PORT
#define DEFAULT_MQTT_PORT 1883
#endif

#ifndef DEFAULT_MQTT_USER
#define DEFAULT_MQTT_USER ""
#endif

#ifndef DEFAULT_MQTT_PASSWORD
#define DEFAULT_MQTT_PASSWORD ""
#endif

#ifndef DEFAULT_MQTT_BASE_TOPIC
#define DEFAULT_MQTT_BASE_TOPIC "unnamed_device"
#endif

#ifndef DEFAULT_CONFIGURATION_VERSION
#define DEFAULT_CONFIGURATION_VERSION 11
#endif

#ifndef DEFAULT_ISONLINE
#define DEFAULT_ISONLINE true
#endif

#ifndef DEFAULT_ISSERVICEMODE
#define DEFAULT_ISSERVICEMODE true
#endif

#ifndef DEFAULT_ISLOGGING
#define DEFAULT_ISLOGGING true
#endif

//FIRMWARE OTA UPDATE

#define FIRMWARE_URL_BASE "http://192.168.0.1/fota/"
#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION -1
#endif

#endif //define CONFIG_H