#ifndef LOGINTERFACE_H
#define LOGINTERFACE_H
#include "StringInterface.hpp"

class LogInterface : public StringInterface
{
  public:
    LogInterface();
    void init();
    String valueToString() override;

  protected:
    String logMsg = "";
    int cmp(String oldValue, String newValue);
    String sample();
    void logPublishCallback();
};

inline LogInterface::LogInterface()
    : StringInterface(LOG_TOPIC)
{
    this->setInterfaceName(LOG_TOPIC);
    this->setSamplingEnabled(true);
    this->setMQTTPublish(true);
    this->setMQTTSubscribe(false);
    this->setSamplingRate(LOG_SAMPLE_RATE);
    this->preventDebugLogging();
}

inline void LogInterface::init()
{
}

inline String LogInterface::sample()
{
    logMsg = Logger.pop();
    if (Logger.queueSize())
    {
        this->forceResample();
    }
    return logMsg;
}

inline int LogInterface::cmp(String oldValue, String newValue)
{
    int value = (oldValue == nullptr) ? 0 : -1;
    return value;
}

inline String LogInterface::valueToString()
{
    return "";
}

#endif //LOGINTERFACE_H