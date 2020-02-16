#ifndef NODEMQTT_PCA9685INTERFACE_HPP
#define NODEMQTT_PCA9685INTERFACE_HPP


#include <Arduino.h>
#include <Wire.h>
#include "_I2CInterface.hpp"

// REGISTER ADDRESSES
#define PCA9685_MODE1 0x00      /**< Mode Register 1 */
#define PCA9685_MODE2 0x01      /**< Mode Register 2 */
#define PCA9685_SUBADR1 0x02    /**< I2C-bus subaddress 1 */
#define PCA9685_SUBADR2 0x03    /**< I2C-bus subaddress 2 */
#define PCA9685_SUBADR3 0x04    /**< I2C-bus subaddress 3 */
#define PCA9685_ALLCALLADR 0x05 /**< LED All Call I2C-bus address */
#define PCA9685_LED0_ON_L 0x06  /**< LED0 on tick, low byte*/
#define PCA9685_LED0_ON_H 0x07  /**< LED0 on tick, high byte*/
#define PCA9685_LED0_OFF_L 0x08 /**< LED0 off tick, low byte */
#define PCA9685_LED0_OFF_H 0x09 /**< LED0 off tick, high byte */
// etc all 16:  LED15_OFF_H 0x45
#define PCA9685_ALLLED_ON_L 0xFA  /**< load all the LEDn_ON registers, low */
#define PCA9685_ALLLED_ON_H 0xFB  /**< load all the LEDn_ON registers, high */
#define PCA9685_ALLLED_OFF_L 0xFC /**< load all the LEDn_OFF registers, low */
#define PCA9685_ALLLED_OFF_H 0xFD /**< load all the LEDn_OFF registers,high */
#define PCA9685_PRESCALE 0xFE     /**< Prescaler for PWM output frequency */
#define PCA9685_TESTMODE 0xFF     /**< defines the test mode to be entered */

// MODE1 bits
#define MODE1_ALLCAL 0x01  /**< respond to LED All Call I2C-bus address */
#define MODE1_SUB3 0x02    /**< respond to I2C-bus subaddress 3 */
#define MODE1_SUB2 0x04    /**< respond to I2C-bus subaddress 2 */
#define MODE1_SUB1 0x08    /**< respond to I2C-bus subaddress 1 */
#define MODE1_SLEEP 0x10   /**< Low power mode. Oscillator off */
#define MODE1_AI 0x20      /**< Auto-Increment enabled */
#define MODE1_EXTCLK 0x40  /**< Use EXTCLK pin clock */
#define MODE1_RESTART 0x80 /**< Restart enabled */
// MODE2 bits
#define MODE2_OUTNE_0 0x01 /**< Active LOW output enable input */
#define MODE2_OUTNE_1                                                          \
  0x02 /**< Active LOW output enable input - high impedience */
#define MODE2_OUTDRV 0x04 /**< totem pole structure vs open-drain */
#define MODE2_OCH 0x08    /**< Outputs change on ACK vs STOP */
#define MODE2_INVRT 0x10  /**< Output logic state inverted */

#define PCA9685_I2C_ADDRESS 0x40      /**< Default PCA9685 I2C Slave Address */
#define FREQUENCY_OSCILLATOR 25000000 /**< Int. osc. frequency in datasheet */

#define PCA9685_PRESCALE_MIN 3   /**< minimum prescale value */
#define PCA9685_PRESCALE_MAX 255 /**< maximum prescale value */

/*!
 *  @brief  Class that stores state and functions for interacting with PCA9685
 * PWM chip
 */
class PCA9685Interface  : public I2CInterface<uint8_t, LENGTH> {
public:
    PCA9685Interface();
    PCA9685Interface(const uint8_t addr);
    PCA9685Interface(const uint8_t addr, TwoWire &i2c);
    void begin(uint8_t prescale = 0);
    void reset();
    void sleep();
    void wakeup();
    void setExtClk(uint8_t prescale);
    void setPWMFreq(float freq);
    void setOutputMode(bool totempole);
    uint8_t getPWM(uint8_t num);
    void setPWM(uint8_t num, uint16_t on, uint16_t off);
    void setPin(uint8_t num, uint16_t val, bool invert = false);
    uint8_t readPrescale(void);
    void writeMicroseconds(uint8_t num, uint16_t Microseconds);

