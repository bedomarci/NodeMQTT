#ifndef PCF8574INTERFACE_H
#define PCF8574INTERFACE_H
#include "ExpanderInterface.hpp"
#include <FunctionalInterrupt.h>
#include <Wire.h>

class PCF8574Interface : public ExpanderInterface<8U>
{
  public:
    PCF8574Interface(String topic, uint8_t sdaPin, uint8_t sclPin, uint8_t address, int8_t interruptPin, uint16_t debounceDelay, bool interruptPu = true);
    PCF8574Interface(String publishTopic, String subscribeTopic, uint8_t sdaPin, uint8_t sclPin, uint8_t address, int8_t interruptPin, uint16_t debounceDelay, bool interruptPu = true);

  protected:
    void readGPIO();
    void updateGPIO();
};

inline PCF8574Interface::PCF8574Interface(String topic, uint8_t sdaPin, uint8_t sclPin, uint8_t address, int8_t interruptPin = -1, uint16_t debounceDelay = DEFAULT_BUTTON_DEBOUNCE, bool interruptPu)
    : PCF8574Interface(topic, topic, sdaPin, sclPin, address, interruptPin, debounceDelay, interruptPu)
{
}

inline PCF8574Interface::PCF8574Interface(String publishTopic, String subscribeTopic, uint8_t sdaPin, uint8_t sclPin, uint8_t address, int8_t interruptPin = -1, uint16_t debounceDelay = DEFAULT_BUTTON_DEBOUNCE, bool interruptPu)
    : ExpanderInterface<8U>(publishTopic, subscribeTopic, sdaPin, sclPin, address, interruptPin, debounceDelay, interruptPu)
{
    this->setInterfaceName(PCF8574_NAME);
}

void PCF8574Interface::readGPIO()
{
    /* Store old _PIN value */
    this->_oldPIN = this->_PIN;
    /* Start request, wait for data and receive GPIO values as byte */
    Wire.requestFrom(this->_address, (uint8_t)0x01);
    while (Wire.available() < 1)
        ;
    _PIN = I2CREAD();
}

void PCF8574Interface::updateGPIO()
{
    /* Compute new GPIO states */
    uint16_t value = (this->_PIN & ~this->_DDR) | this->_PORT;

    /* Start communication and send GPIO values as byte */
    Wire.beginTransmission(this->_address);
    I2CWRITE(value);
    Wire.endTransmission();
}

#endif //PCF8574INTERFACE_H