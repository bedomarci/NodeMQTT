#ifndef RELAYINTERFACE_H
#define RELAYINTERFACE_H
#include "DataInterface.hpp"

class RelayInterface : public DataInterface<int>
{
  public:
    RelayInterface(String topic, uint8_t relayPin);

  protected:
    uint8_t _relayPin;

    int sample();
    void updatePhisicalInterface(int newValue);
};

inline RelayInterface::RelayInterface(String topic, uint8_t relayPin)
    : DataInterface<int>(topic)
{
    _relayPin = relayPin;
    pinMode(relayPin, OUTPUT);
    interfaceName = RELAY_NAME;
}
inline void RelayInterface::updatePhisicalInterface(int newValue)
{
    digitalWrite(_relayPin, newValue);
}

inline void RelayInterface::init()
{
    DataInterface<int>::init();
    setSamplingEnabled(false);
}
// inline int RelayInterface::sample()
// {
//     return currentValue;
// }

#endif //RELAYINTERFACE_H