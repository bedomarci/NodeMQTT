#ifndef EXPANDERINTERFACE_H
#define EXPANDERINTERFACE_H

#include "ArrayInterface.hpp"
#include <FunctionalInterrupt.h>
#include "../misc/helpers.hpp"
#include "../NodeMQTTConfigManager.hpp"
#include <Wire.h>

#define I2CWRITE(x) Wire.write(x)
#define I2CREAD() Wire.read()

#define TRANSACTION_IMMEDIATE 0x00
#define TRANSACTION_SUSPENDED 0x01

#define SOFTPWM_UPDATE_FREQ_25HZ TASK_SECOND / 40
#define SOFTPWM_UPDATE_FREQ_50HZ TASK_SECOND / 20
#define SOFTPWM_UPDATE_FREQ_100HZ TASK_SECOND / 10

#define NO_INTERRUPT_PIN -1

#define ExpanderChangeCallbackSignature std::function<void(uint8_t, Array<uint8_t, LENGTH>, Array<uint8_t, LENGTH>)>

template <uint8_t LENGTH>
class ExpanderInterface : public ArrayInterface<uint8_t, LENGTH>
{
  public:
    ExpanderInterface(String topic, uint8_t sdaPin, uint8_t sclPin, uint8_t address, int8_t interruptPin = NO_INTERRUPT_PIN, uint16_t debounceDelay = DEFAULT_EXPANDER_DEBOUNCE, bool interruptPu = true);
    ExpanderInterface(String publishTopic, String subscribeTopic, uint8_t sdaPin, uint8_t sclPin, uint8_t address, int8_t interruptPin = NO_INTERRUPT_PIN, uint16_t debounceDelay = DEFAULT_EXPANDER_DEBOUNCE, bool interruptPu = true);
    void init();

    void begin(uint8_t address = 0x20);
    void pinMode(uint8_t pin, uint8_t mode, bool invert = false);
    void digitalWrite(uint8_t pin, uint8_t value, bool publishable = true);
    uint8_t digitalRead(uint8_t pin);
    void writeChip(uint16_t value);
    uint16_t readChip();
    void write(Array<uint8_t, LENGTH> newValue, bool publishable = true);
    void writePort(uint16_t newValue, bool publishable = true);
    void clear();
    void set();
    void toggle(uint8_t pin, bool publishable = true);
    void pullUp(uint8_t pin);
    void pullDown(uint8_t pin);
    void disableInterrupt();
    void checkForInterrupt();
    void attachPinInterrupt(uint8_t pin, ExpanderChangeCallbackSignature userFunc, uint8_t mode);
    void detachInterrupt(uint8_t pin);

    void startTransaction();
    void commitTransaction(bool publishable = true);
    void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
    void enableSoftwarePWM(bool enable = true);

  protected:
    Task _taskPoll;
    bool _hasInterrupt = false; //Has interrupt configured
    bool _interruptPullup;
    uint8_t static PCFInterruptGlobalCounter; //Indicates global interrupt
    bool static PCFInterruptAttached;
    uint8_t PCFInterruptLocalCounter = 0; //Indicates interrupt in each instance
    bool transactionState = TRANSACTION_IMMEDIATE;

    /** Output pins values */
    volatile uint16_t _PORT;
    /** Current input pins values */
    volatile uint16_t _PIN;
    /** Pins modes values (OUTPUT or INPUT) */
    volatile uint16_t _DDR;
    /** Pin invert map 0 = original, 1 = inverted*/
    volatile uint16_t _INV;

    volatile uint16_t _oldPIN;
    volatile uint8_t _isrIgnore;
    uint8_t _pcintPin;
    uint8_t _intMode[8];
    ExpanderChangeCallbackSignature _intCallback[LENGTH];
    virtual void readGPIO() = 0;
    virtual void updateGPIO() = 0;

    void poll();
    Array<uint8_t, LENGTH> sample();
    Array<uint8_t, LENGTH> binToArray(uint16_t value);
    void expanderInterruptCallback();

    uint8_t _address;
    uint8_t _sdaPin;
    uint8_t _sclPin;
    int8_t _interruptPin;
    int16_t _debounceDelay;
    void updatePhisicalInterface(Array<uint8_t, LENGTH>);

    //PWM
    bool softwarePWMEnabled = false;
    bool isBeginningOfDutyCycle = true;
    uint16_t pwmDutyCycle = SOFTPWM_UPDATE_FREQ_50HZ;
    uint8_t currentPWMValue = 0;
    uint8_t nextPWMValue = 0;
    // Task tSoftwarePWMCallback(TASK_IMMEDIATE, TASK_FOREVER, calculatePWM, 0, false);
    void calculatePWM();
};

template <uint8_t LENGTH>
inline ExpanderInterface<LENGTH>::ExpanderInterface(String topic, uint8_t sdaPin, uint8_t sclPin, uint8_t address, int8_t interruptPin, uint16_t debounceDelay, bool interruptPu)
    : ExpanderInterface(topic, topic, sdaPin, sclPin, address, interruptPin, debounceDelay)
{
}

