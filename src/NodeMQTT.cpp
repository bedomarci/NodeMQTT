#include "NodeMQTT.hpp"
#include "misc/constants.hpp"
#include "misc/helpers.hpp"

NodeMQTT::NodeMQTT()
{
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
    PMQTT("Initializing MQTT NODE");
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
    _tWifiConnect.enable();
    buzz(TONE_SYSTEM_BOOT);
}

void NodeMQTT::reconnectWifi()
{
    PNET("Connecting to " + String(_wifiSsid));
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
                delay(1500);
                PNET("WiFi connected. IP address: " + WiFi.localIP().toString());
                _tWifiConnect.disable();
            }
            if (!client.connected())
            {
                _tBrokerConnect.enableIfNot();
            }
        }
        else
        {
            heartbeatInterface->setEnabled(false);
            _tWifiConnect.enableIfNot();
        }
    }
    client.loop();
    if (_isServiceMode)
        readSerial();
    _scheduler.execute();
}

void NodeMQTT::addInterface(NodeInterfaceBase *interface)
{
    interfaceList.add(interface);
    interface->setClient(client);
    interface->setScheduler(_scheduler);
    interface->init();
}

void NodeMQTT::reconnectBroker()
{
    PMQTT(DEVICE_NAME + " is attempting to connect to broker");
    if (client.connect(DEVICE_NAME.c_str()))
    {
        PMQTT("connected to broker");
        heartbeatInterface->setEnabled(true);
        NodeInterfaceBase *interface;
        for (int i = 0; i < interfaceList.size(); i++)
        {
            interface = interfaceList.get(i);
            if (interface->hasMQTTSubscribe())
            {
                client.subscribe((interface->getSubscribeTopic()).c_str());
            }
        }
        buzz(TONE_SYSTEM_ONLINE);
        _tBrokerConnect.disable();
    }
    else
    {
        buzz(TONE_WARNING);
        PMQTT("failed to connect broker, rc=" + client.state());
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
    PMQTT("receiving payload=" + s_payload);
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
    heartbeatInterface = new HeartbeatInterface(1000);
    nodeConfigInterface = new NodeConfigInterface();
    addInterface(nodeConfigInterface);
    addInterface(heartbeatInterface);
    // addInterface(Logger.getLoggerInterface());
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