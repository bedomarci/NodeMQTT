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

    void init();

    void setI2C(TwoWire *i2c);

    void setPins(uint8_t sda, uint8_t scl);

protected:
    TwoWire *_i2c;
    uint8_t _address;

    uint8_t i2cReadByte();

    void i2cWriteByte(uint8_t data);

    void i2cWrite2Bytes(uint8_t data1, uint8_t data2);

private:
    uint8_t _sdaPin;
    uint8_t _sclPin;
};

template<typename T, uint16_t LENGTH>
inline I2CInterface<T, LENGTH>::I2CInterface(String publishTopic, String subscribeTopic)
        : ArrayInterface<T, LENGTH>(publishTopic, subscribeTopic) {
}

template<typename T, uint16_t LENGTH>
inline void I2CInterface<T, LENGTH>::init() {
    if (!this->_i2c) {
        _i2c = &Wire;
        Wire.begin(_sdaPin, _sclPin);
    }
    Wire.setClock(I2C_CLOCK_SPEED);
    if (!isI2CDeviceWorking(_address)) {
        Logger.logf(FATAL, F("Device is not responding at address 0x%02X. Expander interface shuts down!"), _address);
        String devices = find_i2c_devices();
        Logger.logf(INFO, F("Following I2C devices are available: %s"), devices.c_str());
        this->setEnabled(false);
        return;
    }
}

template<typename T, uint16_t LENGTH>
inline void I2CInterface<T, LENGTH>::setPins(uint8_t sda, uint8_t scl) {
    _sdaPin = sda;
    _sclPin = scl;
}


template<typename T, uint16_t LENGTH>
inline uint8_t I2CInterface<T, LENGTH>::i2cReadByte() {
    Wire.requestFrom(this->_address, 1);
    while (Wire.available() < 1);
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
inline uint8_t I2CInterface<T, LENGTH>::setI2C(TwoWire *i2c) {
    _i2c = i2c;
}
#endif
