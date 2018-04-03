#include "NodeMQTTLogger.hpp"
#include "interfaces/StringInterface.hpp"

NodeMQTTLoggerClass::NodeMQTTLoggerClass()
{
    logQueue = LinkedList<String>();
}
void NodeMQTTLoggerClass::push(String message)
{
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
    Serial.println(message);
    //TODO convert to string and push to log queue.
}

void NodeMQTTLoggerClass::log(String &message, LOG_LEVEL level)
{
    char buffer[LOG_MAX_MESSAGE_LENGTH];
    sprintf(buffer, "[%c|%d] %s", level, millis(), message.c_str());
    String formattedMessage = String(buffer);
    Serial.println(formattedMessage);
    push(formattedMessage);
}

// StringInterface *NodeMQTTLoggerClass::getLoggerInterface()
// {
//     return loggerInterface;
// }

NodeMQTTLoggerClass Logger;