#include "NodeMQTT.hpp"
#include "constants.hpp"
#include "misc/helpers.hpp"
#include "NodeMQTTCommandProcessor.hpp"
#include "NodeMQTTConfigManager.hpp"
#include "NodeMQTTUpdateManager.hpp"
#include "NodeMQTTScheduler.hpp"
#include "NodeMQTTCron.hpp"
#include "NodeMQTTIOContainer.hpp"

NodeMQTT::NodeMQTT() {
//    _config = new NodeMQTTConfig();
    interfaceList = LinkedList<NodeInterfaceBase *>();
    _scheduler.init();
    _context.scheduler = &_scheduler;
    _context.transport = &_transport;
    _context.parser = &_parser;
//    _context.configuration = _config;
    _context.interfaces = &interfaceList;

    NodeMQTTIO.init(&_context);
    NodeMQTTCommandProcessor.init(&_context);
    NodeMQTTScheduler.init(&_context);
    NodeMQTTCron.init(&_context);
    Logger.init(&_context);

    printHeader(NodeMQTTIO);
    Logger.setFatalCallback([=]() { onFatalError(); });
    registerConfiguration();

}

void NodeMQTT::begin() {
    NodeMQTTConfigManager.load();
    loadConfiguration();

    Logger.logf(INFO, MSG_INTRODUCTION, this->baseTopic.c_str(), toDateTimeString(FIRMWARE_BUILD_TIME).c_str(), FIRMWARE_BUILD_TIME);
    d(F("Initializing NodeMQTT"));

    _transport.setContext(&_context);
    _transport.setMessageCallback([=](char *t, byte *p, unsigned int l) { parse(t, (char *) p, l); });
    _transport.setBrokerConnectedCallback([=]() { onBrokerConnected(); });
    _transport.setBrokerConnectingCallback([=]() { onBrokerConnecting(); });
    _transport.setBrokerDisconnectedCallback([=]() { onBrokerDisconnected(); });
    _transport.setNetworkConnectedCallback([=]() { onNetworkConnected(); });
    _transport.setNetworkConnectingCallback([=]() { onNetworkConnecting(); });
    _transport.setNetworkDisconnectedCallback([=]() { onNetworkDisconnected(); });

    if (this->isOnline) {
        _transport.init();
    }

    _parser.setContext(&_context);
    _parser.setInterfaces(&interfaceList);

    Logger.setLogging(this->isLogging);
#ifdef NODEMQTT_SERVICE_MODE
    this->isServiceMode = true;
#endif

    if (this->isServiceMode)
//        NodeMQTTConfigManager.print(_config);
        NodeMQTTConfigManager.print();

    this->addDefaultInterfaces();
    setInterfaceBaseTopic();
    this->initializeInterfaces();

    if (this->isOnline)
        _transport.connectNetwork();

    buzz(TONE_SYSTEM_BOOT);
}

void NodeMQTT::handle() {
    if (this->isOnline) {
        _transport.loop();
#ifdef WIFI_TRANSPORT
        if (this->isServiceMode && _transport.isNetworkConnected()) {
            NodeMQTTUpdateManager.checkForUpload();
        }
#endif
    }
    _scheduler.execute(); //TASK EXECUTION
//    ESP.wdtFeed();
    yield();
}

void NodeMQTT::addInterface(NodeInterfaceBase *interface) {
    interfaceList.add(interface);
    interface->setContext(&_context);
}


void NodeMQTT::subscribeTopics() {
    NodeInterfaceBase *interface;
    String topics = "";
    for (int i = 0; i < interfaceList.size(); i++) {
        interface = interfaceList.get(i);
        if (interface->hasMQTTSubscribe()) {
            _transport.subscribe((interface->getSubscribeTopic()).c_str());
            if (topics.length() > 0)
                topics += ", ";
            topics += interface->getSubscribeTopic();
        }
    }
    Logger.logf(INFO, F("Subscribed to: %s"), topics.c_str());
}

void NodeMQTT::initializeInterfaces() {
    for (int i = 0; i < interfaceList.size(); i++)
        interfaceList.get(i)->init();
}


void NodeMQTT::parse(char *topic, char *payload, unsigned int length) {
    payload[length] = '\0';
    String s_payload = String(payload);
    Logger.logf(DEBUG, F("Receiving payload: %s"), s_payload.c_str());
    _parser.parse(topic, payload);
}

void NodeMQTT::setBaseTopic(String baseTopic) {
    this->baseTopic = baseTopic;
    setInterfaceBaseTopic();
}


void NodeMQTT::setInterfaceBaseTopic() {
    NodeInterfaceBase *interface;
    for (int i = 0; i < interfaceList.size(); i++) {
        interface = interfaceList.get(i);
        if (interface->getBaseTopic().length() == 0) {
            interface->setBaseTopic(this->baseTopic);
        }
    }
}

String NodeMQTT::getBaseTopic() {
    return this->baseTopic;
}

