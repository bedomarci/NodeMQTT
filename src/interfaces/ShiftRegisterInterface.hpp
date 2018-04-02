#ifndef SHIFTREGISTERINTERFACE_H 
#define SHIFTREGISTERINTERFACE_H
#include "IntegerInterface.hpp"

class ShiftRegisterInterface : public IntegerInterface {
    public:
        ShiftRegisterInterface(String& topic, uint8_t latchPin, uint8_t clockPin, uint8_t dataPin);
        
    protected:
        uint8_t _latchPin;
        uint8_t _clockPin;
        uint8_t _dataPin;
        int sample();
        void updatePhisicalInterface(int newValue);
};
#endif //SHIFTREGISTERINTERFACE_H