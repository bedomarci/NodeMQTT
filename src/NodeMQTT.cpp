#include "NodeMQTT.hpp"
#include "constants.hpp"
#include "misc/helpers.hpp"
#include "NodeMQTTCommandProcessor.hpp"
#include "NodeMQTTConfigManager.hpp"
#include "NodeMQTTUpdateManager.hpp"


NodeMQTT::NodeMQTT()
{
    Serial.begin(NODEMQTT_SERIAL_SPEED);
    printHeader();
    interfaceList = LinkedList<NodeInterfaceBase *>();
    _scheduler.init();
    Logger.setFatalCallback([=]() { onFatalError(); });
    _context.scheduler = &_scheduler;
    _context.transport = &_transport;
    _context.parser = &_parser;
    _context.configuration = &_config;
    _context.interfaces = &interfaceList;
}
void NodeMQTT::begin()
{
    NodeMQTTConfig loadedConfig;
    NodeMQTTConfigManager.loadInto(loadedConfig);
    begin(loadedConfig);
}
void NodeMQTT::begin(NodeMQTTConfig &configuration)
{
    d(F("Initializing MQTT NODE"));

    _config = configuration;

    _transport.setContext(&_context);
    _transport.setMessageCallback([=](char *t, byte *p, unsigned int l) { parse(t, (char*)p, l); });
    _transport.setBrokerConnectedCallback([=]() { onBrokerConnected(); });
    _transport.setBrokerConnectingCallback([=]() { onBrokerConnecting(); });
    _transport.setBrokerDisconnectedCallback([=]() { onBrokerDisconnected(); });
    _transport.setNetworkConnectedCallback([=]() { onNetworkConnected(); });
    _transport.setNetworkConnectingCallback([=]() { onNetworkConnecting(); });
    _transport.setNetworkDisconnectedCallback([=]() { onNetworkDisconnected(); });
    if (_config.isOnline)
        _transport.init();

    _parser.setContext(&_context);
    _parser.setInterfaces(&interfaceList);
    
    Logger.setLogging(_config.isLogging);
#ifdef NODEMQTT_SERVICE_MODE
    _config.isServiceMode = true;
#endif

    NodeMQTTCommandProcessor.init(&_context);

    if (_config.isServiceMode)
        NodeMQTTConfigManager.print(_config);

    this->addDefaultInterfaces();
    this->setBaseTopic(String(_config.baseTopic));
    this->initializeInterfaces();

    if (_context.configuration->isOnline)
        _transport.connectNetwork();

    buzz(TONE_SYSTEM_BOOT);
}

void NodeMQTT::handle()
{
    if (_config.isOnline)
    {
        _transport.loop();
#ifdef WIFI_TRANSPORT
        if (_config.isServiceMode && _transport.isNetworkConnected())
        {
            NodeMQTTUpdateManager.checkForUpload();
        }
#endif
    }
    readSerial();         //SERIAL PULL
    _scheduler.execute(); //TASK EXECUTION
}

void NodeMQTT::addInterface(NodeInterfaceBase *interface)
{
    interfaceList.add(interface);
    // interface->setTransport(&_transport);
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

void NodeMQTT::readSerial()
{
    if (Serial.available())
    {
        _serialBuffer = new char[SERIAL_BUFFER_SIZE];
        size_t length = Serial.readBytesUntil('\r', _serialBuffer, SERIAL_BUFFER_SIZE);
        _serialBuffer[length] = '\0';       
        NodeMQTTCommandProcessor.execute(_serialBuffer);
        free(_serialBuffer);
    }
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
    NodeMQTTUpdateManager.begin(&_config);
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
    nodeConfigInterface->publishCurrentConfig(_config);
    buzz(TONE_SYSTEM_ONLINE);
}
void NodeMQTT::onBrokerDisconnected()
{
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

void NodeMQTT::printHeader()
{
    Serial.print("\n\n\n");
    Serial.println(FPSTR(TERMINAL_HR));
    Serial.print(FPSTR(NODEMQTT_TERMINAL_HEADER));
    Serial.print("\n");
    Serial.println(FPSTR(TERMINAL_HR));
}
void NodeMQTT::onFatalError()
{
    buzz(TONE_FAIL);
}

ApplicationContext*  NodeMQTT::getContext() {
    return &_context;
}