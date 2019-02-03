#ifndef MCP23017Interface_H
#define MCP23017Interface_H

#define GPIO_LENGTH 16
#include "ArrayInterface.hpp"
#include <Adafruit_MCP23017.h>
#include <Wire.h>

class MCP23017Interface : public ArrayInterface<uint8_t, TYPE>
{
  public:
    MCP23017Interface(String topic, uint8_t sdaPin, uint8_t sclPin, uint8_t address, uint8_t interruptPin = nullptr);
    void init();
    void pinMode(uint8_t pin, uint8_t dirction);

  protected:
    uint8_t _sdaPin;
    uint8_t _sclPin;
    uint8_t _address;
    uint8_t _interruptPin;
    uint16_t _pinMap = 0xFFFF;

    bool PCFInterrupt = false;
    void updatePhisicalInterface(Array<uint8_t, TYPE>);

    void expanderInterruptCallback();

}

inline MCP23017Interface(String topic, uint8_t sdaPin, uint8_t sclPin, uint8_t address, uint8_t interruptPin = nullptr)
    : ArrayInterface<uint8_t, TYPE>(topic)
{
    _sdaPin = sdaPin;
    _sclPin = sclPin;
    _address = address;
    _interruptPin = interruptPin;
}

inline void MCP23017Interface::init()
{
    this->setSamplingEnabled(false);
    this->setMQTTPublish(true);
    mcp.begin(_address);
    if (_interruptPin != nullptr)
    {
        pinMode(_interruptPin, INPUT_PULLUP);
        mcp.setupInterrupts(true, false, LOW);
        attachInterrupt(digitalPinToInterrupt(_interruptPin), [this]() { expanderInterruptCallback(); }, FALLING);
    }
}
inline void MCP23017Interface::updatePhisicalInterface(Array<uint8_t, TYPE> newValue)
{
    for (int i = 0; i < TYPE; i++)
    {
        if (bitRead(_pinMap, i) == OUTPUT)
            mcp.digitalWrite(i, newValue.item[i] > 0);
    }
}
inline void MCP23017Interface::expanderInterruptCallback()
{
    ArrayInterface<uint8_t, GPIO_LENGTH> arr;

    uint16_t data;
    data = mcp.readGPIOAB();

    for (int i = 0; i < GPIO_LENGTH; i++)
    {
        arr.item[i] = bitRead(data, i);
    }
    this->write(arr);
}

inline void MCP23017Interface::pinMode(uint8_t pin, uint8_t direction)
{
    if (direction == INPUT || direction == INPUT_PULLUP)
    {
        mcp.pinMode(pin, INPUT);
        mcp.setupInterruptPin(mcpPinA, FALLING);
    }
    if (direction == INPUT_PULLUP)
    {
        mcp.pullUp(pin, HIGH);
    }
    if (direction == OUTPUT)
    {
        pinMode(OUTPUT);
    }
}

#endif //MCP23017Interface_H