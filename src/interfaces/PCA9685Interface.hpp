#ifndef NODEMQTT_PCA9685INTERFACE_HPP
#define NODEMQTT_PCA9685INTERFACE_HPP


#include <Arduino.h>
#include <Wire.h>
#include "_I2CInterface.hpp"
#include "ExpanderInterface.hpp"
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

private:
    Adafruit_PWMServoDriver *driver;
    bool _invert = false;
};

PCA9685Interface::PCA9685Interface(String publishTopic, String subscribeTopic, TwoWire *i2c) : I2CInterface<uint16_t, 16>(publishTopic, subscribeTopic, i2c) {
    this->setAddress(0x40);
    this->setSamplingEnabled(false);
    this->setMQTTPublishSubscribe(true, false);
    this->setInterfaceName(PCA9685_NAME);
}

PCA9685Interface::PCA9685Interface(String publishTopic, String subscribeTopic, const uint8_t addr) : I2CInterface<uint16_t, 16>(publishTopic, subscribeTopic, addr) {
    this->setSamplingEnabled(false);
    this->setMQTTPublishSubscribe(true, false);
    this->setInterfaceName(PCA9685_NAME);
}

PCA9685Interface::PCA9685Interface(String publishTopic, String subscribeTopic, TwoWire *i2c, const uint8_t addr) : I2CInterface<uint16_t, 16>(publishTopic, subscribeTopic, i2c,
                                                                                                                                              addr) {
    this->setSamplingEnabled(false);
    this->setMQTTPublishSubscribe(true, false);
    this->setInterfaceName(PCA9685_NAME);
}

PCA9685Interface::PCA9685Interface(String topic, TwoWire *i2c) : PCA9685Interface(topic, topic, i2c) {}

PCA9685Interface::PCA9685Interface(String topic, const uint8_t addr) : PCA9685Interface(topic, topic, addr) {}

PCA9685Interface::PCA9685Interface(String topic, TwoWire *i2c, const uint8_t addr) : PCA9685Interface(topic, topic, i2c, addr) {}


void PCA9685Interface::init() {
    I2CInterface<uint16_t, 16>::init();
    driver = new Adafruit_PWMServoDriver(this->getAddress(), *this->getWire());
    driver->begin();
}

void PCA9685Interface::updatePhisicalInterface(Array<uint16_t, 16> newValue) {
    for (int i = 0; i < 16; i++)
        driver->setPin(i, newValue[i], _invert);
}

void PCA9685Interface::reset() {
    driver->reset();
}

void PCA9685Interface::sleep() {
    driver->sleep();
}

void PCA9685Interface::wakeup() {
    driver->wakeup();
}

void PCA9685Interface::setPWMFreq(float freq) {
    driver->setPWMFreq(freq);
}

void PCA9685Interface::setOutputMode(bool totempole) {
    driver->setOutputMode(totempole);
}

void PCA9685Interface::invert(bool invert){
    _invert = invert;
}

void PCA9685Interface::setExtClk(uint8_t prescale) {
    driver->setExtClk(prescale);
}


#endif //NODEMQTT_PCA9685INTERFACE_HPP
