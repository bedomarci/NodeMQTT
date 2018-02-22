#include "BuzzerInterface.hpp"

#define PLAYER_CALLBACK [this](){notePlayerCallback();}
#define STOP_CALLBACK [this](){noteStopCallback();}
#define DISABLE_CALLBACK [this](){noteDisableCallback();}


BuzzerInterface::BuzzerInterface(String topic, uint8_t buzzerPin) : IntegerInterface(topic, topic) {
    _buzzerPin = buzzerPin;
    samplingEnabled = false;
    hasMQTTPublish = false;
    pinMode(buzzerPin, OUTPUT);
    _noteTask.set(TASK_IMMEDIATE,TASK_FOREVER,PLAYER_CALLBACK,0,DISABLE_CALLBACK);
    interfaceName = "bzr";
}

void BuzzerInterface::init() {
    _scheduler->addTask(_noteTask);
}

void BuzzerInterface::updatePhisicalInterface(int toneId)
{
    if (abs(toneId) >= TONE_COUNT) toneId = 0;
    if(_noteTask.isEnabled()) {
        _noteTask.disable();
    }
    if (fetchTone(toneId)) {
        _noteTask.restart();
    } else {
        noteDisableCallback();
    }
}

void BuzzerInterface::notePlayerCallback()
{
    if (_noteCounter>=_noteBufferLength)
    {
        _noteTask.disable();
        return;
    }
    _currentNote = _noteBuffer[_noteCounter];
    tone(_buzzerPin, _currentNote.frequency, _currentNote.duration);
    _noteTask.setInterval(_currentNote.duration);
    _noteTask.setCallback(STOP_CALLBACK);
    _noteCounter++;
}

void BuzzerInterface::noteStopCallback() {
    noTone(_buzzerPin);
    _noteTask.setInterval(_currentNote.pause);
    _noteTask.setCallback(PLAYER_CALLBACK);
}
void BuzzerInterface::noteDisableCallback(){
    _noteCounter = 0;
    free(_noteBuffer);
}

bool BuzzerInterface::fetchTone(int toneId) {
    _noteBufferLength = noteLengthArray[toneId];
    _noteBuffer = new Note[_noteBufferLength];
    if (_noteBuffer) {
      memcpy_P(_noteBuffer, notePtrArray[toneId], _noteBufferLength*sizeof(struct Note));
    }
    return _noteBuffer;
}

int BuzzerInterface::cmp(int oldValue, int newValue){
    return -1;
}

int BuzzerInterface::sample() {
    return currentValue;
}