    void setOscillatorFrequency(uint32_t freq);
    uint32_t getOscillatorFrequency(void);

private:
    uint8_t _i2caddr;
    TwoWire *_i2c;

    uint32_t _oscillator_freq;
//    uint8_t read8(uint8_t addr);
//    void write8(uint8_t addr, uint8_t d);
};


/*!
 *  @brief  Instantiates a new PCA9685 PWM driver chip with the I2C address on a
 * TwoWire interface
 */
PCA9685Interface::PCA9685Interface()
        : _address(PCA9685_I2C_ADDRESS), _i2c(&Wire) {}

/*!
 *  @brief  Instantiates a new PCA9685 PWM driver chip with the I2C address on a
 * TwoWire interface
 *  @param  addr The 7-bit I2C address to locate this chip, default is 0x40
 */
PCA9685Interface::PCA9685Interface(const uint8_t addr)
        : _address(addr), _i2c(&Wire) {}

/*!
 *  @brief  Instantiates a new PCA9685 PWM driver chip with the I2C address on a
 * TwoWire interface
 *  @param  addr The 7-bit I2C address to locate this chip, default is 0x40
 *  @param  i2c  A reference to a 'TwoWire' object that we'll use to communicate
 *  with
 */
PCA9685Interface::PCA9685Interface(const uint8_t addr,
                                                 TwoWire &i2c)
        : _i2caddr(addr), _i2c(&i2c) {}

/*!
 *  @brief  Setups the I2C interface and hardware
 *  @param  prescale
 *          Sets External Clock (Optional)
 */
void PCA9685Interface::begin(uint8_t prescale) {
//    _i2c->begin();
I2CInterface::init();
    reset();
    if (prescale) {
        setExtClk(prescale);
    } else {
        // set a default frequency
        setPWMFreq(1000);
    }
    // set the default internal frequency
    setOscillatorFrequency(FREQUENCY_OSCILLATOR);
}

/*!
 *  @brief  Sends a reset command to the PCA9685 chip over I2C
 */
void PCA9685Interface::reset() {
    i2cWrite2Bytes(PCA9685_MODE1, MODE1_RESTART);
    delay(10);
}

/*!
 *  @brief  Puts board into sleep mode
 */
void PCA9685Interface::sleep() {
    uint8_t awake = read8(PCA9685_MODE1);
    uint8_t sleep = awake | MODE1_SLEEP; // set sleep bit high
    i2cWrite2Bytes(PCA9685_MODE1, sleep);
    delay(5); // wait until cycle ends for sleep to be active
}

/*!
 *  @brief  Wakes board from sleep
 */
void PCA9685Interface::wakeup() {
    uint8_t sleep = read8(PCA9685_MODE1);
    uint8_t wakeup = sleep & ~MODE1_SLEEP; // set sleep bit low
    i2cWrite2Bytes(PCA9685_MODE1, wakeup);
}

/*!
 *  @brief  Sets EXTCLK pin to use the external clock
 *  @param  prescale
 *          Configures the prescale value to be used by the external clock
 */
void PCA9685Interface::setExtClk(uint8_t prescale) {
    uint8_t oldmode = read8(PCA9685_MODE1);
    uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
    i2cWrite2Bytes(PCA9685_MODE1, newmode); // go to sleep, turn off internal oscillator

    // This sets both the SLEEP and EXTCLK bits of the MODE1 register to switch to
    // use the external clock.
    i2cWrite2Bytes(PCA9685_MODE1, (newmode |= MODE1_EXTCLK));

    i2cWrite2Bytes(PCA9685_PRESCALE, prescale); // set the prescaler

    delay(5);
    // clear the SLEEP bit to start
    i2cWrite2Bytes(PCA9685_MODE1, (newmode & ~MODE1_SLEEP) | MODE1_RESTART | MODE1_AI);

#ifdef ENABLE_DEBUG_OUTPUT
    Serial.print("Mode now 0x");
  Serial.println(read8(PCA9685_MODE1), HEX);
#endif
}