template <uint8_t LENGTH>
inline ExpanderInterface<LENGTH>::ExpanderInterface(String publishTopic, String subscribeTopic, uint8_t sdaPin, uint8_t sclPin, uint8_t address, int8_t interruptPin, uint16_t debounceDelay, bool interruptPu)
    : ArrayInterface<uint8_t, LENGTH>(publishTopic, subscribeTopic)
{
    _sdaPin = sdaPin;
    _sclPin = sclPin;
    _address = address;
    _debounceDelay = debounceDelay;
    _interruptPin = interruptPin;
    _hasInterrupt = (_interruptPin != NO_INTERRUPT_PIN);
    _taskPoll.set(_debounceDelay, TASK_FOREVER, [this]() { poll(); });
    _interruptPullup = interruptPu;
    this->setMQTTPublish(true);
    this->setMQTTSubscribe(true);
}
template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::init()
{
    // if (!NodeMQTTConfigManagerClass::I2CInitialized)
    {
        Wire.begin(_sdaPin, _sclPin);
        Wire.setClock(I2C_CLOCK_SPEED);
        // NodeMQTTConfigManagerClass::I2CInitialized = true;
    }
    if (!isI2CDeviceWorking(_address))
    {
        Logger.logf(FATAL, F("Device is not responding at address 0x%02X. Expander interface shuts down!"), _address);
        this->setEnabled(false);
        return;
    }

    readGPIO();

    this->setSamplingEnabled(!_hasInterrupt);
    if (_hasInterrupt)
    {
        ::pinMode(_interruptPin, (_interruptPullup) ? INPUT_PULLUP : INPUT);
        this->getScheduler()->addTask(_taskPoll);
        _taskPoll.enable();
        if (!ExpanderInterface<LENGTH>::PCFInterruptAttached)
        {
            ::attachInterrupt(digitalPinToInterrupt(_interruptPin), [this]() { this->expanderInterruptCallback(); }, FALLING);
            ExpanderInterface<LENGTH>::PCFInterruptAttached = true;
        }
    }
}
template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::updatePhisicalInterface(Array<uint8_t, LENGTH> newValue)
{
    uint16_t data = 0;
    for (int i = 0; i < LENGTH; i++)
    {
        uint8_t value = newValue.item[i] & 1;
        if (value)
            data |= (1 << i);
    }
    data |= (~_DDR); //DO NOT WRITE BACK INPUT VALUE;
    this->writeChip(data);
}
template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::expanderInterruptCallback()
{
    PCFInterruptGlobalCounter++;
}

template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::poll()
{
    if (PCFInterruptGlobalCounter == PCFInterruptLocalCounter)
        return;
    PCFInterruptLocalCounter++;
    this->write(this->sample());
}
template <uint8_t LENGTH>
inline Array<uint8_t, LENGTH> ExpanderInterface<LENGTH>::sample()
{
    uint16_t data;
    data = this->readChip();
    data |= (_PORT & _DDR);
    return this->binToArray(data);
}

template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::pinMode(uint8_t pin, uint8_t mode, bool invert)
{
    switch (mode)
    {
    case INPUT:
        _DDR &= ~(1 << pin);
        _PORT &= ~(1 << pin);
        break;

    case INPUT_PULLUP:
        _DDR &= ~(1 << pin);
        _PORT |= (1 << pin);
        break;

    case OUTPUT:
        _DDR |= (1 << pin);
        _PORT &= ~(1 << pin);
        break;

    default:
        break;
    }

    if (invert)
        _INV |= (1 << pin);
    else
        _INV &= ~(1 << pin);

    updateGPIO();
    this->updatePhisicalInterface(this->read());
}
template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::digitalWrite(uint8_t pin, uint8_t value, bool publishable)
{
    Array<uint8_t, LENGTH> arr = this->read();
    arr[pin] = value;
    this->write(arr, publishable);
}
template <uint8_t LENGTH>
inline uint8_t ExpanderInterface<LENGTH>::digitalRead(uint8_t pin)
{
    readGPIO();
    /* Check for interrupt (manual detection) */
    checkForInterrupt();
    /* Read and return the pin state */
    return ((_PIN ^ _INV) & (1 << pin)) ? HIGH : LOW;
}
template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::write(Array<uint8_t, LENGTH> newValue, bool publishable)
{
    ArrayInterface<uint8_t, LENGTH>::write(newValue, publishable && (transactionState == TRANSACTION_IMMEDIATE));
}

template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::writePort(uint16_t newValue, bool publishable)
{
    Array<uint8_t, LENGTH> arr;
    array_fromInt32<uint8_t, LENGTH>(newValue, arr);
    this->write(arr, publishable);
}

