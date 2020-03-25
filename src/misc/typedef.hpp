#ifndef TYPEDEF_H
#define TYPEDEF_H

#include "config.hpp"
#include "typedefDeclaration.hpp"
#include <functional>
//#include "../constants.hpp"
#include <LinkedList.h>
//#include <TaskScheduler.h>

extern "C" {
#include <ccronexpr.h>
}


class Scheduler;

struct ApplicationContext;

//#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
//typedef std::function<void(char *, uint8_t *, unsigned int)> NodeMQTTMessageCallback;
//typedef std::function<void()> NodeMQTTCallback;
//typedef std::function<void(const char *)> NodeMQTTStringCallback;
//#define NodeMQTTChangeCallback std::function<void(T, T)>
//#else
//typedef void (*NodeMQTTMessageCallback)(char *, uint8_t *, unsigned int);
//typedef void (*NodeMQTTCallback)();
//typedef void (*NodeMQTTStringCallback)(const char *);
//#define NodeMQTTChangeCallback void (*)(T, T)
//#endif

struct NodeMQTTCronJob {
    const char *cronString;
    NodeMQTTCallback cb;
    cron_expr cronExpression;
    bool enabled;
    time_t nextExecution;
};

struct NodeMQTTScheduledTask {
    NodeMQTTCallback cb;
    time_t nextExecution;
};

struct NodeMQTTTime {
    uint8_t Second;
    uint8_t Minute;
    uint8_t Hour;
    uint8_t Wday;   // day of week, sunday is day 1
    uint8_t Day;
    uint8_t Month;
    uint8_t Year;   // offset from 1970;
};

class NodeInterfaceBase {
public:
    virtual void setContext(ApplicationContext *context) = 0;

    virtual void writeRaw(String value, bool publishable = true) = 0;
    virtual void handle() = 0;
    virtual void setSamplingRate(unsigned long samplingRate) = 0;
    virtual void init() = 0;
    virtual void setEnabled(bool enabled) = 0;
    virtual bool isEnabled() = 0;
    virtual String valueToString() = 0;

    virtual bool hasMQTTPublish() = 0;
    virtual bool hasMQTTSubscribe() = 0;
    virtual bool hasInitializedValue() = 0;
    virtual void setMQTTPublish(bool publish) = 0;
    virtual void setMQTTSubscribe(bool subscribe) = 0;
    virtual void setBaseTopic(String baseTopic) = 0;
    virtual String getBaseTopic() = 0;
    virtual String getSubscribeTopic() = 0;
    virtual String getPublishTopic() = 0;

    virtual void republish() = 0;
};

#include "../transports/_AbstractTransport.hpp"
#include "../parsers/_AbstractParser.hpp"

struct ApplicationContext
{
    Scheduler *scheduler;
    LinkedList<NodeInterfaceBase *> *interfaces;
    time_t currentTime = 0;

    AbstractTransport *transport;
    AbstractParser *parser;
};


#endif //TYPEDEF_H