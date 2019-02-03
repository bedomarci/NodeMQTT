#ifndef LOGINTERFACE_H
#define LOGINTERFACE_H
#include "StringInterface.hpp"

class LogInterface : public StringInterface
{
  public:
    LogInterface();
    void init();
    void valueToString(String &sValue);

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
    setSamplingEnabled(true);
    setMQTTPublish(true);
    setMQTTSubscribe(false);
    setSamplingRate(LOG_SAMPLE_RATE);
}

inline void LogInterface::init()
{
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

inline void LogInterface::valueToString(String &sValue)
{
}

#endif //LOGINTERFACE_H