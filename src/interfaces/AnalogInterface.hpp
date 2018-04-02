#ifndef ANALOGINTERFACE_H
#define ANALOGINTERFACE_H
#include "IntegerInterface.hpp"


class AnalogInterface : public IntegerInterface {
    public:
        AnalogInterface(String topic, uint8_t analogPin, uint8_t tolerance);
    protected:
        uint8_t _analogPin;
        uint8_t _tolerance;
        int cmp(int oldValue, int newValue);
        int sample();
        void updatePhisicalInterface(int newValue);
};

inline
AnalogInterface::AnalogInterface(String topic, uint8_t analogPin, uint8_t tolerance=0) : IntegerInterface(topic, topic) {
    _analogPin = analogPin;
    _tolerance = tolerance;
    pinMode(analogPin, INPUT);
    setMQTTSubscribe(false);
    interfaceName = ANALOG_NAME;
}

inline
void AnalogInterface::updatePhisicalInterface(int newValue) {
    return;
}

inline
int AnalogInterface::sample() {
    return analogRead(_analogPin);
}

inline
int AnalogInterface::cmp(int oldValue, int newValue){
    if (abs(oldValue-newValue)<=_tolerance) return 0;
    return IntegerInterface::cmp(oldValue, newValue);
}

#endif //ANALOGINTERFACE_H