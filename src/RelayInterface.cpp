#include "RelayInterface.hpp"

RelayInterface::RelayInterface(String topic, uint8_t relayPin) : IntegerInterface(topic, topic) {
    _relayPin = relayPin;
    samplingEnabled = false;
    pinMode(relayPin, OUTPUT);
    interfaceName = "rel";
}

void RelayInterface::updatePhisicalInterface(int newValue) {
    digitalWrite(_relayPin, newValue);
}

int RelayInterface::sample() {
    return currentValue;
}