#include "NodeMQTT.hpp"
#include "misc/constants.hpp"
#include "misc/helpers.hpp"

NodeMQTT::NodeMQTT()
{
    Serial.begin(115200);
    printHeader();
    interfaceList = LinkedList<NodeInterfaceBase *>();
    _scheduler.init();
    _scheduler.addTask(_tReadSerial);
}
void NodeMQTT::begin()
{
    NodeMQTTConfigManager.loadInto(_config);
    begin(_config);
}
void NodeMQTT::begin(NodeMQTTConfig &configuration)
{
    d(("Initializing MQTT NODE"));
    _transport.setScheduler(&_scheduler);
    _transport.setConfiguration(&_config);
    _transport.setMessageCallback([=](char *t, byte *p, unsigned int l) { mqttParser(t, p, l); });
    _transport.setBrokerConnectedCallback([=]() { onBrokerConnected(); });
    _transport.setBrokerConnectingCallback([=]() { onBrokerConnecting(); });
    _transport.setBrokerDisconnectedCallback([=]() { onBrokerDisconnected(); });
    _transport.setNetworkConnectedCallback([=]() { onNetworkConnected(); });
    _transport.setNetworkConnectingCallback([=]() { onNetworkConnecting(); });
    _transport.setNetworkDisconnectedCallback([=]() { onNetworkDisconnected(); });
    _transport.init();
#ifndef NODEMQTT_SERVICE_MODE
    _isServiceMode = &configuration.isServiceMode;
#else
    _isServiceMode = true;
#endif
    // client.setServer(_mqttServer, configuration.mqttPort);

    if (_isServiceMode)
        NodeMQTTConfigManager.print(_config);
    addDefaultInterfaces();

    setBaseTopic(String(configuration.baseTopic));
    if (_isOnline)
    {
        _transport.connectNetwork();
    }
    buzz(TONE_SYSTEM_BOOT);
}

void NodeMQTT::handle()
{
    _transport.loop();
    readSerial();         //SERIAL PULL
    _scheduler.execute(); //TASK EXECUTION
}

void NodeMQTT::addInterface(NodeInterfaceBase *interface)
{
    interfaceList.add(interface);
    interface->setTransport(&_transport);
    interface->setScheduler(&_scheduler);
    interface->init();
}

void NodeMQTT::subscribeTopics()
{
    NodeInterfaceBase *interface;
    for (int i = 0; i < interfaceList.size(); i++)
    {
        interface = interfaceList.get(i);
        if (interface->hasMQTTSubscribe())
        {
            _transport.subscribe((interface->getSubscribeTopic()).c_str());
        }
    }
}

void NodeMQTT::readSerial()
{
    if (Serial.available())
    {
        _serialBuffer = new byte[SERIAL_BUFFER_SIZE];
        size_t length = Serial.readBytesUntil('\r', (char *)_serialBuffer, SERIAL_BUFFER_SIZE);
        String topic = _baseTopic + "/" + FPSTR(CONFIG_TOPIC);
        mqttParser(&String(topic)[0u], _serialBuffer, length);
        free(_serialBuffer);
    }
}

void NodeMQTT::mqttParser(char *topic, byte *payload, unsigned int length)
{
    NodeInterfaceBase *interface;
    payload[length] = '\0';
    String s_payload = String((char *)payload);
    D_MQTT("Receiving payload: " + s_payload);
    for (int i = 0; i < interfaceList.size(); i++)
    {
        interface = interfaceList.get(i);
        if (strcmp(interface->getSubscribeTopic().c_str(), topic) == 0)
        {
            interface->writeRaw(s_payload, false);
        }
    }
}

void NodeMQTT::setBaseTopic(String baseTopic)
{
    _baseTopic = baseTopic;
    NodeInterfaceBase *interface;
    for (int i = 0; i < interfaceList.size(); i++)
    {
        interface = interfaceList.get(i);
        interface->setBaseTopic(_baseTopic);
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
    addInterface(nodeConfigInterface);
    addInterface(heartbeatInterface);
    addInterface(logInterface);
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
        buzzerInterface->write(noteId, false);
    }
}

void NodeMQTT::onNetworkConnecting()
{
    if (networkConnectingCallback != nullptr)
        networkConnectingCallback();
}
void NodeMQTT::onNetworkConnected()
{
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

    subscribeTopics();
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
    // Serial.println(FPSTR(TERMINAL_HR));
    // Serial.print(FPSTR(NODEMQTT_TERMINAL_HEADER));
    // Serial.print("\n");
    // Serial.println(FPSTR(TERMINAL_HR));
}