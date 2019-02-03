#ifndef DDFPLAYERINTERFACE_H
#define DDFPLAYERINTERFACE_H
#include "_NodeInterface.hpp"

#ifdef ESP8266
#include "SoftwareSerial.h"
#endif
#ifdef ESP32
#include "HardwareSerial.h"
#endif

#include "DFRobotDFPlayerMini.h"

#define READ_STATE_CALLBACK [this]() { readStateCallback(); }

#define STATUS_PULL_RATE 132
#define DEFAULT_SERIAL_TIMEOUT 500
#define SOFTWARE_SERIAL_PAUSE 50

#define VOLUME_MAX 30

enum DFPlayerCommandTask
{
    NEXT = 0,
    PREVIOUS = 1,
    PLAY = 2,
    LOOP = 3,
    PAUSE = 4,
    START = 5,
    STOP = 6,
    PLAYFOLDER = 7,
    VOLUME = 8,
};

struct DFPlayerCommand
{
    DFPlayerCommandTask task;
    uint8_t param1;
    uint8_t param2;
};

class DFPlayerInterface : public NodeInterface<DFPlayerCommand>
{
  public:
    DFPlayerInterface(String publishTopic, String subscribeTopic, uint8_t serialRxPin = 0, uint8_t serialTxPin = 0);
    DFPlayerInterface(String topic, uint8_t serialRxPin = 0, uint8_t serialTxPin = 0);
    void init();
    void valueToString(String &sValue);

    uint8_t readCommand();
    bool begin(Stream &stream, bool isACK = true, bool doReset = true);
    bool waitAvailable();
    bool available();
    uint8_t readType();
    uint16_t read();
    void setTimeOut(unsigned long timeOutDuration);
    void next();
    void previous();
    void play(int fileNumber = 1);
    void volumeUp();
    void volumeDown();
    void volume(uint8_t volume);
    void EQ(uint8_t eq);
    void loop(int fileNumber);
    void outputDevice(uint8_t device);
    void sleep();
    void reset();
    void start();
    void pause();
    void playFolder(uint8_t folderNumber, uint8_t fileNumber);
    void outputSetting(bool enable, uint8_t gain);
    void enableLoopAll();
    void disableLoopAll();
    void playMp3Folder(int fileNumber);
    void advertise(int fileNumber);
    void playLargeFolder(uint8_t folderNumber, uint16_t fileNumber);
    void stopAdvertise();
    void stop();
    void loopFolder(int folderNumber);
    void randomAll();
    void enableLoop();
    void disableLoop();
    void enableDAC();
    void disableDAC();
    int readState();
    int readVolume();
    uint8_t readEQ();
    int readFileCounts(uint8_t device);
    int readCurrentFileNumber(uint8_t device);
    int readFileCountsInFolder(int folderNumber);
    int readFileCounts();
    int readFolderCounts();
    int readCurrentFileNumber();

  protected:
    DFPlayerCommand fromJSON(JsonObject &rootObject) override;
    JsonObject &toJSON(DFPlayerCommand value, JsonObject &root) override;
    int cmp(DFPlayerCommand oldValue, DFPlayerCommand newValue) override;
    void updatePhisicalInterface(DFPlayerCommand newValue) override;
    void readStateCallback();
    SoftwareSerial *DFSerial;
    DFRobotDFPlayerMini *DFPlayer;
    uint8_t _serialRxPin, _serialTxPin;
    Task _tReadState;
    void printDetail(uint8_t type, int value);
    void activateSerial();
};

inline DFPlayerInterface::DFPlayerInterface(String publishTopic, String subscribeTopic, uint8_t serialRxPin, uint8_t serialTxPin)
    : NodeInterface<DFPlayerCommand>(publishTopic, subscribeTopic)
{
    _serialRxPin = serialRxPin;
    _serialTxPin = serialTxPin;
    _interfaceName = DFPLAYER_NAME;
    _tReadState.set(STATUS_PULL_RATE, TASK_FOREVER, READ_STATE_CALLBACK);
    this->setSamplingEnabled(false);
}

inline DFPlayerInterface::DFPlayerInterface(String topic, uint8_t serialRxPin, uint8_t serialTxPin)
    : DFPlayerInterface(topic, topic, serialRxPin, serialTxPin)
{
}

