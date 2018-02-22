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
#endif //RELAYINTERFACE_H