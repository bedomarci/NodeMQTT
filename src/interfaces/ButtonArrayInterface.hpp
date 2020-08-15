#ifndef BUTTONARAYINTERFACE_H
#define BUTTONARAYINTERFACE_H
#include "ArrayInterface.hpp"
#include "JC_Button.h"

template <uint8_t LENGTH>
class ButtonArrayInterface : public ArrayInterface<uint8_t, LENGTH>
{
  public:
    ButtonArrayInterface(String topic, const uint8_t buttonPins[LENGTH], unsigned long debounceDelay = DEFAULT_BUTTON_DEBOUNCE, bool puEnabled = true, bool invert = false);
    void init();
    void press(uint16_t index);
    void release(uint16_t index);

  protected:
    Array<uint8_t, LENGTH> buffer;
    Array<uint8_t, LENGTH> sample();
    void updatePhisicalInterface(uint8_t newValue);
    Button *buttons[LENGTH];
};
template <uint8_t LENGTH>
inline ButtonArrayInterface<LENGTH>::ButtonArrayInterface(String topic, const uint8_t buttonPins[LENGTH], unsigned long debounceDelay, bool puEnabled, bool invert)
    : ArrayInterface<uint8_t, LENGTH>(topic)
{
    for (int i = 0; i < LENGTH; i++)
    {
        buttons[i] = new Button(buttonPins[i], debounceDelay, puEnabled, invert);
    }
    this->setInterfaceName(BUTTON_ARRAY_NAME);
    this->setSamplingEnabled(true);
}
template <uint8_t LENGTH>
inline void ButtonArrayInterface<LENGTH>::updatePhisicalInterface(uint8_t newValue)
{
    return;
}
template <uint8_t LENGTH>
inline Array<uint8_t, LENGTH> ButtonArrayInterface<LENGTH>::sample()
{
    array_fill<uint8_t, LENGTH>(buffer, 0);
    for (int i = 0; i < LENGTH; i++)
    {
        buttons[i]->read();
        buffer[i] = (uint8_t)buttons[i]->isPressed();
    }

    return buffer;
}
template <uint8_t LENGTH>
inline void ButtonArrayInterface<LENGTH>::press(uint16_t index)
{
    this->writeItem(index, 1);
}
template <uint8_t LENGTH>
inline void ButtonArrayInterface<LENGTH>::release(uint16_t index)
{
    this->writeItem(index, 0);
}
template <uint8_t LENGTH>
inline void ButtonArrayInterface<LENGTH>::init()
{
    // IntegerInterface::boot();
}

#endif //BUTTONARAYINTERFACE_H