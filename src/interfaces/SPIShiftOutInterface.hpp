#ifndef NODEMQTT_SPISHIFTOUTINTERFACE_HPP
#define NODEMQTT_SPISHIFTOUTINTERFACE_HPP

#include <SPI.h>
#include "ArrayInterface.hpp"

template<uint8_t LENGTH>
class SPIShiftOutInterface : public ArrayInterface<uint16_t, LENGTH> {
public:
    SPIShiftOutInterface(String publishTopic, String subscribeTopic, uint8_t latch);

    SPIShiftOutInterface(String publishTopic, String subscribeTopic, uint8_t latch, SPIClass *spi);

    void init();

    void fade(uint8_t pin, uint16_t fadeFrom, uint16_t fadeTo, uint16_t duration, uint16_t delayed = 0);

    void setAnalogPin(uint8_t pin, bool analog);

    void setInvertPin(uint8_t pin, bool invert);

    void analogWrite(uint8_t pin, uint16_t value, bool publishable = true);

    void digitalWrite(uint8_t pin, uint8_t value, bool publishable = true);

protected:

    void updatePhisicalInterface(Array<uint16_t, LENGTH>) override;

private:
    SPIClass *_spi;
    uint8_t _latch;

    uint16_t _fadeFrom[LENGTH];
    uint16_t _fadeTo[LENGTH];
    uint16_t _fadeIterations[LENGTH];

    bool analogPinMap[LENGTH];
    uint8_t invertPinMap[LENGTH];

    void fadeCallback(uint8_t pin);

    SPISettings *settings;

    Task *tFade[LENGTH];
};


template<uint8_t LENGTH>
inline SPIShiftOutInterface<LENGTH>::SPIShiftOutInterface(String publishTopic, String subscribeTopic, uint8_t latch)
        : SPIShiftOutInterface<LENGTH>(publishTopic, subscribeTopic, latch, &SPI) {

}

template<uint8_t LENGTH>
inline SPIShiftOutInterface<LENGTH>::SPIShiftOutInterface(String publishTopic, String subscribeTopic, uint8_t latch, SPIClass *spi)
        : ArrayInterface<uint16_t, LENGTH>(publishTopic, subscribeTopic) {
    _spi = spi;
    _latch = latch;
    ::pinMode(_latch, OUTPUT);
    settings = new SPISettings(2000000, MSBFIRST, SPI_MODE1);
    this->setSamplingEnabled(false);
    for (int i = 0; i < LENGTH; i++) {
        tFade[i] = new Task(DEFAULT_FADE_INTERVAL, TASK_FOREVER, [this, i]() { fadeCallback(i); });
    }

}

template<uint8_t LENGTH>
inline void SPIShiftOutInterface<LENGTH>::init() {
    SPI.begin();
}


template<uint8_t LENGTH>
inline void SPIShiftOutInterface<LENGTH>::updatePhisicalInterface(Array<uint16_t, LENGTH> newValue) {
//    SPI.beginTransaction(*settings);
    uint8_t bytes =  ceil(LENGTH / 8);
    ::digitalWrite(_latch, LOW);
    for (int byteCounter = 0; byteCounter < bytes; byteCounter++) {
        uint8_t byte = 0;
        for (int bitCounter = 0; bitCounter < 8; bitCounter++) {
            uint8_t currentBit = byteCounter * 8 + bitCounter;
            byte |= (!!newValue[currentBit] ^ invertPinMap[currentBit]) << bitCounter;
        }
        Serial.println(byte);
        shiftOut(D5, D7, MSBFIRST, byte);
//        _spi->transfer(byte);
    }
    ::digitalWrite(_latch, HIGH);
//    SPI.endTransaction();
}

template<uint8_t LENGTH>
inline void SPIShiftOutInterface<LENGTH>::setAnalogPin(uint8_t pin, bool analog) {
    analogPinMap[constrain(pin, 0, LENGTH - 1)] = analog;
}

template<uint8_t LENGTH>
inline void SPIShiftOutInterface<LENGTH>::setInvertPin(uint8_t pin, bool invert) {
    invertPinMap[constrain(pin, 0, LENGTH - 1)] = invert;
}

template<uint8_t LENGTH>
inline void SPIShiftOutInterface<LENGTH>::analogWrite(uint8_t pin, uint16_t value, bool publishable) {
    this->setAnalogPin(pin, true);
    this->writeItem(pin, value, publishable);
}

template<uint8_t LENGTH>
inline void SPIShiftOutInterface<LENGTH>::digitalWrite(uint8_t pin, uint8_t value, bool publishable) {
    this->setAnalogPin(pin, false);
    this->writeItem(pin, value, publishable);
}

template<uint8_t LENGTH>
inline void SPIShiftOutInterface<LENGTH>::fade(uint8_t pin, uint16_t fadeFrom, uint16_t fadeTo, uint16_t duration, uint16_t delayed) {
    _fadeFrom[pin] = constrain(fadeFrom, 0, PWMRANGE);
    _fadeTo[pin] = constrain(fadeTo, 0, PWMRANGE);
    _fadeIterations[pin] = (duration / (DEFAULT_FADE_INTERVAL));

    tFade[pin]->setIterations(_fadeIterations);
    tFade[pin]->restartDelayed(delayed);
}

template<uint8_t LENGTH>
inline void SPIShiftOutInterface<LENGTH>::fadeCallback(uint8_t pin) {
    bool publish = false;
    int value = map(tFade[pin]->getRunCounter(), 1, _fadeIterations[pin], _fadeFrom[pin], _fadeTo[pin]);
    if (tFade[pin]->isLastIteration())
        publish = true;
    this->analogWrite(pin, value, publish);
}

#endif //NODEMQTT_SPISHIFTOUTINTERFACE_HPP
