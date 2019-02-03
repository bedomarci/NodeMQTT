#ifndef RELAYINTERFACE_H
#define RELAYINTERFACE_H
#include "DataInterface.hpp"

class RelayInterface : public DataInterface<int>
{
  public:
    RelayInterface(String topic, uint8_t relayPin, bool invertPin = false);
    void init();

  protected:
    uint8_t _relayPin;
    uint8_t _invertPin;

    // int sample();
    void updatePhisicalInterface(int newValue);
};

inline RelayInterface::RelayInterface(String topic, uint8_t relayPin, bool invertPin)
    : DataInterface<int>(topic)
{
    _relayPin = relayPin;
    _invertPin = invertPin;
    _interfaceName = RELAY_NAME;
    setSamplingEnabled(false);
}
inline void RelayInterface::updatePhisicalInterface(int newValue)
{
    digitalWrite(_relayPin, ((bool)newValue) != _invertPin);
}

inline void RelayInterface::init()
{
    DataInterface<int>::init();
    pinMode(_relayPin, OUTPUT);
}
#endif //RELAYINTERFACE_H