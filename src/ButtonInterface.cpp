#include "ButtonInterface.hpp"


ButtonInterface::ButtonInterface(String topic, uint8_t btnPin, unsigned long debounceDelay) : IntegerInterface(topic, topic) {
    _btnPin = btnPin;
    _debounceDelay = debounceDelay;
    pinMode(btnPin, INPUT_PULLUP);
    interfaceName = "btn";
}

void ButtonInterface::updatePhisicalInterface(int newValue) {
    return;
}

int ButtonInterface::sample() {
    int reading = digitalRead(_btnPin);
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > _debounceDelay) {
        if (reading != buttonState) {
            buttonState = reading;
        }
    }
    lastButtonState = reading;
    return buttonState;
}
