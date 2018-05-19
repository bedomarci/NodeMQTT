#ifndef NODEMQTT_H
#define NODEMQTT_H
#include <ESP8266WiFi.h>
#include "misc/config.hpp"
#include "interfaces/_NodeInterface.hpp"
#include "interfaces/HeartbeatInterface.hpp"
#include "interfaces/NodeConfigInterface.hpp"
#include "interfaces/BuzzerInterface.hpp"
#include "interfaces/LogInterface.hpp"
#include "NodeMQTTConfigManager.hpp"
#include "NodeMQTTUpdateManager.hpp"

typedef std::function<void()> NodeMQTTCallback;

class NodeMQTT
{
public:
  NodeMQTT();
  void begin();
  void begin(NodeMQTTConfig &configuration);
  void handle();
  void addInterface(NodeInterfaceBase *);

  void setOnWifiConnecting(NodeMQTTCallback);
  void setOnWifiConnected(NodeMQTTCallback);
  void setOnBrokerConnecting(NodeMQTTCallback);  
  void setOnBrokerConnected(NodeMQTTCallback);  

  void setBaseTopic(String baseTopic);

  void buzz(int);
  void setSystemBuzzer(BuzzerInterface *interface);
  void addTask(Task &task);

protected:
  bool _isSerialDebugging = false;
  bool _isOnline = true;
  bool _isServiceMode = false;
  byte *_serialBuffer = 0;
  const char *_wifiSsid;
  const char *_wifiPassword;
  const char *_mqttServer;
  const char *_mqttUser;
  const char *_mqttPass;
  WiFiClient espClient;
  PubSubClient client;

  NodeMQTTCallback wifiConnectedCallback;
  NodeMQTTCallback wifiConnectingCallback;
  NodeMQTTCallback brokerConnectedCallback;
  NodeMQTTCallback brokerConnectingCallback;

  //SCHEDULER
  Scheduler _scheduler;
  Task _tWifiConnect;
  Task _tBrokerConnect;
  Task _tReadSerial;

  NodeMQTTConfig _config;

  LinkedList<NodeInterfaceBase *> interfaceList;
  String _baseTopic = DEVICE_NAME;

  void reconnectBroker();
  bool isConnectionAlive();
  void reconnectWifi();
  void readSerial();
  void mqttParser(char *topic, byte *payload, unsigned int length);
  void addDefaultInterfaces();

  //STATIC
  static String _MQTTDeviceName;

  //BUILTIN INTERFACES
  HeartbeatInterface *heartbeatInterface;
  NodeConfigInterface *nodeConfigInterface;
  LogInterface *logInterface;
  BuzzerInterface *buzzerInterface;
};
#endif //NODEMQTT_H