template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::writeChip(uint16_t value)
{
    _PORT = value;
    updateGPIO();
}
template <uint8_t LENGTH>
inline uint16_t ExpanderInterface<LENGTH>::readChip()
{
    readGPIO();
    /* Check for interrupt (manual detection) */
    checkForInterrupt();
    return _PIN ^ _INV;
}
template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::pullUp(uint8_t pin)
{
    /* Same as pinMode(INPUT_PULLUP) */
    pinMode(pin, INPUT_PULLUP); // /!\ pinMode form THE LIBRARY
}
template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::pullDown(uint8_t pin)
{
    /* Same as pinMode(INPUT) */
    pinMode(pin, INPUT); // /!\ pinMode form THE LIBRARY
}
template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::clear()
{
    Array<uint8_t, LENGTH> arr;
    array_fill<uint8_t, LENGTH>(arr, 0);
    this->write(arr);
}
template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::set()
{
    Array<uint8_t, LENGTH> arr;
    array_fill<uint8_t, LENGTH>(arr, 1);
    this->write(arr);
}
template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::toggle(uint8_t pin, bool publishable)
{
    Array<uint8_t, LENGTH> arr = this->read();
    arr[pin] = ~(arr[pin] > 0) & 1;
    this->write(arr, publishable);
}

template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::checkForInterrupt()
{
    uint16_t invPin = _PIN ^ _INV;
    uint16_t invOldPin = _oldPIN ^ _INV;
    Array<uint8_t, LENGTH> oldValue, newValue;
    oldValue = this->binToArray(invOldPin);
    newValue = this->binToArray(invPin);

    if (_isrIgnore)
        return;
    else
        _isrIgnore = 1;
    /* Re-enable interrupts to allow Wire library to work */
    sei();
    for (uint8_t i = 0; i < LENGTH; ++i)
    {
        /* Check for interrupt handler */
        if (!_intCallback[i])
            continue;

        /* Check for interrupt event */
        switch (_intMode[i])
        {
        case CHANGE:
            if ((1 << i) & (invPin ^ invOldPin))
                _intCallback[i](i, oldValue, newValue);
            break;
        case LOW:
            if (!(invPin & (1 << i)))
                _intCallback[i](i, oldValue, newValue);
            break;
        case FALLING:
            if ((invOldPin & (1 << i)) && !(invPin & (1 << i)))
                _intCallback[i](i, oldValue, newValue);
            break;
        case RISING:
            if (!(invOldPin & (1 << i)) && (invPin & (1 << i)))
                _intCallback[i](i, oldValue, newValue);
            break;
        }
    }
    _isrIgnore = 0;
}
template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::attachPinInterrupt(uint8_t pin, ExpanderChangeCallbackSignature userFunc, uint8_t mode)
{
    _intMode[pin] = mode;
    _intCallback[pin] = userFunc;
}
template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::detachInterrupt(uint8_t pin)
{
    _intCallback[pin] = 0;
}

template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::calculatePWM()
{
    currentPWMValue = nextPWMValue;
    uint16_t value = 0xFFFF;
    //
    for (uint8_t i = 0; i < LENGTH; i++)
    {
        if (this->currentValue[i] <= currentPWMValue) //if value is currentvalue
            value &= ~(1 << i);                       //write low in pin
    }

    //calculation for next PWM value this duty cycle
    nextPWMValue = 0xFF;
    for (int i = 0; i < LENGTH; i++)
    {
        if (this->currentValue[i] < nextPWMValue && nextPWMValue > currentPWMValue)
        {
            nextPWMValue = this->currentValue[i];
        }
    }
    //Start new duty cycle if no higher PWM value
    if (nextPWMValue == currentPWMValue or nextPWMValue == 0xFF)
    {
        nextPWMValue = 0x00;
    }
    //Calculate next callback
    uint16_t nextCall;
    nextCall = (nextPWMValue == 0x00)
                   ? 256 - currentPWMValue
                   : nextPWMValue - currentPWMValue;

    // tSoftwarePWMCallback.setInterval(pwmDutyCycle * nextCall / 0xFF);
    writeChip(value);
}

template <uint8_t LENGTH>
inline Array<uint8_t, LENGTH> ExpanderInterface<LENGTH>::binToArray(uint16_t value)
{
    Array<uint8_t, LENGTH> arr;
    for (int i = 0; i < LENGTH; i++)
    {
        arr[i] = bitRead(value, i);
    }
    return arr;
}

template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::startTransaction()
{
    transactionState = TRANSACTION_SUSPENDED;
}

template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::commitTransaction(bool publishable)
{
    transactionState = TRANSACTION_IMMEDIATE;
    if (publishable)
        this->publish(this->read());
}
template <uint8_t LENGTH>
inline void ExpanderInterface<LENGTH>::shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
{
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
        if (bitOrder == LSBFIRST)
            this->digitalWrite(dataPin, !!(val & (1 << i)), false);
        else
            this->digitalWrite(dataPin, !!(val & (1 << (7 - i))), false);

        this->digitalWrite(clockPin, HIGH, false);
        this->digitalWrite(clockPin, LOW, false);
    }
}

template <uint8_t LENGTH>
uint8_t ExpanderInterface<LENGTH>::PCFInterruptGlobalCounter = 0;
template <uint8_t LENGTH>
bool ExpanderInterface<LENGTH>::PCFInterruptAttached = false;

#endif //EXPANDERINTERFACE_H