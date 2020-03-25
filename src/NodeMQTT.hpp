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
#include "transports/WifiTransport.hpp"
#include "parsers/PubSubParser.hpp"
#include "misc/time/NTPTime.hpp"
#include "io/_AbstractIO.hpp"
#include "NodeMQTTScheduler.hpp"

class NodeMQTT {
public:
    NodeMQTT();

    void begin();

    void handle();

    void addInterface(NodeInterfaceBase *);

    void addIO(AbstractIO *io);

    void setNetworkConnectingCallback(const NodeMQTTCallback &);

    void setNetworkConnectedCallback(const NodeMQTTCallback &);

    void setNetworkDisconnectedCallback(const NodeMQTTCallback &);

    void setBrokerConnectingCallback(const NodeMQTTCallback &);

    void setBrokerConnectedCallback(const NodeMQTTCallback &);

    void setBrokerDisconnectedCallback(const NodeMQTTCallback &);

    void setTimeSyncedCallback(NodeMQTTCallback);

    void setTimeReceivedCallback(NodeMQTTCallback);

    void setBaseTopic(String baseTopic);

    void setInterfaceBaseTopic();

    String getBaseTopic();

    void buzz(int);

    void setSystemBuzzer(BuzzerInterface *interface);

    void addTask(Task &task);


    ApplicationContext *getContext();

protected:
    char *_serialBuffer = 0;

    TRANSPORT_CLASS _transport;
    PARSER_CLASS _parser;
    Scheduler _scheduler;
    ApplicationContext _context;

    LinkedList<NodeInterfaceBase *> interfaceList;
    String baseTopic = String(UUID);

    void registerConfiguration();

    void loadConfiguration();

    void parse(char *topic, char *payload, unsigned int length);

    void subscribeTopics();

    void addDefaultInterfaces();

    void initializeInterfaces();

    void initializeComponents();

    void bootComponents();

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
    HeartbeatInterface *heartbeatInterface = nullptr;
    CommandInterface *commandInterface = nullptr;
    NodeConfigInterface *nodeConfigInterface = nullptr;
    LogInterface *logInterface = nullptr;
    BuzzerInterface *buzzerInterface = nullptr;

private:
    NodeMQTTCallback networkConnectingCallback;
    NodeMQTTCallback networkConnectedCallback;
    NodeMQTTCallback networkDisconnectedCallback;
    NodeMQTTCallback brokerConnectingCallback;
    NodeMQTTCallback brokerConnectedCallback;
    NodeMQTTCallback brokerDisconnectedCallback;
    NTPTime ntpTime;

    bool isLogging = true;
    bool isServiceMode = true;
    bool isOnline = true;

};

#endif //NODEMQTT_H