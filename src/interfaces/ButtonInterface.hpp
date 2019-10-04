#ifndef BUTTONINTERFACE_H
#define BUTTONINTERFACE_H
#include "DataInterface.hpp"
#include "JC_Button.h"

class ButtonInterface : public DataInterface<int>
{
  public:
    ButtonInterface(String topic, uint8_t buttonPin, unsigned long debounceDelay = DEFAULT_BUTTON_DEBOUNCE, bool pullUp = true, bool invert = false);
    void init();
    void press();
    void release();

  protected:
    int sample();
    void updatePhisicalInterface(int newValue);
    Button *button;
};


inline ButtonInterface::ButtonInterface(String topic, uint8_t buttonPin, unsigned long debounceDelay, bool pullUp, bool invert)
    : DataInterface<int>(topic)
{
    button = new Button(buttonPin, debounceDelay, pullUp, invert);
    this->setInterfaceName(BUTTON_NAME);
    this->setSamplingEnabled(true);
}

inline void ButtonInterface::updatePhisicalInterface(int newValue)
{
    return;
}

inline int ButtonInterface::sample()
{
    button->read();
    return button->isPressed();
}
inline void ButtonInterface::press()
{
    write(1);
}
inline void ButtonInterface::release()
{
    write(0);
}
inline void ButtonInterface::init()
{
}

#endif //BUTTONINTERFACE_H