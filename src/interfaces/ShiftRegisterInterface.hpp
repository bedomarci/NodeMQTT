#ifndef SHIFTREGISTERINTERFACE_H
#define SHIFTREGISTERINTERFACE_H
#include "ArrayInterface.hpp"

template <unsigned int LENGTH>
class ShiftRegisterInterface : public ArrayInterface<uint8_t, LENGTH>
{
  public:
    ShiftRegisterInterface(String topic, uint8_t latchPin, uint8_t clockPin, uint8_t dataPin);
    void init();

  protected:
    uint8_t _latchPin;
    uint8_t _clockPin;
    uint8_t _dataPin;
    uint8_t numberOfShiftRegisters;
    void updatePhisicalInterface(Array<uint8_t, LENGTH>);
};

template <unsigned int LENGTH>
inline void ShiftRegisterInterface<LENGTH>::init()
{
    this->setSamplingEnabled(false);
    this->setMQTTPublish(false);
}

template <unsigned int LENGTH>
inline ShiftRegisterInterface<LENGTH>::ShiftRegisterInterface(String topic, uint8_t latchPin, uint8_t clockPin, uint8_t dataPin)
    : ArrayInterface<uint8_t, LENGTH>(topic)
{
    _latchPin = latchPin;
    _clockPin = clockPin;
    _dataPin = dataPin;

    numberOfShiftRegisters = (LENGTH >> 3) + 1;

    pinMode(_latchPin, OUTPUT);
    pinMode(_clockPin, OUTPUT);
    pinMode(_dataPin, OUTPUT);

    digitalWrite(_latchPin, HIGH);
    digitalWrite(_clockPin, HIGH);
}


template <unsigned int LENGTH>
inline void ShiftRegisterInterface<LENGTH>::updatePhisicalInterface(Array<uint8_t, LENGTH> newValues)
{
    digitalWrite(_latchPin, LOW);
    for (int i = 0; i < numberOfShiftRegisters; i++)
    {
        uint8_t byteToSend = 0;
        uint8_t remainingBits = LENGTH - (i * 8);
        if (remainingBits > 8)
            remainingBits = 8;

        for (int j = 0; j < remainingBits; j++)
        {
            uint8_t bitToSend = newValues.item[(i * 8) + j] != 0;
            byteToSend = byteToSend & (bitToSend << j);
        }
        shiftOut(_dataPin, _clockPin, MSBFIRST, byteToSend);
    }
    digitalWrite(_latchPin, HIGH);
}

#endif //SHIFTREGISTERINTERFACE_H