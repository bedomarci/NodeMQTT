#if defined(ESP8266) || defined(ESP32)
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

class Scheduler;
struct ApplicationContext;
class NodeInterfaceBase;
struct NodeMQTTProperty;
struct NodeMQTTCronJob;
struct NodeMQTTScheduledTask;
