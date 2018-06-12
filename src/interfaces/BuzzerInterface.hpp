#ifndef BUZZERINTERFACE_H
#define BUZZERINTERFACE_H

#include "DataInterface.hpp"
#include "misc/pitches.hpp"

#define PLAYER_CALLBACK [this]() { notePlayerCallback(); }
#define STOP_CALLBACK [this]() { noteStopCallback(); }
#define DISABLE_CALLBACK [this]() { noteDisableCallback(); }

#define TONE_COUNT 10

struct Note
{
    uint16_t duration;
    uint16_t frequency;
    uint16_t pause;
};

const Note infoTone[] PROGMEM = {{300, NOTE_C5, 0}};                                                                                                 //1
const Note successTone[] PROGMEM = {{150, NOTE_C5, 30}, {150, NOTE_E5, 30}, {150, NOTE_G5, 30}, {500, NOTE_C6, 0}};                                  //4
const Note warningTone[] PROGMEM = {{100, NOTE_C6, 0}, {100, NOTE_F6, 0}};                                                                           //2
const Note errorTone[] PROGMEM = {{100, NOTE_C4, 0}, {100, NOTE_F4, 0}, {100, NOTE_C4, 0}, {100, NOTE_F4, 0}, {100, NOTE_C4, 0}, {100, NOTE_F4, 0}}; //6
const Note alertTone[] PROGMEM = {{100, NOTE_C6, 0}, {100, NOTE_F6, 0}, {100, NOTE_C6, 0}, {100, NOTE_F6, 0}, {100, NOTE_C6, 0}};                    //5
const Note failTone[] PROGMEM = {{300, NOTE_C6, 30}, {300, NOTE_G5, 30}, {300, NOTE_E5, 30}, {500, NOTE_C5, 0}};                                     //4
const Note happyTone[] PROGMEM = {{100, NOTE_C4, 0}, {100, NOTE_F4, 0}, {100, NOTE_C4, 0}, {100, NOTE_F4, 0}, {100, NOTE_C4, 0}};                    //5
const Note sadTone[] PROGMEM = {{100, NOTE_C4, 0}, {100, NOTE_F4, 0}, {100, NOTE_C4, 0}, {100, NOTE_F4, 0}, {100, NOTE_C4, 0}};                      //5

const Note systemBoot[] PROGMEM = {{100, NOTE_C6, 0}};   // 1 ,{100,NOTE_E5,0}}; //2
const Note systemOnline[] PROGMEM = {{100, NOTE_G6, 0}}; // 1 ,{100,NOTE_C6,0}}; //2

class BuzzerInterface : public DataInterface<int>
{
  public:
    BuzzerInterface(String topic, uint8_t buzzerPin);
    void init();

  protected:
    uint8_t _noteCounter = 0;
    Note _currentNote;
    Note *_noteBuffer;
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

    const Note *notePtrArray[TONE_COUNT] = {infoTone, successTone, warningTone, errorTone, alertTone, failTone, happyTone, sadTone, systemBoot, systemOnline};
    const uint8_t noteLengthArray[TONE_COUNT] = {1, 4, 2, 6, 5, 4, 5, 5, 1, 1};
};

inline BuzzerInterface::BuzzerInterface(String topic, uint8_t buzzerPin) : DataInterface<int>(topic)
{
    _buzzerPin = buzzerPin;
    setSamplingEnabled(false);
    setMQTTPublish(false);
    pinMode(buzzerPin, OUTPUT);
    _noteTask.set(TASK_IMMEDIATE, TASK_FOREVER, PLAYER_CALLBACK, 0, DISABLE_CALLBACK);
    _interfaceName = BUZZER_NAME;
}

inline void BuzzerInterface::init()
{
    _scheduler->addTask(_noteTask);
}

inline void BuzzerInterface::updatePhisicalInterface(int toneId)
{
    if (abs(toneId) >= TONE_COUNT)
        toneId = 0;
    if (_noteTask.isEnabled())
    {
        _noteTask.disable();
    }
    if (fetchTone(toneId))
    {
        _noteTask.restart();
    }
    else
    {
        noteDisableCallback();
    }
}

inline void BuzzerInterface::notePlayerCallback()
{
    if (_noteCounter >= _noteBufferLength)
    {
        _noteTask.disable();
        return;
    }
    _currentNote = _noteBuffer[_noteCounter];
#ifdef ESP8266
    tone(_buzzerPin, _currentNote.frequency, _currentNote.duration);
#endif
#ifdef ESP32
    //not implemented yet
#endif
    _noteTask.setInterval(_currentNote.duration);
    _noteTask.setCallback(STOP_CALLBACK);
    _noteCounter++;
}

inline void BuzzerInterface::noteStopCallback()
{

#ifdef ESP8266
    noTone(_buzzerPin);
#endif
#ifdef ESP32
    //not implemented yet
#endif
    _noteTask.setInterval(_currentNote.pause);
    _noteTask.setCallback(PLAYER_CALLBACK);
}
inline void BuzzerInterface::noteDisableCallback()
{
    _noteCounter = 0;
    free(_noteBuffer);
}

inline bool BuzzerInterface::fetchTone(int toneId)
{
    _noteBufferLength = noteLengthArray[toneId];
    _noteBuffer = new Note[_noteBufferLength];
    if (_noteBuffer)
    {
        memcpy_P(_noteBuffer, notePtrArray[toneId], _noteBufferLength * sizeof(struct Note));
    }
    return _noteBuffer;
}

inline int BuzzerInterface::cmp(int oldValue, int newValue)
{
    return -1;
}

inline int BuzzerInterface::sample()
{
    return currentValue;
}

#endif //BUZZERINTERFACE_H