/*!
 *  @brief  Sets the PWM frequency for the entire chip, up to ~1.6 KHz
 *  @param  freq Floating point frequency that we will attempt to match
 */
void PCA9685Interface::setPWMFreq(float freq) {
#ifdef ENABLE_DEBUG_OUTPUT
    Serial.print("Attempting to set freq ");
  Serial.println(freq);
#endif
    // Range output modulation frequency is dependant on oscillator
    if (freq < 1)
        freq = 1;
    if (freq > 3500)
        freq = 3500; // Datasheet limit is 3052=50MHz/(4*4096)

    float prescaleval = ((_oscillator_freq / (freq * 4096.0)) + 0.5) - 1;
    if (prescaleval < PCA9685_PRESCALE_MIN)
        prescaleval = PCA9685_PRESCALE_MIN;
    if (prescaleval > PCA9685_PRESCALE_MAX)
        prescaleval = PCA9685_PRESCALE_MAX;
    uint8_t prescale = (uint8_t)prescaleval;

#ifdef ENABLE_DEBUG_OUTPUT
    Serial.print("Final pre-scale: ");
  Serial.println(prescale);
#endif

    uint8_t oldmode = read8(PCA9685_MODE1);
    uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
    i2cWrite2Bytes(PCA9685_MODE1, newmode);                             // go to sleep
    i2cWrite2Bytes(PCA9685_PRESCALE, prescale); // set the prescaler
    i2cWrite2Bytes(PCA9685_MODE1, oldmode);
    delay(5);
    // This sets the MODE1 register to turn on auto increment.
    i2cWrite2Bytes(PCA9685_MODE1, oldmode | MODE1_RESTART | MODE1_AI);

#ifdef ENABLE_DEBUG_OUTPUT
    Serial.print("Mode now 0x");
  Serial.println(read8(PCA9685_MODE1), HEX);
#endif
}

/*!
 *  @brief  Sets the output mode of the PCA9685 to either
 *  open drain or push pull / totempole.
 *  Warning: LEDs with integrated zener diodes should
 *  only be driven in open drain mode.
 *  @param  totempole Totempole if true, open drain if false.
 */
void PCA9685Interface::setOutputMode(bool totempole) {
    uint8_t oldmode = read8(PCA9685_MODE2);
    uint8_t newmode;
    if (totempole) {
        newmode = oldmode | MODE2_OUTDRV;
    } else {
        newmode = oldmode & ~MODE2_OUTDRV;
    }
    i2cWrite2Bytes(PCA9685_MODE2, newmode);
#ifdef ENABLE_DEBUG_OUTPUT
    Serial.print("Setting output mode: ");
  Serial.print(totempole ? "totempole" : "open drain");
  Serial.print(" by setting MODE2 to ");
  Serial.println(newmode);
#endif
}

/*!
 *  @brief  Reads set Prescale from PCA9685
 *  @return prescale value
 */
uint8_t PCA9685Interface::readPrescale(void) {
    return read8(PCA9685_PRESCALE);
}

/*!
 *  @brief  Gets the PWM output of one of the PCA9685 pins
 *  @param  num One of the PWM output pins, from 0 to 15
 *  @return requested PWM output value
 */
uint8_t PCA9685Interface::getPWM(uint8_t num) {
    _i2c->requestFrom((int)_i2caddr, PCA9685_LED0_ON_L + 4 * num, (int)4);
    return _i2c->read();
}

/*!
 *  @brief  Sets the PWM output of one of the PCA9685 pins
 *  @param  num One of the PWM output pins, from 0 to 15
 *  @param  on At what point in the 4096-part cycle to turn the PWM output ON
 *  @param  off At what point in the 4096-part cycle to turn the PWM output OFF
 */
