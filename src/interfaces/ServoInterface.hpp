#ifndef SERVOINTERFACE_H
#define SERVOINTERFACE_H
#include "DataInterface.hpp"
// #if defined (ESP32)
// #include <ESP32_Servo.h>
// #else
#include <Servo.h>
// #endif

class ServoInterface : public DataInterface<int>
{
  public:
    ServoInterface(String topic, uint8_t seroPin);
    void init();
    void attach();
    void detach();

  protected:
    uint8_t _servoPin;
    Servo servo;
    void updatePhisicalInterface(int newValue);
};

inline ServoInterface::ServoInterface(String topic, uint8_t servoPin) : DataInterface<int>(topic)
{
    _servoPin = servoPin;
    setSamplingEnabled(false);
    setMQTTPublish(false);
}

inline void ServoInterface::init()
{
    pinMode(_servoPin, OUTPUT);
    attach();
}

inline void ServoInterface::updatePhisicalInterface(int newValue)
{
    servo.write(newValue);
}

inline void ServoInterface::attach()
{
    servo.attach(_servoPin);
}
inline void ServoInterface::detach()
{
    servo.detach();
}

#endif //SERVOINTERFACE_H