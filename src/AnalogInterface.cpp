#include "AnalogInterface.hpp"

AnalogInterface::AnalogInterface(String topic, uint8_t analogPin, uint8_t tolerance=0) : IntegerInterface(topic, topic) {
    _analogPin = analogPin;
    _tolerance = tolerance;
    pinMode(analogPin, INPUT);
    hasMQTTSubscribe = false;
    interfaceName = "an";
}

void AnalogInterface::updatePhisicalInterface(int newValue) {
    return;
}

int AnalogInterface::sample() {
    return analogRead(_analogPin);
}

int AnalogInterface::cmp(int oldValue, int newValue){
    if (abs(oldValue-newValue)<=_tolerance) return 0;
    return IntegerInterface::cmp(oldValue, newValue);
}