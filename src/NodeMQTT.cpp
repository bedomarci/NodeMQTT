#include "NodeMQTT.hpp"
#include "misc/constants.hpp"
#include "misc/helpers.hpp"

NodeMQTT::NodeMQTT()
{
    Serial.begin(115200);
    client = PubSubClient(espClient);
    interfaceList = LinkedList<NodeInterfaceBase *>();
    client.setCallback([=](char *t, byte *p, unsigned int l) { mqttParser(t, p, l); });
    _scheduler.init();
    _tWifiConnect.set(WIFI_CONNECT_ATTEMPT_WAITING, TASK_FOREVER, [this]() { reconnectWifi(); });
    _tBrokerConnect.set(MQTT_CONNECT_ATTEMPT_WAITING, TASK_FOREVER, [this]() { reconnectBroker(); });
    _scheduler.addTask(_tWifiConnect);
    _scheduler.addTask(_tBrokerConnect);
    _scheduler.addTask(_tReadSerial);
}
void NodeMQTT::begin()
{
    NodeMQTTConfigManager.loadInto(_config);
    begin(_config);
}
void NodeMQTT::begin(NodeMQTTConfig &configuration)
{
    D_MQTT(("Initializing MQTT NODE"));
    _wifiSsid = &configuration.wifiSsid[0];
    _wifiPassword = &configuration.wifiPassword[0];
    _mqttServer = &configuration.mqttServer[0];
    _mqttUser = &configuration.mqttUser[0];
    _mqttPass = &configuration.mqttPassword[0];
    _isOnline = &configuration.isOnline;
#ifndef SERVICE_MODE
    _isServiceMode = &configuration.isServiceMode;
#else
    _isServiceMode = true;
#endif
    client.setServer(_mqttServer, configuration.mqttPort);

    if (_isServiceMode)
        NodeMQTTConfigManager.print(_config);
    addDefaultInterfaces();

    setBaseTopic(String(configuration.baseTopic));
    if(_isOnline) {
        _tWifiConnect.enable();
    }
    buzz(TONE_SYSTEM_BOOT);
}

void NodeMQTT::reconnectWifi()
{
    D_NET("Connecting to " + String(_wifiSsid));
    if (wifiConnectingCallback != nullptr)
        wifiConnectingCallback();
    WiFi.begin(_wifiSsid, _wifiPassword);   
}

void NodeMQTT::handle()
{
    if (_isOnline)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            if (_tWifiConnect.isEnabled())
            {
                delay(500);
                if (wifiConnectedCallback != nullptr)
                    wifiConnectedCallback();
                D_NET("WiFi connected. IP address: " + WiFi.localIP().toString());
                _tWifiConnect.disable();
            } else {
                if (!client.connected())
                {
                    _tBrokerConnect.enableIfNot();
                }
            }
        }
        else
        {
            if (heartbeatInterface->isEnabled())
                heartbeatInterface->setEnabled(false);
            if(_isOnline)
                _tWifiConnect.enableIfNot();
        }
    }

    client.loop();        //PUBSUB PULL
    readSerial();         //SERIAL PULL
    _scheduler.execute(); //TASK EXECUTION
}

void NodeMQTT::addInterface(NodeInterfaceBase *interface)
{
    interfaceList.add(interface);
    interface->setClient(&client);
    interface->setScheduler(&_scheduler);
    interface->init();
}

void NodeMQTT::reconnectBroker()
{
    D_MQTT(DEVICE_NAME + " is attempting to connect to broker");
    if (brokerConnectingCallback != nullptr)
        brokerConnectingCallback();
    if (
        client.connect(DEVICE_NAME.c_str(),
                       (const char *)_config.mqttUser,
                       (const char *)_config.mqttPassword,
                       heartbeatInterface->getPublishTopic().c_str(),
                       1, true, WILL_MESSAGE))
    {
        D_MQTT(("Connected to broker!"));
        NodeInterfaceBase *interface;
        for (int i = 0; i < interfaceList.size(); i++)
        {
            interface = interfaceList.get(i);
            if (interface->hasMQTTSubscribe())
            {
                client.subscribe((interface->getSubscribeTopic()).c_str());
            }
        }
        heartbeatInterface->setEnabled(true);
        nodeConfigInterface->publishCurrentConfig(_config);
        buzz(TONE_SYSTEM_ONLINE);

        if (brokerConnectedCallback != nullptr)
            brokerConnectedCallback();
        _tBrokerConnect.disable();
    }
    else
    {
        buzz(TONE_WARNING);
        D_MQTT("Failed to connect broker, rc=" + client.state());
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
    D_MQTT("receiving payload=" + s_payload);
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

void NodeMQTT::setOnWifiConnected(NodeMQTTCallback cb)
{
    wifiConnectedCallback = cb;
}
void NodeMQTT::setOnBrokerConnected(NodeMQTTCallback cb)
{
    brokerConnectedCallback = cb;
}
void NodeMQTT::setOnWifiConnecting(NodeMQTTCallback cb)
{
    wifiConnectingCallback = cb;
}
void NodeMQTT::setOnBrokerConnecting(NodeMQTTCallback cb)
{
    brokerConnectingCallback = cb;
}