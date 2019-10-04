#include "NodeMQTTLogger.hpp"
#include "NodeMQTTIOContainer.hpp"
#include "misc/typedef.hpp"
#include <stdio.h>

NodeMQTTLoggerClass::NodeMQTTLoggerClass()
{
    logQueue = LinkedList<String>();
}


void NodeMQTTLoggerClass::init(ApplicationContext * context) {
    _context = context;
}

void NodeMQTTLoggerClass::setLogging(bool isLogging)
{
    _isLogging = isLogging;
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

void NodeMQTTLoggerClass::log(LOG_LEVEL level, String message)
{
    log(level, message.c_str());
}
void NodeMQTTLoggerClass::logf(LOG_LEVEL level, String message, ...)
{
    va_list arg;
    va_start(arg, message);
    logf(level, message.c_str(), arg);
}

void NodeMQTTLoggerClass::log(LOG_LEVEL level, int value, uint8_t base)
{
    char buffer[24];
    switch (base)
    {
    case OCT:
        sprintf(buffer, "%o", value);
        break;
    case HEX:
        sprintf(buffer, "%x", value);
        break;
    case DEC:
    default:
        sprintf(buffer, "%d", value);
        break;
    }
    log(level, buffer);
}

void NodeMQTTLoggerClass::log(LOG_LEVEL level, const __FlashStringHelper *message)
{
    String msg = String(message);
    log(level, msg.c_str());
}
void NodeMQTTLoggerClass::logf(LOG_LEVEL level, const __FlashStringHelper *message, ...)
{
    va_list arg;
    va_start(arg, message);
    String msg = String(message);
    logf(level, msg.c_str(), arg);
}
void NodeMQTTLoggerClass::logf(LOG_LEVEL level, const char *message, ...)
{
    va_list arg;
    va_start(arg, message);
    logf(level, message, arg);
}

void NodeMQTTLoggerClass::logf(LOG_LEVEL level, const char *message, va_list arg)
{
    char temp[LOG_MAX_MESSAGE_LENGTH];
    char *buffer = temp;
    vsnprintf(temp, sizeof(temp), message, arg);
    va_end(arg);
    log(level, buffer);
    if (buffer != temp)
    {
        delete[] buffer;
    }
}

void NodeMQTTLoggerClass::log(LOG_LEVEL level, const char *message)
{
    char buffer[LOG_MAX_MESSAGE_LENGTH];

    if (level == FATAL)
        onFatal();
    if (_context && !_context->currentTime) {
        sprintf(buffer, LOG_FORMAT_MILLIS, level, millis(), String(message).substring(0,LOG_MAX_PRINT_LENGTH).c_str());
    } else {
        sprintf(buffer, LOG_FORMAT_NTP, level, toTimeString(_context->currentTime).c_str(), String(message).substring(0,LOG_MAX_PRINT_LENGTH).c_str()); //-3 will make room for dots
    }
    String formattedMessage = String(buffer);
    if ((level != DEBUG && !_isLogging) || _isLogging) {
    if (formattedMessage.length() == LOG_MAX_MESSAGE_LENGTH-1) {
        formattedMessage = formattedMessage.substring(0, LOG_MAX_MESSAGE_LENGTH-4) + "...";
    } 
    NodeMQTTIO.println(formattedMessage);
    }
    if (level != DEBUG && _isLogging)
        push(formattedMessage); 
}
void NodeMQTTLoggerClass::setFatalCallback(NodeMQTTCallback callback)
{
    fatalCallback = callback;
}

void NodeMQTTLoggerClass::onFatal()
{
    if (fatalCallback != nullptr)
        fatalCallback();
}

NodeMQTTLoggerClass Logger;