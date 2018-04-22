#include "NodeMQTTLogger.hpp"

NodeMQTTLoggerClass::NodeMQTTLoggerClass()
{
    logQueue = LinkedList<String>();
}
void NodeMQTTLoggerClass::push(String message)
{
    if (logQueue.size() >= _queueLength)
        pop();
    logQueue.add(message);
}
String NodeMQTTLoggerClass::pop()
{
    return logQueue.shift();
}

int NodeMQTTLoggerClass::queueSize()
{
    return logQueue.size();
}

void NodeMQTTLoggerClass::log(const char *message, LOG_LEVEL level)
{
    String msg = String(message);
    log(msg, level);
}

void NodeMQTTLoggerClass::log(int message, LOG_LEVEL level)
{
    String msg = String(message);
    log(msg, level);
}

void NodeMQTTLoggerClass::log(const __FlashStringHelper *message, LOG_LEVEL level)
{
    String msg = String(message);
    log(msg, level);
}

void NodeMQTTLoggerClass::log(String message, LOG_LEVEL level)
{
    char buffer[LOG_MAX_MESSAGE_LENGTH];
    sprintf(buffer, "[%c|%d] %s", level, millis(), message.c_str());
    String formattedMessage = String(buffer);
    Serial.println(formattedMessage);
    if (level != DEBUG)
        push(formattedMessage);
}

NodeMQTTLoggerClass Logger;