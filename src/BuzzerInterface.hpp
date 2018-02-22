#ifndef BUZZERINTERFACE_H
#define BUZZERINTERFACE_H

#include "_IntegerInterface.hpp"
#include "misc/pitches.hpp"




#define TONE_COUNT 10

struct Note {
    uint16_t duration;
    uint16_t frequency;
    uint16_t pause;
};


const Note infoTone[]     PROGMEM = {{300,NOTE_C5,0}}; //1
const Note successTone[]  PROGMEM = {{150,NOTE_C5,30},{150,NOTE_E5,30},{150,NOTE_G5,30},{500,NOTE_C6,0}}; //4
const Note warningTone[]  PROGMEM = {{100,NOTE_C6,0},{100,NOTE_F6,0}}; //2
const Note errorTone[]    PROGMEM = {{100,NOTE_C4,0},{100,NOTE_F4,0},{100,NOTE_C4,0},{100,NOTE_F4,0},{100,NOTE_C4,0},{100,NOTE_F4,0}}; //6
const Note alertTone[]    PROGMEM = {{100,NOTE_C6,0},{100,NOTE_F6,0},{100,NOTE_C6,0},{100,NOTE_F6,0},{100,NOTE_C6,0}}; //5
const Note failTone[]     PROGMEM = {{300,NOTE_C6,30},{300,NOTE_G5,30},{300,NOTE_E5,30},{500,NOTE_C5,0}};//4
const Note happyTone[]    PROGMEM = {{100,NOTE_C4,0},{100,NOTE_F4,0},{100,NOTE_C4,0},{100,NOTE_F4,0},{100,NOTE_C4,0}}; //5
const Note sadTone[]      PROGMEM = {{100,NOTE_C4,0},{100,NOTE_F4,0},{100,NOTE_C4,0},{100,NOTE_F4,0},{100,NOTE_C4,0}}; //5


const Note systemBoot[]   PROGMEM = {{100,NOTE_C6,0}};// 1 ,{100,NOTE_E5,0}}; //2
const Note systemOnline[] PROGMEM = {{100,NOTE_G6,0}};// 1 ,{100,NOTE_C6,0}}; //2


class BuzzerInterface : public IntegerInterface {
    public:
        BuzzerInterface(String topic, uint8_t buzzerPin);
        void init();
    protected:
        uint8_t _noteCounter = 0;
        Note _currentNote;
        Note * _noteBuffer;
        uint8_t _noteBufferLength = 0;
        bool _isPlaying = false;
        Task _noteTask;

        uint8_t _buzzerPin;
        int sample();
        void updatePhisicalInterface(int newValue);
        int cmp(int oldValue, int newValue);
        void notePlayerCallback();
        void noteStopCallback();
        void noteDisableCallback();
        bool fetchTone(int tone);


        const Note* notePtrArray [TONE_COUNT] = {infoTone,successTone,warningTone,errorTone,alertTone,failTone,happyTone,sadTone,systemBoot,systemOnline};
        const uint8_t noteLengthArray[TONE_COUNT] = {1,4,2,6,5,4,5,5,1,1};
};

#endif //BUZZERINTERFACE_H