void PCA9685Interface::setPWM(uint8_t num, uint16_t on, uint16_t off) {
#ifdef ENABLE_DEBUG_OUTPUT
    Serial.print("Setting PWM ");
  Serial.print(num);
  Serial.print(": ");
  Serial.print(on);
  Serial.print("->");
  Serial.println(off);
#endif

    _i2c->beginTransmission(_i2caddr);
    _i2c->write(PCA9685_LED0_ON_L + 4 * num);
    _i2c->write(on);
    _i2c->write(on >> 8);
    _i2c->write(off);
    _i2c->write(off >> 8);
    _i2c->endTransmission();
}

/*!
 *   @brief  Helper to set pin PWM output. Sets pin without having to deal with
 * on/off tick placement and properly handles a zero value as completely off and
 * 4095 as completely on.  Optional invert parameter supports inverting the
 * pulse for sinking to ground.
 *   @param  num One of the PWM output pins, from 0 to 15
 *   @param  val The number of ticks out of 4096 to be active, should be a value
 * from 0 to 4095 inclusive.
 *   @param  invert If true, inverts the output, defaults to 'false'
 */
void PCA9685Interface::setPin(uint8_t num, uint16_t val, bool invert) {
    // Clamp value between 0 and 4095 inclusive.
    val = min(val, (uint16_t)4095);
    if (invert) {
        if (val == 0) {
            // Special value for signal fully on.
            setPWM(num, 4096, 0);
        } else if (val == 4095) {
            // Special value for signal fully off.
            setPWM(num, 0, 4096);
        } else {
            setPWM(num, 0, 4095 - val);
        }
    } else {
        if (val == 4095) {
            // Special value for signal fully on.
            setPWM(num, 4096, 0);
        } else if (val == 0) {
            // Special value for signal fully off.
            setPWM(num, 0, 4096);
        } else {
            setPWM(num, 0, val);
        }
    }
}

/*!
 *  @brief  Sets the PWM output of one of the PCA9685 pins based on the input
 * microseconds, output is not precise
 *  @param  num One of the PWM output pins, from 0 to 15
 *  @param  Microseconds The number of Microseconds to turn the PWM output ON
 */
void PCA9685Interface::writeMicroseconds(uint8_t num,
                                                uint16_t Microseconds) {
#ifdef ENABLE_DEBUG_OUTPUT
    Serial.print("Setting PWM Via Microseconds on output");
  Serial.print(num);
  Serial.print(": ");
  Serial.print(Microseconds);
  Serial.println("->");
#endif

    double pulse = Microseconds;
    double pulselength;
    pulselength = 1000000; // 1,000,000 us per second

    // Read prescale
    uint16_t prescale = readPrescale();

#ifdef ENABLE_DEBUG_OUTPUT
    Serial.print(prescale);
  Serial.println(" PCA9685 chip prescale");
#endif

    // Calculate the pulse for PWM based on Equation 1 from the datasheet section
    // 7.3.5
    prescale += 1;
    pulselength *= prescale;
    pulselength /= _oscillator_freq;

#ifdef ENABLE_DEBUG_OUTPUT
    Serial.print(pulselength);
  Serial.println(" us per bit");
#endif

    pulse /= pulselength;

#ifdef ENABLE_DEBUG_OUTPUT
    Serial.print(pulse);
  Serial.println(" pulse for PWM");
#endif

    setPWM(num, 0, pulse);
}

/*!
 *  @brief  Getter for the internally tracked oscillator used for freq
 * calculations
 *  @returns The frequency the PCA9685 thinks it is running at (it cannot
 * introspect)
 */
uint32_t PCA9685Interface::getOscillatorFrequency(void) {
    return _oscillator_freq;
}

/*!
 *  @brief Setter for the internally tracked oscillator used for freq
 * calculations
 *  @param freq The frequency the PCA9685 should use for frequency calculations
 */
void PCA9685Interface::setOscillatorFrequency(uint32_t freq) {
    _oscillator_freq = freq;
}

/******************* Low level I2C interface */
uint8_t PCA9685Interface::read8(uint8_t addr) {
    _i2c->beginTransmission(_i2caddr);
    _i2c->write(addr);
    _i2c->endTransmission();

    _i2c->requestFrom((uint8_t)_i2caddr, (uint8_t)1);
    return _i2c->read();
}



#endif //NODEMQTT_PCA9685INTERFACE_HPP
