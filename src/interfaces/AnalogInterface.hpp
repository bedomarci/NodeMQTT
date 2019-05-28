#ifndef ANALOGINTERFACE_H
#define ANALOGINTERFACE_H
#include "DataInterface.hpp"

class AnalogInterface : public DataInterface<int>
{
  public:
    AnalogInterface(String topic, uint8_t analogPin, uint8_t tolerance);
    void init();

  protected:
    uint8_t _analogPin;
    uint8_t _tolerance;
    int cmp(int oldValue, int newValue);
    int sample();
    void updatePhisicalInterface(int newValue);
};

inline AnalogInterface::AnalogInterface(String topic, uint8_t analogPin, uint8_t tolerance = 0)
    : DataInterface<int>(topic)
{
    _analogPin = analogPin;
    _tolerance = tolerance;
    pinMode(analogPin, INPUT);
    _interfaceName = ANALOG_NAME;
    setMQTTSubscribe(false);
    setMQTTPublish(true);
    setSamplingEnabled(true);
}

inline void AnalogInterface::updatePhisicalInterface(int newValue)
{
    return;
}

inline int AnalogInterface::sample()
{
    return analogRead(_analogPin);
}

inline int AnalogInterface::cmp(int oldValue, int newValue)
{
    if (abs(oldValue - newValue) <= _tolerance)
        return 0;
    return DataInterface<int>::cmp(oldValue, newValue);
}

inline void AnalogInterface::init()
{
    DataInterface<int>::init();
}

#endif //ANALOGINTERFACE_H