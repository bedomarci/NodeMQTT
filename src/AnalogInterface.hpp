#ifndef ANALOGINTERFACE_H
#define ANALOGINTERFACE_H
#include "_IntegerInterface.hpp"


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
#endif //ANALOGINTERFACE_H