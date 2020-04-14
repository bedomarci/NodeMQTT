//CONFIGURATION FILE
#ifndef CONFIG_H
#define CONFIG_H

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#define TELNET_SERVER_CLASS WiFiServer
#define TELNET_CLIENT_CLASS WiFiClient
#endif
#if defined(ARDUINO_ARCH_ESP32)

#include <WiFi.h>
#endif

//TASK SCHEDULER CONFIGURATION
#ifdef _TASKSCHEDULER_H_
#error Must include NodeMQTT.hpp before TaskScheduler.h!
#include <stophere>
#endif
#if (defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)) && !defined(_TASK_STD_FUNCTION)
#define _TASK_STD_FUNCTION

#include <functional>

#endif

#ifndef NODEMQTT_SERIAL_SPEED
#define NODEMQTT_SERIAL_SPEED 115200
#endif

#ifndef SERIAL_BUFFER_SIZE
#define SERIAL_BUFFER_SIZE 512
#endif

#ifndef TELNET_BUFFER_SIZE
#define TELNET_BUFFER_SIZE 256
#endif

#define JSON_DOCUMENT_SIZE 2048
#define PWM_FREQ 5000
#define PWM_RESOLUTION 10

#define MQTT_CONNECT_ATTEMPT_WAITING 15 * TASK_SECOND //

#define LOG_MAX_MESSAGE_LENGTH 128
#define LOG_PREFIX_LENGTH 18
#define LOG_MAX_PRINT_LENGTH (LOG_MAX_MESSAGE_LENGTH - LOG_PREFIX_LENGTH)

#define LOG_MAX_QUEUE_LENGTH 30

// if connection is established, and then lost for some reason,
// ESP will automatically reconnect to last used access point
// once it is again back on-line. This will be done automatically
// by Wi-Fi library, without any user intervention. Reconnection triggered after 60s of being offline.
#define WIFI_CONNECT_ATTEMPT_WAITING 10 * TASK_SECOND //60000L
#define WIFI_TRANSMISSION_POWER 20.5f //min: 0 dB, max: 20.5 dB
#define WIFI_PYH_MODE WIFI_PHY_MODE_11N
#define TELNET_SERVER_PORT 23
#define EEPROM_SIZE 512
#define EEPROM_MAX_WORD_LENGTH 64

#define DEFAULT_SAMPLE_RATE (TASK_SECOND / 10)
#define DEFAULT_FADE_INTERVAL (TASK_SECOND / 25)
#define DEFAULT_BUTTON_DEBOUNCE 50
#define DEFAULT_EXPANDER_DEBOUNCE 25
#define DEFAULT_HEARTBEAT_RATE 10 * TASK_SECOND
#define LOG_SAMPLE_RATE TASK_SECOND / 5

#define NULL_TRANSPORT 0
#define WIFI_TRANSPORT 1
#define RF_TRANSPORT 2


#if !defined(NODEMQTT_TRANSPORT) //&& !defined(WIFI_TRANSPORT)
#define NODEMQTT_TRANSPORT WIFI_TRANSPORT
#endif

#if (NODEMQTT_TRANSPORT == RF_TRANSPORT)
#define TRANSPORT_CLASS RF24Transport
#endif

#if (NODEMQTT_TRANSPORT == WIFI_TRANSPORT)
#define TRANSPORT_CLASS WifiTransport
#endif

#if (NODEMQTT_TRANSPORT == NULL_TRANSPORT)
#define TRANSPORT_CLASS NullTransport
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

//IO CONFIGURATION

#ifndef IO_SERIAL
#define IO_SERIAL true
#endif

#if (NODEMQTT_TRANSPORT == WIFI_TRANSPORT) && !defined(IO_TELNET)
#define IO_TELNET true
#endif

#if defined(ARDUINO_ARCH_ESP32) && !defined(IO_BLUETOOTH)
#define IO_BLUETOOTH true
#endif

#if !defined(PUBSUB_PARSER)
#define PUBSUB_PARSER
#endif

//PARSER CONFIGURATION

#ifdef PUBSUB_PARSER
#define PARSER_CLASS PubSubParser
#endif

//NTP TIME SERVER 
//time-1.timefreq.bldrdoc.gov
#define NTP_TIME_SERVER_ADDRESS "europe.pool.ntp.org"
#define NTP_TIME_SERVER_PORT 8888 // local port to listen for UDP packets
#define NTP_UPDATE_INTERVAL (TASK_MINUTE * 30)
#define NTP_DEFAULT_TIMEZONE 1




//I2C CONFIGURATION
#define I2C_CLOCK_SPEED 100000L

#ifndef DEFAULT_WIFI_SSID
#define DEFAULT_WIFI_SSID "nodenetwork"
#endif

#ifndef DEFAULT_WIFI_BSSID
#define DEFAULT_WIFI_BSSID  "00:00:00:00:00:00" //{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#endif

#ifndef DEFAULT_IP_ADDRESS
#define DEFAULT_IP_ADDRESS  "0.0.0.0"
#endif

#ifndef DEFAULT_GATEWAY
#define DEFAULT_GATEWAY     "192.168.0.1" //{192, 168, 0, 1}
#endif

#ifndef DEFAULT_SUBNET
#define DEFAULT_SUBNET      "255.255.255.0" //{255, 255, 255, 0}
#endif

#ifndef DEFAULT_DNS
#define DEFAULT_DNS      "1.1.1.1" //{255, 255, 255, 0}
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

#define DEFAULT_FIRMWARE_URL "http://192.168.0.1/fota/"

#endif //define CONFIG_H