void NodeMQTT::addDefaultInterfaces() {
    heartbeatInterface = new HeartbeatInterface();
    nodeConfigInterface = new NodeConfigInterface();
    logInterface = new LogInterface();
    commandInterface = new CommandInterface();
    logInterface->disable();
    addInterface(nodeConfigInterface);
    addInterface(heartbeatInterface);
    addInterface(logInterface);
    addInterface(commandInterface);
}

void NodeMQTT::setSystemBuzzer(BuzzerInterface *interface) {
    buzzerInterface = interface;
}

void NodeMQTT::addTask(Task &task) {
    _scheduler.addTask(task);
}

void NodeMQTT::buzz(int noteId) {
    if (buzzerInterface) {
        buzzerInterface->preventDebugLogging(true);
        buzzerInterface->write(noteId, false);
        buzzerInterface->preventDebugLogging(false);
    }
}

void NodeMQTT::onNetworkConnecting() {
    if (networkConnectingCallback != nullptr)
        networkConnectingCallback();
}

void NodeMQTT::onNetworkConnected() {

#ifdef WIFI_TRANSPORT
    NodeMQTTUpdateManager.init(getContext());
    ntpTime.init(getContext());
#endif
    if (networkConnectedCallback != nullptr)
        networkConnectedCallback();
}

void NodeMQTT::onNetworkDisconnected() {
    if (networkDisconnectedCallback != nullptr)
        networkDisconnectedCallback();

    if (heartbeatInterface->isEnabled())
        heartbeatInterface->setEnabled(false);
    buzz(TONE_WARNING);
}

void NodeMQTT::onBrokerConnecting() {
    if (brokerConnectingCallback != nullptr)
        brokerConnectingCallback();
}

void NodeMQTT::onBrokerConnected() {
    if (brokerConnectedCallback != nullptr)
        brokerConnectedCallback();

    this->subscribeTopics();
    heartbeatInterface->setEnabled(true);
    nodeConfigInterface->publishCurrentConfig();
    logInterface->enable();
    buzz(TONE_SYSTEM_ONLINE);
}

void NodeMQTT::onBrokerDisconnected() {
    logInterface->disable();
    if (brokerDisconnectedCallback != nullptr)
        brokerDisconnectedCallback();

    if (heartbeatInterface->isEnabled())
        heartbeatInterface->setEnabled(false);
}

void NodeMQTT::setNetworkConnectingCallback(NodeMQTTCallback cb) {
    networkConnectingCallback = cb;
}

void NodeMQTT::setNetworkConnectedCallback(NodeMQTTCallback cb) {
    networkConnectedCallback = cb;
}

void NodeMQTT::setNetworkDisconnectedCallback(NodeMQTTCallback cb) {
    networkDisconnectedCallback = cb;
}

void NodeMQTT::setBrokerConnectingCallback(NodeMQTTCallback cb) {
    brokerConnectingCallback = cb;
}

void NodeMQTT::setBrokerConnectedCallback(NodeMQTTCallback cb) {
    brokerConnectedCallback = cb;
}

void NodeMQTT::setBrokerDisconnectedCallback(NodeMQTTCallback cb) {
    brokerDisconnectedCallback = cb;
}

void NodeMQTT::setTimeSyncedCallback(NodeMQTTCallback cb) {
    ntpTime.setTimeSyncedCallback(cb);
}

void NodeMQTT::setTimeReceivedCallback(NodeMQTTCallback cb) {
    ntpTime.setTimeReceivedCallback(cb);
}

void NodeMQTT::onFatalError() {
    buzz(TONE_FAIL);
}

ApplicationContext *NodeMQTT::getContext() {
    return &_context;
}

void NodeMQTT::registerConfiguration() {
    NodeMQTTConfigManager.registerBoolProperty(PROP_SYS_ONLINE, (const char *) ATTR_ONLINE, true);
    NodeMQTTConfigManager.registerBoolProperty(PROP_SYS_SERVICEMODE, (const char *) ATTR_SERVICEMODE, true);
    NodeMQTTConfigManager.registerBoolProperty(PROP_SYS_LOGGING, (const char *) ATTR_LOGGING, true);
    NodeMQTTConfigManager.registerStringProperty(PROP_SYS_BASETOPIC, (const char *) ATTR_BASETOPIC, DEFAULT_MQTT_BASE_TOPIC);

}

void NodeMQTT::loadConfiguration() {
    this->isOnline = NodeMQTTConfigManager.getBoolProperty(PROP_SYS_ONLINE);
    this->isLogging = NodeMQTTConfigManager.getBoolProperty(PROP_SYS_LOGGING);
    this->isServiceMode = NodeMQTTConfigManager.getBoolProperty(PROP_SYS_SERVICEMODE);
    this->baseTopic = NodeMQTTConfigManager.getStringProperty(PROP_SYS_BASETOPIC);

}


