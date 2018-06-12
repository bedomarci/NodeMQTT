#ifndef LOGINTERFACE_H
#define LOGINTERFACE_H
#include "StringInterface.hpp"

class LogInterface : public StringInterface
{
  public:
    LogInterface();
    void init();

  protected:
    String logMsg = "";
    int cmp(String oldValue, String newValue);
    String sample();
    void logPublishCallback();
};

inline LogInterface::LogInterface()
    : StringInterface(LOG_TOPIC)
{
    _interfaceName = LOG_TOPIC;
}

inline void LogInterface::init()
{
    setSamplingEnabled(true);
    setMQTTPublish(true);
    setMQTTSubscribe(true);
}

inline String LogInterface::sample()
{
    logMsg = Logger.pop();
    if (Logger.queueSize())
    {
        forceResample();
    }
    return logMsg;
}

inline int LogInterface::cmp(String oldValue, String newValue)
{
    int value = (oldValue == nullptr) ? 0 : -1;
    return value;
}

#endif //LOGINTERFACE_H