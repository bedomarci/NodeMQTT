#ifndef PWMINTERFACE_H
#define PWMINTERFACE_H
#include "DataInterface.hpp"
#include "../constants.hpp"

class PWMInterface : public DataInterface<int>
{
  public:
    PWMInterface(String topic, uint8_t pwmPin, bool invert = false);
    void write(int newValue, bool publishable = true);
    void fade(uint16_t fadeFrom, uint16_t fadeTo, uint16_t duration, uint16_t delayed = 0);
    void init();
    bool isFading();

  protected:
#if defined(ARDUINO_ARCH_ESP32)
    static uint8_t _PWMChannelCounter;
    uint8_t _currentPWMChannelCounter;
#endif
    bool _invert = false;
    uint8_t _pwmPin;

    uint16_t _fadeFrom;
    uint16_t _fadeTo;
    uint16_t _fadeIterations;

    void updatePhisicalInterface(int newValue);
    void pwmWrite(uint8_t pin, int value);
    void fadeCallback();
    Task tFade;
};

inline PWMInterface::PWMInterface(String topic, uint8_t pwmPin, bool invert)
    : DataInterface<int>(topic)
{
    _invert = invert;
    _pwmPin = pwmPin;
    _interfaceName = PWM_NAME;
    this->setSamplingEnabled(false);
    this->setMQTTPublish(false);
    this->setMQTTSubscribe(true);
    pinMode(pwmPin, OUTPUT);
    tFade.set(DEFAULT_FADE_INTERVAL, TASK_FOREVER, [this]() { fadeCallback(); });
#if defined(ARDUINO_ARCH_ESP32)
    _currentPWMChannelCounter = PWMInterface::_PWMChannelCounter++;
    ledcSetup(_currentPWMChannelCounter, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(_pwmPin, _currentPWMChannelCounter);
#endif
}

inline void PWMInterface::updatePhisicalInterface(int newValue)
{
    if (_invert)
    {
        this->pwmWrite(_pwmPin, PWMRANGE - (constrain(newValue, 0, PWMRANGE)));
    }
    else
    {
        this->pwmWrite(_pwmPin, constrain(newValue, 0, PWMRANGE));
    }
}
inline void PWMInterface::fade(uint16_t fadeFrom, uint16_t fadeTo, uint16_t duration, uint16_t delayed)
{
    _fadeFrom = constrain(fadeFrom, 0, PWMRANGE);
    _fadeTo = constrain(fadeTo, 0, PWMRANGE);
    _fadeIterations = (duration / (DEFAULT_FADE_INTERVAL));

    tFade.setIterations(_fadeIterations);
    tFade.restartDelayed(delayed);
}
inline void PWMInterface::fadeCallback()
{
    bool publish = false;
    int value = map(tFade.getRunCounter(), 1, _fadeIterations, _fadeFrom, _fadeTo);
    if (tFade.isLastIteration()){
        publish = true;
        value = _fadeTo;
    }
    NodeInterface<int>::write(value, publish);
}
inline void PWMInterface::write(int newValue, bool publishable)
{
    //Writing to the interface disables fade task.
    if (tFade.isEnabled())
        tFade.disable();
    NodeInterface<int>::write(newValue, publishable);
}
inline void PWMInterface::init()
{
    getScheduler()->addTask(tFade);
}

inline bool PWMInterface::isFading()
{
    return tFade.isEnabled();
}

inline void PWMInterface::pwmWrite(uint8_t pin, int value)
{
#if defined(ARDUINO_ARCH_ESP32)
    ledcWrite(_currentPWMChannelCounter, value);
#else
    analogWrite(pin, value);
#endif
}

#if defined(ARDUINO_ARCH_ESP32)
uint8_t PWMInterface::_PWMChannelCounter = 0;
#endif

#endif //PWMINTERFACE_H