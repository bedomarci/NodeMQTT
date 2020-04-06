#ifndef PCA9685INTERFACE_HPP
#define PCA9685INTERFACE_HPP

#include <Wire.h>
#include "_I2CInterface.hpp"
#include "Adafruit_PWMServoDriver.h"

class PCA9685Interface : public I2CInterface<uint16_t, 16> {
public:
    PCA9685Interface(String publishTopic, String subscribeTopic, TwoWire *i2c);

    PCA9685Interface(String publishTopic, String subscribeTopic, const uint8_t addr);

    PCA9685Interface(String publishTopic, String subscribeTopic, TwoWire *i2c, const uint8_t addr);

    PCA9685Interface(String topic, TwoWire *i2c);

    PCA9685Interface(String topic, const uint8_t addr);

    PCA9685Interface(String topic, TwoWire *i2c, const uint8_t addr);

    void updatePhisicalInterface(Array<uint16_t, 16>);

    void init();

    void invert(bool invert);

    void reset();

    void sleep();

    void wakeup();

    void setExtClk(uint8_t prescale);

    void setPWMFreq(float freq);

    void setOutputMode(bool totempole);

    void setOscillatorFrequency(uint32_t freq);

private:
    Adafruit_PWMServoDriver *driver;
    bool _invert = false;
};

inline PCA9685Interface::PCA9685Interface(String publishTopic, String subscribeTopic, TwoWire *i2c) : I2CInterface<uint16_t, 16>(publishTopic, subscribeTopic, i2c) {
    this->setAddress(0x40);
    this->setSamplingEnabled(false);
    this->setMQTTPublishSubscribe(true, false);
    this->setInterfaceName(PCA9685_NAME);
}

inline PCA9685Interface::PCA9685Interface(String publishTopic, String subscribeTopic, const uint8_t addr) : I2CInterface<uint16_t, 16>(publishTopic, subscribeTopic, addr) {
    this->setSamplingEnabled(false);
    this->setMQTTPublishSubscribe(true, false);
    this->setInterfaceName(PCA9685_NAME);
}

inline PCA9685Interface::PCA9685Interface(String publishTopic, String subscribeTopic, TwoWire *i2c, const uint8_t addr) : I2CInterface<uint16_t, 16>(publishTopic, subscribeTopic, i2c,
                                                                                                                                                     addr) {
    this->setSamplingEnabled(false);
    this->setMQTTPublishSubscribe(true, false);
    this->setInterfaceName(PCA9685_NAME);
}

inline PCA9685Interface::PCA9685Interface(String topic, TwoWire *i2c) : PCA9685Interface(topic, topic, i2c) {}

inline PCA9685Interface::PCA9685Interface(String topic, const uint8_t addr) : PCA9685Interface(topic, topic, addr) {}

inline PCA9685Interface::PCA9685Interface(String topic, TwoWire *i2c, const uint8_t addr) : PCA9685Interface(topic, topic, i2c, addr) {}


inline void PCA9685Interface::init() {
    I2CInterface<uint16_t, 16>::init();
    driver = new Adafruit_PWMServoDriver(this->getAddress(), *this->getWire());
    driver->begin();
}

inline void PCA9685Interface::updatePhisicalInterface(Array<uint16_t, 16> newValue) {
    for (int i = 0; i < 16; i++) {
        driver->setPin(i, newValue[i], _invert);
    }
}

inline void PCA9685Interface::reset() {
    driver->reset();
}

inline void PCA9685Interface::sleep() {
    driver->sleep();
}

inline void PCA9685Interface::wakeup() {
    driver->wakeup();
}

inline void PCA9685Interface::setPWMFreq(float freq) {
    driver->setPWMFreq(freq);
}

inline void PCA9685Interface::setOscillatorFrequency(uint32_t freq) {
    driver->setOscillatorFrequency(freq);
}

inline void PCA9685Interface::setOutputMode(bool totempole) {
    driver->setOutputMode(totempole);
}

inline void PCA9685Interface::invert(bool invert) {
    _invert = invert;
}

inline void PCA9685Interface::setExtClk(uint8_t prescale) {
    driver->setExtClk(prescale);
}


#endif //NODEMQTT_PCA9685INTERFACE_HPP
