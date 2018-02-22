#ifndef NODELOGGER_H
#define NODELOGGER_H
#include "misc/config.hpp"
#include <Arduino.h>


enum LOG_LEVEL {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class NodeMQTTLoggerClass {

    public:
        void log (const char * message,                 LOG_LEVEL level);
        void log (int message,                          LOG_LEVEL level);
        void log (const __FlashStringHelper * message,  LOG_LEVEL level);
        void log (String& message,                      LOG_LEVEL level);
        
};

extern NodeMQTTLoggerClass Logger;

#endif