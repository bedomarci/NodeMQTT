#ifndef RELAYINTERFACE_H
#define RELAYINTERFACE_H
#include "_IntegerInterface.hpp"


class RelayInterface : public IntegerInterface {
    public:
        RelayInterface(String topic, uint8_t relayPin);
        
    protected:
        uint8_t _relayPin;

        int sample();
        void updatePhisicalInterface(int newValue);
};

inline
RelayInterface::RelayInterface(String topic, uint8_t relayPin) : IntegerInterface(topic, topic) {
    _relayPin = relayPin;
    setSamplingEnabled(false);
    pinMode(relayPin, OUTPUT);
    interfaceName = RELAY_NAME;
}
inline
void RelayInterface::updatePhisicalInterface(int newValue) {
    digitalWrite(_relayPin, newValue);
}
inline
int RelayInterface::sample() {
    return currentValue;
}

#endif //RELAYINTERFACE_H