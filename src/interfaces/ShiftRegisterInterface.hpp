#ifndef SHIFTREGISTERINTERFACE_H
#define SHIFTREGISTERINTERFACE_H
#include "_NodeInterface.hpp"

template <unsigned int LENGTH>
struct ShiftRegisterBits
{
    uint8_t bit[LENGTH];
};

template <unsigned int LENGTH>
class ShiftRegisterInterface : public NodeInterface<ShiftRegisterBits<LENGTH>>
{
  public:
    ShiftRegisterInterface(String &topic, uint8_t latchPin, uint8_t clockPin, uint8_t dataPin);

  protected:
    uint8_t _latchPin;
    uint8_t _clockPin;
    uint8_t _dataPin;
    uint8_t numberOfShiftRegisters;

    int sample();
    void updatePhisicalInterface(ShiftRegisterBits<LENGTH>);
    ShiftRegisterBits<LENGTH> fromJSON(JsonObject &rootObject);
    JsonObject &toJSON(ShiftRegisterBits<LENGTH>);
    int cmp(ShiftRegisterBits<LENGTH> oldValue, ShiftRegisterBits<LENGTH> newValue);
};

template <unsigned int LENGTH>
inline ShiftRegisterInterface<LENGTH>::ShiftRegisterInterface(String &topic, uint8_t latchPin, uint8_t clockPin, uint8_t dataPin)
    : NodeInterface<int>(topic, topic)
{
    _latchPin = latchPin;
    _clockPin = clockPin;
    _dataPin = dataPin;

    numberOfShiftRegisters = (LENGTH >> 3) + 1

                             pinMode(_latchPin, OUTPUT);
    pinMode(_clockPin, OUTPUT);
    pinMode(_dataPin, OUTPUT);

    digitalWrite(_latchPin, HIGH);
    digitalWrite(_clockPin, HIGH);
}

template <unsigned int LENGTH>
inline int ShiftRegisterInterface::sample()
{
}

template <unsigned int LENGTH>
inline void ShiftRegisterInterface::updatePhisicalInterface(ShiftRegisterBits<LENGTH> newValues)
{
    digitalWrite(latchPin, LOW);
    for (int i = 0; i < numberOfShiftRegisters; i++)
    {
        uint8_t byteToSend = 0;
        uint8_t remainingBits = LENGTH - (i * 8);
        if (remainingBits > 8)
            remainingBits = 8;

        for (int j = 0; j < remainingBits; j++)
        {
            uint8_t bitToSend = newValues.bit[(i * 8) + j] != 0;
            byteToSend = byteToSend & (bitToSend << j);
        }
        shiftOut(_datapin, _clockPin, MSBFIRST, byteToSend);
    }
    digitalWrite(latchPin, HIGH);
}

template <unsigned int LENGTH>
inline ShiftRegisterBits<LENGTH> ShiftRegisterInterface::fromJSON(JsonObject &rootObject)
{
    ShiftRegisterBits<LENGTH> values;
    JsonArray &data = rootObject["data"];
    if (data.size() != LENGTH)
    {
        e(P("Incoming JSON array length mismatch on shift register interface!"));
        return currentValue;
    }
    for (int i = 0; i < LENGTH; i++)
    {
        values.bit[i] = data.[i].as<unsigned short>();
    }
    return values;
}

template <unsigned int LENGTH>
inline JsonObject &ShiftRegisterInterface::toJSON(ShiftRegisterBits<LENGTH> value)
{
    return 0;
}

template <unsigned int LENGTH>
inline int ShiftRegisterInterface::cmp(ShiftRegisterBits<LENGTH> oldValue, ShiftRegisterBits<LENGTH> newValue)
{
    return -1;
}

#endif //SHIFTREGISTERINTERFACE_H