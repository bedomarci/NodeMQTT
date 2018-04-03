#ifndef NODELOGGER_H
#define NODELOGGER_H
#include <Arduino.h>
#include <LinkedList.h>
#include "misc/config.hpp"
// #include "interfaces/StringInterface.hpp"

enum LOG_LEVEL
{
    DEBUG = 'D',
    INFO = 'I',
    WARNING = 'W',
    ERROR = 'E'
};

class NodeMQTTLoggerClass
{

  public:
    NodeMQTTLoggerClass();
    void log(const char *message, LOG_LEVEL level);
    void log(int message, LOG_LEVEL level);
    void log(const __FlashStringHelper *message, LOG_LEVEL level);
    void log(String &message, LOG_LEVEL level);
    int queueSize();
    String pop();
    // StringInterface * getLoggerInterface();

  protected:
    // StringInterface * loggerInterface = new StringInterface("log");
    LinkedList<String> logQueue;
    void push(String);
};

extern NodeMQTTLoggerClass Logger;

#endif