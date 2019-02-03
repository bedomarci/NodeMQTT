#ifndef MUXINTERFACE_H
#define MUXINTERFACE_H
#include "ArrayInterface.hpp"

class Mux8Interface : public ArrayInterface<uint8_t, 8>
{
  public:
    Mux8Interface(String topic, uint8_t inputPin, uint8_t address0Pin, uint8_t address1Pin, uint8_t address2Pin);
    void init();

  protected:
    Array<uint8_t, 8> sample();
    uint8_t _inputPin;
    uint8_t _address0Pin;
    uint8_t _address1Pin;
    uint8_t _address2Pin;
};

inline Mux8Interface::Mux8Interface(String topic, uint8_t inputPin, uint8_t address0Pin, uint8_t address1Pin, uint8_t address2Pin)
    : ArrayInterface<uint8_t, 8>(topic)
{
    _inputPin = inputPin;
    _address0Pin = address0Pin;
    _address1Pin = address1Pin;
    _address2Pin = address2Pin;
    setSamplingEnabled(true);
    setMQTTPublish(true);
    setMQTTSubscribe(false);
}

inline void Mux8Interface::init()
{
    pinMode(_address0Pin, OUTPUT);
    pinMode(_address1Pin, OUTPUT);
    pinMode(_address2Pin, OUTPUT);
    pinMode(_inputPin, INPUT_PULLUP);
}

inline Array<uint8_t, 8> Mux8Interface::sample()
{
    Array<uint8_t, 8> array;
    for (uint8_t count = 0; count <= 7; count++)
    {
        uint8_t r0 = bitRead(count, 0);
        uint8_t r1 = bitRead(count, 1);
        uint8_t r2 = bitRead(count, 2);
        digitalWrite(_address0Pin, r0);
        digitalWrite(_address1Pin, r1);
        digitalWrite(_address2Pin, r2);
        array.item[count] = digitalRead(_inputPin);
    }
    return array;
}

#endif