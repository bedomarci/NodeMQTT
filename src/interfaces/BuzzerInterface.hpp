#ifndef BUZZERINTERFACE_H
#define BUZZERINTERFACE_H

#include "DataInterface.hpp"

#define PLAYER_CALLBACK [this]() { notePlayerCallback(); }
#define STOP_CALLBACK [this]() { noteStopCallback(); }
#define DISABLE_CALLBACK [this]() { noteDisableCallback(); }

#define TONE_COUNT 10

class BuzzerInterface : public DataInterface<int>
{
  public:
    BuzzerInterface(String topic, uint8_t buzzerPin);
    void init();
    void valueToString(String &sValue);

  protected:
    uint8_t _noteCounter = 0;
    Note _currentNote;
    Note *_noteBuffer;
    uint8_t _noteBufferLength = 0;
    bool _isPlaying = false;
    Task _noteTask;

    uint8_t _buzzerPin;
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
    getScheduler()->addTask(_noteTask);
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

inline void BuzzerInterface::valueToString(String &sValue)
{
    sValue = String(this->read());
}

#endif //BUZZERINTERFACE_H