#ifndef NODEMQTT_H
#define NODEMQTT_H
#include "misc/config.hpp"
#include "constants.hpp"
#include "interfaces/_NodeInterface.hpp"
#include "interfaces/HeartbeatInterface.hpp"
#include "interfaces/NodeConfigInterface.hpp"
#include "interfaces/CommandInterface.hpp"
#include "interfaces/BuzzerInterface.hpp"
#include "interfaces/LogInterface.hpp"
// #define TRANSPORT_PATH "transports/" #TRANSPORT_CLASS 
// #include TRANSPORT_PATH
// #define PARSER_PATH "parsers/" #PARSER_CLASS ".hpp"
// #include PARSER_PATH
#include "transports/WifiTransport.hpp"
#include "parsers/PubSubParser.hpp"
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

  ApplicationContext* getContext();

protected:
  char *_serialBuffer = 0;

  TRANSPORT_CLASS _transport;
  PARSER_CLASS _parser;
  Scheduler _scheduler;
  NodeMQTTConfig _config;
  ApplicationContext _context;

  LinkedList<NodeInterfaceBase *> interfaceList;
  String _baseTopic = String(UUID);

  void reconnectBroker();
  bool isConnectionAlive();
  void reconnectWifi();
  void readSerial();
  void parse(char *topic, char *payload, unsigned int length);
  void addDefaultInterfaces();
  void subscribeTopics();
  void initializeInterfaces();
  void printHeader();

  void onNetworkConnecting();
  void onNetworkConnected();
  void onNetworkDisconnected();
  void onBrokerConnecting();
  void onBrokerConnected();
  void onBrokerDisconnected();

  void onFatalError();

  //STATIC
  static String _MQTTDeviceName;

  //BUILTIN INTERFACES
  HeartbeatInterface *heartbeatInterface;
  CommandInterface *commandInterface;
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