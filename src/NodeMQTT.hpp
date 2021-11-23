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
#include "parsers/PubSubParser.hpp"
#include "misc/time/NTPTime.hpp"
#include "io/_AbstractIO.hpp"
#include "NodeMQTTScheduler.hpp"

#if (NODEMQTT_TRANSPORT == RF_TRANSPORT)
#include "transports/RF24Transport.hpp"
#endif
#if (NODEMQTT_TRANSPORT == WIFI_TRANSPORT)

#include "transports/WifiTransport.hpp"

#endif
#if (NODEMQTT_TRANSPORT == NULL_TRANSPORT)
#include "transports/NullTransport.hpp"
#endif


class NodeMQTT {
public:
    NodeMQTT();

    void begin();

    void handle();

    void addInterface(NodeInterfaceBase *);

    void addIO(AbstractIO *io);

    void setBaseTopic(String baseTopic);

    void setInterfaceBaseTopic();

    String getBaseTopic();

    void buzz(int);

    void setSystemBuzzer(BuzzerInterface *interface);

    void addTask(Task &task);

    ApplicationContext *getContext();

protected:
    TRANSPORT_CLASS _transport;
    PARSER_CLASS _parser;
    Scheduler _scheduler;
    ApplicationContext _context;

    LinkedList<NodeInterfaceBase *> interfaceList;
    String baseTopic = String(UUID);

    void registerConfiguration();

    void registerEvents();

    void loadConfiguration();

    void parse(char *topic, char *payload, unsigned int length);

    void subscribeTopics();

    void addDefaultInterfaces();

    void initializeInterfaces();

    void initializeComponents();

    void bootComponents();

    void onNetworkDisconnected();

    void onBrokerConnected();

    void onBrokerDisconnected();

    void onFatalError();

    //BUILTIN INTERFACES
    HeartbeatInterface *heartbeatInterface = nullptr;
    CommandInterface *commandInterface = nullptr;
    NodeConfigInterface *nodeConfigInterface = nullptr;
    LogInterface *logInterface = nullptr;
    BuzzerInterface *buzzerInterface = nullptr;

private:
    NTPTime ntpTime;

    bool isLogging = true;
    bool isServiceMode = true;
    bool isOnline = true;
    bool booting = true;

};

#endif //NODEMQTT_H