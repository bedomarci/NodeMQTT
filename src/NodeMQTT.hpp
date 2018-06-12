#ifndef NODEMQTT_H
#define NODEMQTT_H
#include "misc/config.hpp"
#include "interfaces/_NodeInterface.hpp"
#include "interfaces/HeartbeatInterface.hpp"
#include "interfaces/NodeConfigInterface.hpp"
#include "interfaces/BuzzerInterface.hpp"
#include "interfaces/LogInterface.hpp"
#include "transport/WifiTransport.hpp"
#include "NodeMQTTConfigManager.hpp"
#include "NodeMQTTUpdateManager.hpp"

class NodeMQTT
{
public:
  NodeMQTT();
  void begin();
  void begin(NodeMQTTConfig &configuration);
  void handle();
  void addInterface(NodeInterfaceBase *);

  void setNetworkConnectingCallback(NodeMQTTCallback);
  void setNetworkConnectedCallback(NodeMQTTCallback);
  void setNetworkDisconnectedCallback(NodeMQTTCallback);
  void setBrokerConnectingCallback(NodeMQTTCallback);
  void setBrokerConnectedCallback(NodeMQTTCallback);
  void setBrokerDisconnectedCallback(NodeMQTTCallback);

  void setBaseTopic(String baseTopic);
  String getBaseTopic();

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
  TRANSPORT_CLASS _transport;
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
  void subscribeTopics();
  void printHeader();

  void onNetworkConnecting();
  void onNetworkConnected();
  void onNetworkDisconnected();
  void onBrokerConnecting();
  void onBrokerConnected();
  void onBrokerDisconnected();

  //STATIC
  static String _MQTTDeviceName;

  //BUILTIN INTERFACES
  HeartbeatInterface *heartbeatInterface;
  NodeConfigInterface *nodeConfigInterface;
  LogInterface *logInterface;
  BuzzerInterface *buzzerInterface;

private:
  NodeMQTTCallback networkConnectingCallback;
  NodeMQTTCallback networkConnectedCallback;
  NodeMQTTCallback networkDisconnectedCallback;
  NodeMQTTCallback brokerConnectingCallback;
  NodeMQTTCallback brokerConnectedCallback;
  NodeMQTTCallback brokerDisconnectedCallback;
};
#endif //NODEMQTT_H