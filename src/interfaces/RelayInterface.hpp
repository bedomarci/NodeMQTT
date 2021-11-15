#ifndef RELAYINTERFACE_H
#define RELAYINTERFACE_H

#include "DataInterface.hpp"

class RelayInterface : public DataInterface<int> {
public:
    RelayInterface(String topic, uint8_t relayPin, bool invertPin = false);

    RelayInterface(String publishTopic, String subscribeTopic, uint8_t relayPin, bool invertPin = false);

    void init();

    void close();

    void open();

protected:
    uint8_t _relayPin;
    uint8_t _invertPin;

    // int sample();
    void updatePhisicalInterface(int newValue);
};

inline RelayInterface::RelayInterface(String publishTopic, String subscribeTopic, uint8_t relayPin, bool invertPin)
: DataInterface<int>(publishTopic,subscribeTopic) {
    _relayPin = relayPin;
    _invertPin = invertPin;
    _interfaceName = RELAY_NAME;
    setSamplingEnabled(false);
}


inline RelayInterface::RelayInterface(String topic, uint8_t relayPin, bool invertPin)
        : RelayInterface(topic, topic, relayPin, invertPin) {
}

inline void RelayInterface::updatePhisicalInterface(int newValue) {
    digitalWrite(_relayPin, ((bool) newValue) != _invertPin);
}

inline void RelayInterface::init() {
    DataInterface<int>::init();
    pinMode(_relayPin, OUTPUT);
}

inline void RelayInterface::open() {
    this->write(1);
}

inline void RelayInterface::close() {
    this->write(0);
}

#endif //RELAYINTERFACE_H