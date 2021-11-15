#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#include <functional>

typedef std::function<void(char *, uint8_t *, unsigned int)> NodeMQTTMessageCallback;
typedef std::function<void()> NodeMQTTCallback;
typedef std::function<void(const char *)> NodeMQTTStringCallback;
#define NodeMQTTChangeCallback std::function<void(T, T)>

#else
typedef void (*NodeMQTTMessageCallback)(char *, uint8_t *, unsigned int);
typedef void (*NodeMQTTCallback)();
typedef void (*NodeMQTTStringCallback)(const char *);
#define NodeMQTTChangeCallback void (*)(T, T)
#endif


template<typename T, unsigned int LENGTH>
struct Array;
struct Note;
struct ApplicationContext;
struct NodeMQTTProperty;
struct NodeMQTTCronJob;
struct NodeMQTTScheduledTask;

class Scheduler;

class NodeInterfaceBase;

class AbstractParser;

class AbstractTransport;