inline void DFPlayerInterface::updatePhisicalInterface(DFPlayerCommand newValue)
{
    if (DFSerial == nullptr)
        return;
    switch (newValue.task)
    {
    case NEXT:
        next();
        break;
    case PREVIOUS:
        previous();
        break;
    case PLAY:
        play(newValue.param1);
        break;
    case LOOP:
        loop(newValue.param1);
        break;
    case PAUSE:
        pause();
        break;
    case START:
        start();
        break;
    case STOP:
        stop();
        break;
    case PLAYFOLDER:
        playFolder(newValue.param1, newValue.param2);
        break;
    case VOLUME:
        volume(newValue.param1);
        break;
    }
}

inline void DFPlayerInterface::init()
{
#ifdef ESP8266
    DFSerial = new SoftwareSerial(_serialRxPin, _serialTxPin);
#endif
#ifdef ESP32
    DFSerial = new HardwareSerial(1);
#endif

    DFPlayer = new DFRobotDFPlayerMini();
    DFSerial->begin(9600);
    DFSerial->listen();
    if (!DFPlayer->begin(*DFSerial), false)
    {
        fatal(F("Unable to connect to DFPlayer! 1) Please recheck the connection! 2) Please insert the SD card!"));
        setEnabled(false);
    }
    else
    {
        DFPlayer->setTimeOut(DEFAULT_SERIAL_TIMEOUT);
        this->getScheduler()->addTask(_tReadState);
        _tReadState.restart();
    }
}

inline DFPlayerCommand DFPlayerInterface::fromJSON(JsonObject &rootObject)
{
    DFPlayerCommand value;
    value.task = static_cast<DFPlayerCommandTask>(rootObject["task"].as<uint8_t>());
    if (rootObject.containsKey("param1"))
        value.param1 = rootObject["param1"].as<uint8_t>();
    if (rootObject.containsKey("param2"))
        value.param2 = rootObject["param2"].as<uint8_t>();
    return value;
}

inline JsonObject &DFPlayerInterface::toJSON(DFPlayerCommand value, JsonObject &root)
{
    root["task"] = value.task;
    root["param1"] = value.param1;
    root["param2"] = value.param2;
    return root;
}

inline int DFPlayerInterface::cmp(DFPlayerCommand oldValue, DFPlayerCommand newValue)
{
    return -1;
}

inline void DFPlayerInterface::valueToString(String &sValue)
{
}

inline void DFPlayerInterface::readStateCallback()
{
    DFSerial->listen();
    if (DFPlayer->available())
    {
        printDetail(DFPlayer->readType(), DFPlayer->read()); //Print the detail message from DFPlayer to handle different errors and states.
    }
}
inline void DFPlayerInterface::activateSerial()
{
    DFSerial->listen();
    delay(SOFTWARE_SERIAL_PAUSE);
}

