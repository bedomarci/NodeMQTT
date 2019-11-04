#include "NodeMQTT.hpp"
#include "constants.hpp"
#include "misc/helpers.hpp"
#include "NodeMQTTCommandProcessor.hpp"
#include "NodeMQTTConfigManager.hpp"
#include "NodeMQTTUpdateManager.hpp"
#include "NodeMQTTScheduler.hpp"
#include "NodeMQTTIOContainer.hpp"

NodeMQTT::NodeMQTT()
{
    _config = new NodeMQTTConfig();
    interfaceList = LinkedList<NodeInterfaceBase *>();
    _scheduler.init();
    _context.scheduler = &_scheduler;
    _context.transport = &_transport;
    _context.parser = &_parser;
    _context.configuration = _config;
    _context.interfaces = &interfaceList;
    
    NodeMQTTIO.init(&_context);
    NodeMQTTCommandProcessor.init(&_context);
    NodeMQTTScheduler.init(&_context);
    Logger.init(&_context);

    printHeader(NodeMQTTIO);
    Logger.setFatalCallback([=]() { onFatalError(); });
}
void NodeMQTT::begin()
{
    // NodeMQTTConfig * loadedConfig; = new NodeMQTTConfig();

    // NodeMQTTConfigManager.loadInto(loadedConfig);
    NodeMQTTConfigManager.loadInto(_config);
    begin(_config);
}
void NodeMQTT::begin(NodeMQTTConfig *configuration)
{
    _config = configuration;

    Logger.logf(INFO, MSG_INTRODUCTION, getContext()->configuration->baseTopic, toDateTimeString(FIRMWARE_BUILD_TIME).c_str(), FIRMWARE_BUILD_TIME);
    d(F("Initializing NodeMQTT"));

    _transport.setContext(&_context);
    _transport.setMessageCallback([=](char *t, byte *p, unsigned int l) { parse(t, (char *)p, l); });
    _transport.setBrokerConnectedCallback([=]() { onBrokerConnected(); });
    _transport.setBrokerConnectingCallback([=]() { onBrokerConnecting(); });
    _transport.setBrokerDisconnectedCallback([=]() { onBrokerDisconnected(); });
    _transport.setNetworkConnectedCallback([=]() { onNetworkConnected(); });
    _transport.setNetworkConnectingCallback([=]() { onNetworkConnecting(); });
    _transport.setNetworkDisconnectedCallback([=]() { onNetworkDisconnected(); });
    if (_config->isOnline) {
        _transport.init();
    }

    _parser.setContext(&_context);
    _parser.setInterfaces(&interfaceList);

    Logger.setLogging(_config->isLogging);
#ifdef NODEMQTT_SERVICE_MODE
    _config->isServiceMode = true;
#endif

    if (_config->isServiceMode)
        NodeMQTTConfigManager.print(_config);

    this->addDefaultInterfaces();
    this->setBaseTopic(String(_config->baseTopic));
    this->initializeInterfaces();


    if (_context.configuration->isOnline)
        _transport.connectNetwork();

    buzz(TONE_SYSTEM_BOOT);
}

void NodeMQTT::handle()
{
    if (_config->isOnline)
    {
        _transport.loop();
#ifdef WIFI_TRANSPORT
        if (_config->isServiceMode && _transport.isNetworkConnected())
        {
            NodeMQTTUpdateManager.checkForUpload();
        }
#endif
    }
    _scheduler.execute(); //TASK EXECUTION
//    ESP.wdtFeed();
    yield();
}

void NodeMQTT::addInterface(NodeInterfaceBase *interface)
{
    interfaceList.add(interface);
    interface->setContext(&_context);
}



void NodeMQTT::subscribeTopics()
{
    NodeInterfaceBase *interface;
    String topics = "";
    for (int i = 0; i < interfaceList.size(); i++)
    {
        interface = interfaceList.get(i);
        if (interface->hasMQTTSubscribe())
        {
            _transport.subscribe((interface->getSubscribeTopic()).c_str());
            if (topics.length() > 0)
                topics += ", ";
            topics += interface->getSubscribeTopic();
        }
    }
    Logger.logf(INFO, F("Subscribed to: %s"), topics.c_str());
}

