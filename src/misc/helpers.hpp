#ifndef HELPERS_H
#define HELPERS_H
#include <Arduino.h>
#include "NodeMQTTLogger.hpp"

#define d(x) Logger.log(x, DEBUG)
#define debug(x) Logger.log(x, DEBUG)

#define i(x) Logger.log(x, INFO)
#define info(x) Logger.log(x, INFO)

#define w(x) Logger.log(x, WARNING)
#define warning(x) Logger.log(x, WARNING)

#define e(x) Logger.log(x, ERROR)
#define error(x) Logger.log(x, ERROR)

#ifdef DEBUG_ALL
#define DEBUG_NETWORK
#define DEBUG_MQTT
#define DEBUG_CONFIG
#define DEBUG_INTERFACE
#define DEBUG_OTHER
#define SERVICE_MODE
#endif //define DEBUG

#ifdef DEBUG_NETWORK
#define PNET(x) d(x)
#else
#define PNET(x)
#endif
#ifdef DEBUG_MQTT
#define PMQTT(x) d(x)
#else
#define PMQTT(x)
#endif
#ifdef DEBUG_CONFIG
#define PCON(x) d(x)
#else
#define PCON(x)
#endif
#ifdef DEBUG_INTERFACE
#define PINT(x) d(x)
#else
#define PINT(x)
#endif
#ifdef DEBUG_OTHER
#define P(x) d(x)
#else
#define P(x)
#endif

// String _MQTTDeviceName = String("ESP-" + String((int)ESP.getChipId(), HEX));
inline const char *getMQTTDeviceName()
{
    // return _MQTTDeviceName.c_str();
    return String((int)ESP.getChipId(), HEX).c_str();
}

#endif