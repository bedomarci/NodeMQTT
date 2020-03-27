#ifndef NODELOGGER_H
#define NODELOGGER_H

#include <Arduino.h>
#include "NodeMQTTConfigManager.hpp"
#include <LinkedList.h>
#include <stdio.h>
#include "misc/config.hpp"
#include "misc/NodeMQTTComponent.hpp"
#include "misc/typedefDeclaration.hpp"
#include "constants.hpp"

enum LOG_LEVEL
{
  DEBUG = 'D',
  INFO = 'I',
  WARNING = 'W',
  ERROR = 'E',
  FATAL = 'F',
};

class NodeMQTTLoggerClass : public NodeMQTTComponent
{

public:
  NodeMQTTLoggerClass();

    void init() override;

    void boot() override;
  void log(LOG_LEVEL level, const char *message);
  void logf(LOG_LEVEL level, const char *message, ...);
  void log(LOG_LEVEL level, const __FlashStringHelper *message);
  void logf(LOG_LEVEL level, const __FlashStringHelper *message, ...);
  void log(LOG_LEVEL level, String message);
  void logf(LOG_LEVEL level, String message, ...);
  void log(LOG_LEVEL level, int message, uint8_t base = DEC);
  void setLogging(bool isLogging);
  int queueSize();
  String pop();

protected:
  void logf(LOG_LEVEL level, const char *message, va_list arg);
  void push(String);
  LinkedList<String> logQueue;
  uint8_t _queueLength = LOG_MAX_QUEUE_LENGTH;
  bool _isLogging = true;
};

extern NodeMQTTLoggerClass Logger;

#endif