void NodeMQTT::initializeInterfaces()
{
    for (int i = 0; i < interfaceList.size(); i++)
        interfaceList.get(i)->init();
}



void NodeMQTT::parse(char *topic, char *payload, unsigned int length)
{
    payload[length] = '\0';
    String s_payload = String(payload);
    Logger.logf(DEBUG, F("Receiving payload: %s"), s_payload.c_str());
    _parser.parse(topic, payload);
}

void NodeMQTT::setBaseTopic(String baseTopic)
{
    _baseTopic = baseTopic;
    NodeInterfaceBase *interface;
    for (int i = 0; i < interfaceList.size(); i++)
    {
        interface = interfaceList.get(i);
        if (interface->getBaseTopic().length() == 0)
        {
            interface->setBaseTopic(baseTopic);
        }
    }
}

String NodeMQTT::getBaseTopic()
{
    return _baseTopic;
}

void NodeMQTT::addDefaultInterfaces()
{
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

void NodeMQTT::setSystemBuzzer(BuzzerInterface *interface)
{
    buzzerInterface = interface;
}

void NodeMQTT::addTask(Task &task)
{
    _scheduler.addTask(task);
}

void NodeMQTT::buzz(int noteId)
{
    if (buzzerInterface)
    {
        buzzerInterface->preventDebugLogging(true);
        buzzerInterface->write(noteId, false);
        buzzerInterface->preventDebugLogging(false);
    }
}

void NodeMQTT::onNetworkConnecting()
{
    if (networkConnectingCallback != nullptr)
        networkConnectingCallback();
}
void NodeMQTT::onNetworkConnected()
{
    
    #ifdef WIFI_TRANSPORT
    NodeMQTTUpdateManager.init(getContext());
    ntpTime.init(getContext());
    #endif
    if (networkConnectedCallback != nullptr)
        networkConnectedCallback();
}
void NodeMQTT::onNetworkDisconnected()
{
    if (networkDisconnectedCallback != nullptr)
        networkDisconnectedCallback();

    if (heartbeatInterface->isEnabled())
        heartbeatInterface->setEnabled(false);
    buzz(TONE_WARNING);
}
void NodeMQTT::onBrokerConnecting()
{
    if (brokerConnectingCallback != nullptr)
        brokerConnectingCallback();
}
void NodeMQTT::onBrokerConnected()
{
    if (brokerConnectedCallback != nullptr)
        brokerConnectedCallback();

    this->subscribeTopics();
    heartbeatInterface->setEnabled(true);
    nodeConfigInterface->publishCurrentConfig(*_config);
    logInterface->enable();
    buzz(TONE_SYSTEM_ONLINE);
}
void NodeMQTT::onBrokerDisconnected()
{
    logInterface->disable();
    if (brokerDisconnectedCallback != nullptr)
        brokerDisconnectedCallback();

    if (heartbeatInterface->isEnabled())
        heartbeatInterface->setEnabled(false);
}

void NodeMQTT::setNetworkConnectingCallback(NodeMQTTCallback cb)
{
    networkConnectingCallback = cb;
}
void NodeMQTT::setNetworkConnectedCallback(NodeMQTTCallback cb)
{
    networkConnectedCallback = cb;
}
void NodeMQTT::setNetworkDisconnectedCallback(NodeMQTTCallback cb)
{
    networkDisconnectedCallback = cb;
}
void NodeMQTT::setBrokerConnectingCallback(NodeMQTTCallback cb)
{
    brokerConnectingCallback = cb;
}
void NodeMQTT::setBrokerConnectedCallback(NodeMQTTCallback cb)
{
    brokerConnectedCallback = cb;
}
void NodeMQTT::setBrokerDisconnectedCallback(NodeMQTTCallback cb)
{
    brokerDisconnectedCallback = cb;
}

void NodeMQTT::onFatalError()
{
    buzz(TONE_FAIL);
}

ApplicationContext *NodeMQTT::getContext()
{
    return &_context;
}

