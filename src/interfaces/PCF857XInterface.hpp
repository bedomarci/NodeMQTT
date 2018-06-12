#ifndef PCF857XINTERFACE_H
#define PCF857XINTERFACE_H
#include "ArrayInterface.hpp"
#include <pcf8574_esp.h>
#include <Wire.h>

enum PCF857XType
{
    PCF8574 = 8,
    PCF8575 = 16,
}

template <PCF857XType TYPE>
class PCF857XInterface : public ArrayInterface<uint8_t, TYPE>
{
  public:
    PCF857XInterface(String topic, uint8_t sdaPin, uint8_t sclPin, uint8_t address, uint8_t interruptPin = nullptr);
    void init();
    void pinMode(uint8_t pin, uint8_t dirction);

  protected:
    uint8_t _sdaPin;
    uint8_t _sclPin;
    uint8_t _address;
    uint8_t _interruptPin;
    uint16_t _pinMap = 0xFFFF;

    bool PCFInterrupt = false;
    TwoWire *wire;

    PCF857x *expander void updatePhisicalInterface(Array<uint8_t, TYPE>);

    void expanderInterruptCallback();

}

template <PCF857XType TYPE>
inline PCF857XInterface(String topic, uint8_t sdaPin, uint8_t sclPin, uint8_t address, uint8_t interruptPin = nullptr)
    : ArrayInterface<uint8_t, TYPE>(topic)
{
    _sdaPin = sdaPin;
    _sclPin = sclPin;
    _address = address;
    _interruptPin = interruptPin;
}
template <PCF857XType TYPE>
inline void PCF857XInterface::init()
{
    this->setSamplingEnabled(false);
    this->setMQTTPublish(true);

    wire = new TwoWire();
    wire->begin(_sdaPin, _sclPin);
    wire->setClock(I2C_CLOCK_SPEED);
    expander = new PCF857x(0x20, wire, (TYPE == PCF8575));
    expander.begin();
    if (_interruptPin != nullptr)
    {
        pinMode(_interruptPin, INPUT_PULLUP);
        expander.resetInterruptPin();
        attachInterrupt(digitalPinToInterrupt(_interruptPin), [this]() { expanderInterruptCallback(); }, FALLING);
    }
}
template <PCF857XType TYPE>
inline void PCF857XInterface::updatePhisicalInterface(Array<uint8_t, TYPE> newValue)
{
    for (int i = 0; i < TYPE; i++)
    {
        if (bitRead(_pinMap, i) == OUTPUT)
            expander->write(newValue.item[i])
    }
}
template <PCF857XType TYPE>
inline void PCF857XInterface::expanderInterruptCallback()
{
    ArrayInterface<uint8_t, TYPE> arr;
    if (TYPE == PCF8574)
    {
        uint8_t data;
        data = expander->read8();
    }
    else
    {
        uint16_t data;
        data = expander->read16();
    }
    for (int i = 0; i < TYPE; i++)
    {
        arr.item[i] = bitRead(data, i);
    }
    this->write(arr);
}

template <PCF857XType TYPE>
inline void PCF857XInterface::pinMode(uint8_t pin, uint8_t direction)
{
    bitWrite(_pinMap, pin, direction);
}

#endif //PCF857XINTERFACE_H