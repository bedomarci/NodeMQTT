#ifndef BUTTONINTERFACE_H
#define BUTTONINTERFACE_H
#include "_IntegerInterface.hpp"


class ButtonInterface : public IntegerInterface {
    public:
        ButtonInterface(String topic, uint8_t btnPin, unsigned long debounceDelay);
        
    protected:
        
        uint8_t _btnPin;
        int sample();
        void updatePhisicalInterface(int newValue);
       
        int buttonState;             // the current reading from the input pin
        int lastButtonState = LOW;   // the previous reading from the input pin
        unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
        unsigned long _debounceDelay = 50;    // the debounce time; increase if the output flickers
};
#endif //BUTTONINTERFACE_H