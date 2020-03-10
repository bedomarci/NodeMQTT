#ifndef I2CINTERFACE_H
#define I2CINTERFACE_H



#include <Arduino.h>
#include <Wire.h>
#include "ArrayInterface.hpp"


#define I2CWRITE(x) Wire.write(x)
#define I2CREAD() Wire.read()

template<typename T, uint16_t LENGTH>
class I2CInterface : public ArrayInterface<T, LENGTH> {
public:
    I2CInterface(String publishTopic, String subscribeTopic);

    I2CInterface(String publishTopic, String subscribeTopic, TwoWire *i2c);

    I2CInterface(String publishTopic, String subscribeTopic, uint8_t sda, uint8_t scl);

    I2CInterface(String publishTopic, String subscribeTopic, uint8_t address);

    I2CInterface(String publishTopic, String subscribeTopic, TwoWire *i2c, uint8_t address);

    I2CInterface(String publishTopic, String subscribeTopic, uint8_t sda, uint8_t scl, uint8_t address);


    void init();

    void setI2C(TwoWire *i2c);

    void setI2C(uint8_t sda, uint8_t scl);

    void setAddress(uint8_t address);

    uint8_t getAddress();

protected:

    TwoWire *getWire();

    uint8_t i2cReadByte();

    void i2cWriteByte(uint8_t data);

    void i2cWrite2Bytes(uint8_t data1, uint8_t data2);

private:
    TwoWire *_i2c;
    uint8_t _sdaPin = 255;
    uint8_t _sclPin = 255;
    uint8_t _address;
};

template<typename T, uint16_t LENGTH>
inline I2CInterface<T, LENGTH>::I2CInterface(String publishTopic, String subscribeTopic): ArrayInterface<T, LENGTH>(
        publishTopic, subscribeTopic) {}

template<typename T, uint16_t LENGTH>
inline I2CInterface<T, LENGTH>::I2CInterface(String publishTopic, String subscribeTopic, TwoWire *i2c)
        : ArrayInterface<T, LENGTH>(publishTopic, subscribeTopic) {
    setI2C(i2c);
}

template<typename T, uint16_t LENGTH>
inline I2CInterface<T, LENGTH>::I2CInterface(String publishTopic, String subscribeTopic, uint8_t sda, uint8_t scl)
        : ArrayInterface<T, LENGTH>(publishTopic, subscribeTopic) {
    setI2C(sda, scl);
}

template<typename T, uint16_t LENGTH>
inline I2CInterface<T, LENGTH>::I2CInterface(String publishTopic, String subscribeTopic, uint8_t address)
        : ArrayInterface<T, LENGTH>(publishTopic, subscribeTopic) {
    setAddress(address);
}

template<typename T, uint16_t LENGTH>
inline I2CInterface<T, LENGTH>::I2CInterface(String publishTopic, String subscribeTopic, TwoWire *i2c, uint8_t address)
        : ArrayInterface<T, LENGTH>(publishTopic, subscribeTopic) {
    setI2C(i2c);
    setAddress(address);
}

template<typename T, uint16_t LENGTH>
inline I2CInterface<T, LENGTH>::I2CInterface(String publishTopic, String subscribeTopic, uint8_t sda, uint8_t scl,
                                             uint8_t address): ArrayInterface<T, LENGTH>(publishTopic, subscribeTopic) {
    setI2C(sda, scl);
    setAddress(address);
}


template<typename T, uint16_t LENGTH>
inline void I2CInterface<T, LENGTH>::init() {
    if (!this->_i2c) {
        _i2c = &Wire;
    }
    if (_sdaPin != 255 && _sclPin != 255) {
        _i2c->begin(_sdaPin, _sclPin);
    } else {
        _i2c->begin();
    }
    _i2c->setClock(I2C_CLOCK_SPEED);
    if (!isI2CDeviceWorking(_address)) {
        Logger.logf(FATAL, F("Device is not responding at address 0x%02X. %s interface shuts down!"), _address, this->_interfaceName.c_str());
        String devices = find_i2c_devices();
        Logger.logf(INFO, F("Following I2C devices are available: %s"), devices.c_str());
        this->setEnabled(false);
        return;
    }
}

template<typename T, uint16_t LENGTH>
inline void I2CInterface<T, LENGTH>::setI2C(uint8_t sda, uint8_t scl) {
    _sdaPin = sda;
    _sclPin = scl;
}


template<typename T, uint16_t LENGTH>
inline uint8_t I2CInterface<T, LENGTH>::i2cReadByte() {
    _i2c->requestFrom(this->_address, (uint8_t) 1);
    while (_i2c->available() < 1);
    return _i2c->read();
}

template<typename T, uint16_t LENGTH>
inline void I2CInterface<T, LENGTH>::i2cWriteByte(uint8_t data) {
    _i2c->beginTransmission(this->_address);
    _i2c->write(data);
    _i2c->endTransmission();
}

template<typename T, uint16_t LENGTH>
inline void I2CInterface<T, LENGTH>::i2cWrite2Bytes(uint8_t data1, uint8_t data2) {
    _i2c->beginTransmission(this->_address);
    _i2c->write(data1);
    _i2c->write(data2);
    _i2c->endTransmission();
}

template<typename T, uint16_t LENGTH>
inline void I2CInterface<T, LENGTH>::setI2C(TwoWire *i2c) {
    _i2c = i2c;
}

template<typename T, uint16_t LENGTH>
inline void I2CInterface<T, LENGTH>::setAddress(uint8_t address) {
    _address = address;
}

template<typename T, uint16_t LENGTH>
inline uint8_t I2CInterface<T, LENGTH>::getAddress() {
    return _address;
}

template<typename T, uint16_t LENGTH>
inline TwoWire *I2CInterface<T, LENGTH>::getWire() {
    return _i2c;
}
#endif
