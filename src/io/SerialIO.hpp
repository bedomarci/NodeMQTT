#ifndef SERIALIO_H
#define SERIALIO_H

#include "_AbstractIO.hpp"
#include "misc/helpers.hpp"

class SerialIO : public AbstractIO
{
    public:
        SerialIO();
        size_t write(uint8_t c);
    protected:
        void handle();
        void printHeader();
    private:
        char *_serialBuffer = new char[SERIAL_BUFFER_SIZE];
        uint16_t index = 0;
        bool terminated = false;
        
};
inline SerialIO::SerialIO() {
    Serial.begin(NODEMQTT_SERIAL_SPEED);
    //flushing input buffer
    if (Serial.available() > 0) {
        Serial.read();
    }
}

inline void SerialIO::handle(){
    while(Serial.available() > 0)
    {
        char c = Serial.read();
        if (c != '\r' && c != '\n') { //if the incoming character is not the byte that is the incoming package ender
            _serialBuffer[index] = c; //the buffer position in the array get assigned to the current read
            index++; //once that has happend the buffer advances, doing this over and over again until the end of package marker is read.
        } else {
            _serialBuffer[index] = '\0';
            terminated = true;
        }
    }
    if (terminated) {
        if (index > 1)
            receive(_serialBuffer);
        index = 0;
        terminated = false;
        free(_serialBuffer);
        _serialBuffer = new char[SERIAL_BUFFER_SIZE];
    }
}

inline void SerialIO::printHeader(){

}

inline size_t SerialIO::write(uint8_t c) {
    return Serial.write(c);
}

#endif //SERIALIO_H