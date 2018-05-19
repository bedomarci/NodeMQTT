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
#define SERVICE_MODE
#endif //define DEBUG

#ifdef DEBUG_NETWORK
#define D_NET(x) d(x)
#else
#define D_NET(x)
#endif
#ifdef DEBUG_MQTT
#define D_MQTT(x) d(x)
#else
#define D_MQTT(x)
#endif
#ifdef DEBUG_CONFIG
#define D_CONF(x) d(x)
#else
#define D_CONF(x)
#endif
#ifdef DEBUG_INTERFACE
#define D_INTR(x) d(x)
#else
#define D_INTR(x)
#endif

#define DEVICE_NAME String(ESP.getChipId(), HEX)
// inline String getMQTTDeviceName()
// {
//     return ;
// }

#endif