#ifndef HELPERS_H
#define HELPERS_H
#include <Arduino.h>
#include <Wire.h>
#include "NodeMQTTLogger.hpp"

#define d(x) Logger.log(DEBUG, x)
#define debug(x) Logger.log(DEBUG, x)

#define i(x) Logger.log(INFO, x)
#define info(x) Logger.log(INFO, x)

#define w(x) Logger.log(WARNING, x)
#define warning(x) Logger.log(WARNING, x)

#define e(x) Logger.log(ERROR, x)
#define error(x) Logger.log(ERROR, x)

#define fatal(x) Logger.log(FATAL, x)

//DEVICE NAME
#ifdef ESP8266
#define DEVICE_NAME String(ESP.getChipId(), HEX)
#endif
#ifdef ESP32
#define DEVICE_NAME String((uint32_t)ESP.getEfuseMac(), HEX)
#endif
// inline String getMQTTDeviceName()
// {
//     return ;
// }

inline void pinModes(uint8_t *pinArray, uint8_t length, uint8_t mode)
{
    for (int i = 0; i < length; i++)
    {
        pinMode(pinArray[i], mode);
    }
}

inline bool isI2CDeviceWorking(uint8_t address)
{
    Wire.beginTransmission(address);
    int error = Wire.endTransmission();
    return error == 0;
}

template <typename T, uint16_t LENGTH>
inline void array_shiftRight(Array<T, LENGTH> &array, uint8_t shift)
{
    if (shift < 1)
        return;
    for (uint8_t s = 0; s < shift; s++)
    {
        T temp = array.item[LENGTH - 1];
        for (uint16_t i = LENGTH - 1; i > 0; i--)
        {
            array.item[i] = array.item[i - 1];
        }
        array.item[0] = temp;
    }
}

template <typename T, uint16_t LENGTH>
inline void array_shiftLeft(Array<T, LENGTH> &array, uint8_t shift)
{
    if (shift < 1)
        return;

    for (uint8_t s = 0; s < shift; s++)
    {
        T temp = array.item[0];
        for (uint16_t i = 0; i < LENGTH - 2; i++)
        {
            array.item[i] = array.item[i + 1];
        }
        array.item[LENGTH - 1] = temp;
    }
}

template <typename T, uint16_t LENGTH>
inline void array_fill(Array<T, LENGTH> &array, T value)
{
    for (uint16_t i = 0; i < LENGTH; i++)
    {
        array.item[i] = value;
    }
}

template <typename T, uint16_t LENGTH>
inline String array_toString(Array<T, LENGTH> &array)
{
    String str = "[";
    for (uint16_t i = 0; i < LENGTH; i++)
    {
        str += String(array.item[i]);
        if (i < LENGTH - 1)
            str += ", ";
    }
    str += "]";
    return str;
}

template <typename T, uint16_t LENGTH>
inline void array_fromInt32(uint32_t newValue, Array<T, LENGTH> &array, uint8_t bitOrder = MSBFIRST)
{
    if (LENGTH > 32)
        return;

    // val = val & ~(0xFFFF << LENGTH);

    for (int i = 0; i < LENGTH; i++)
    {
        uint8_t index = (bitOrder == MSBFIRST) ? (LENGTH - 1) - i : i;
        array[i] = bitRead(newValue, index);
    }
}

template <typename T, uint16_t LENGTH>
inline void array_print(Array<T, LENGTH> &array)
{
    Serial.println(array_toString<T, LENGTH>(array));
}

template <typename T, uint16_t LENGTH>
inline void array_push(Array<T, LENGTH> &array, T value)
{
    array_shiftRight<T, LENGTH>(array, 1);
    array[0] = value;
}

template <typename T, uint16_t LENGTH>
inline T array_min(Array<T, LENGTH> &array)
{
    T min;
    min = array[0];
    for (int i = 1; i < LENGTH; i++)
    {
        if (min > array[i])
            min = array[i];
    }
    return min;
}

template <typename T, uint16_t LENGTH>
inline T array_max(Array<T, LENGTH> &array)
{
    T max;
    max = array[0];
    for (int i = 1; i < LENGTH; i++)
    {
        if (max < array[i])
            max = array[i];
    }
    return min;
}

template <typename T, uint16_t LENGTH>
inline int array_sum(Array<T, LENGTH> &array)
{
    int sum = 0;
    for (int i = 1; i < LENGTH; i++)
    {
        sum += array[i];
    }
    return sum;
}

#endif