inline uint8_t DFPlayerInterface::readCommand()
{
    activateSerial();
    return DFPlayer->readCommand();
}
inline uint8_t DFPlayerInterface::readType()
{
    activateSerial();
    return DFPlayer->readType();
}
inline uint16_t DFPlayerInterface::read()
{
    activateSerial();
    return DFPlayer->read();
}
inline void DFPlayerInterface::setTimeOut(unsigned long timeOutDuration)
{
    activateSerial();
    DFPlayer->setTimeOut(timeOutDuration);
}
inline void DFPlayerInterface::next()
{
    activateSerial();
    DFPlayer->next();
}
inline void DFPlayerInterface::previous()
{
    activateSerial();
    DFPlayer->previous();
}
inline void DFPlayerInterface::play(int fileNumber)
{
    activateSerial();
    DFPlayer->play(fileNumber);
}
inline void DFPlayerInterface::volumeUp()
{
    activateSerial();
    DFPlayer->volumeUp();
}
inline void DFPlayerInterface::volumeDown()
{
    activateSerial();
    DFPlayer->volumeDown();
}
inline void DFPlayerInterface::volume(uint8_t volume)
{
    activateSerial();
    DFPlayer->volume(volume);
}
inline void DFPlayerInterface::EQ(uint8_t eq)
{
    activateSerial();
    DFPlayer->EQ(eq);
}
inline void DFPlayerInterface::loop(int fileNumber)
{
    activateSerial();
    DFPlayer->loop(fileNumber);
}
inline void DFPlayerInterface::outputDevice(uint8_t device)
{
    activateSerial();
    DFPlayer->outputDevice(device);
}
inline void DFPlayerInterface::sleep()
{
    activateSerial();
    DFPlayer->sleep();
}
inline void DFPlayerInterface::reset()
{
    activateSerial();
    DFPlayer->reset();
}
inline void DFPlayerInterface::start()
{
    activateSerial();
    DFPlayer->start();
}
inline void DFPlayerInterface::pause()
{
    activateSerial();
    DFPlayer->pause();
}
inline void DFPlayerInterface::playFolder(uint8_t folderNumber, uint8_t fileNumber)
{
    activateSerial();
    DFPlayer->playFolder(folderNumber, fileNumber);
}
inline void DFPlayerInterface::outputSetting(bool enable, uint8_t gain)
{
    activateSerial();
    DFPlayer->outputSetting(enable, gain);
}
inline void DFPlayerInterface::enableLoopAll()
{
    activateSerial();
    DFPlayer->enableLoopAll();
}
inline void DFPlayerInterface::disableLoopAll()
{
    activateSerial();
    DFPlayer->disableLoopAll();
}
inline void DFPlayerInterface::playMp3Folder(int fileNumber)
{
    activateSerial();
    DFPlayer->playMp3Folder(fileNumber);
}
inline void DFPlayerInterface::advertise(int fileNumber)
{
    activateSerial();
    DFPlayer->advertise(fileNumber);
}
inline void DFPlayerInterface::playLargeFolder(uint8_t folderNumber, uint16_t fileNumber)
{
    activateSerial();
    DFPlayer->playLargeFolder(folderNumber, fileNumber);
}
inline void DFPlayerInterface::stopAdvertise()
{
    activateSerial();
    DFPlayer->stopAdvertise();
}
inline void DFPlayerInterface::stop()
{
    activateSerial();
    DFPlayer->stop();
}
inline void DFPlayerInterface::loopFolder(int folderNumber)
{
    activateSerial();
    DFPlayer->loopFolder(folderNumber);
}
inline void DFPlayerInterface::randomAll()
{
    activateSerial();
    DFPlayer->randomAll();
}
inline void DFPlayerInterface::enableLoop()
{
    activateSerial();
    DFPlayer->enableLoop();
}
inline void DFPlayerInterface::disableLoop()
{
    activateSerial();
    DFPlayer->disableLoop();
}
inline void DFPlayerInterface::enableDAC()
{
    activateSerial();
    DFPlayer->enableDAC();
}
inline void DFPlayerInterface::disableDAC()
{
    activateSerial();
    DFPlayer->disableDAC();
}
inline int DFPlayerInterface::readState()
{
    activateSerial();
    return DFPlayer->readState();
}
inline int DFPlayerInterface::readVolume()
{
    activateSerial();
    return DFPlayer->readVolume();
}
inline uint8_t DFPlayerInterface::readEQ()
{
    activateSerial();
    return DFPlayer->readEQ();
}
inline int DFPlayerInterface::readFileCounts(uint8_t device)
{
    activateSerial();
    return DFPlayer->readFileCounts(device);
}
inline int DFPlayerInterface::readCurrentFileNumber(uint8_t device)
{
    activateSerial();
    return DFPlayer->readCurrentFileNumber(device);
}
inline int DFPlayerInterface::readFileCountsInFolder(int folderNumber)
{
    activateSerial();
    return DFPlayer->readFileCountsInFolder(folderNumber);
}
inline int DFPlayerInterface::readFileCounts()
{
    activateSerial();
    return DFPlayer->readFileCounts();
}
inline int DFPlayerInterface::readFolderCounts()
{
    activateSerial();
    return DFPlayer->readFolderCounts();
}
inline int DFPlayerInterface::readCurrentFileNumber()
{
    activateSerial();
    return DFPlayer->readCurrentFileNumber();
}

inline void DFPlayerInterface::printDetail(uint8_t type, int value)
{
    switch (type)
    {
    case TimeOut:
        i(F("Time Out!"));
        break;
    case WrongStack:
        error(F("Stack Wrong!"));
        break;
    case DFPlayerCardInserted:
        i(F("Card Inserted!"));
        break;
    case DFPlayerCardRemoved:
        fatal(F("Card Removed!"));
        break;
    case DFPlayerCardOnline:
        i(F("Card Online!"));
        break;
    case DFPlayerPlayFinished:
        i(F("Play Finished!"));
        break;
    case DFPlayerError:
        e(F("DFPlayerError:"));
        switch (value)
        {
        case Busy:
            e(F("Card not found"));
            break;
        case Sleeping:
            e(F("Sleeping"));
            break;
        case SerialWrongStack:
            e(F("Get Wrong Stack"));
            break;
        case CheckSumNotMatch:
            e(F("Check Sum Not Match"));
            break;
        case FileIndexOut:
            e(F("File Index Out of Bound"));
            break;
        case FileMismatch:
            e(F("Cannot Find File"));
            break;
        case Advertise:
            e(F("In Advertise"));
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

#endif //DDFPLAYERINTERFACE_H