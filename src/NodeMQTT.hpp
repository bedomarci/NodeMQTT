#ifndef NODEMQTT_H
#define NODEMQTT_H
#include "misc/config.hpp"
#include <ESP8266WiFi.h>
#include "_NodeInterface.hpp"
#include "NodeMQTTConfigManager.hpp"
#include "NodeMQTTUpdateManager.hpp"
#include "HeartbeatInterface.hpp"
#include "NodeConfigInterface.hpp"
#include "BuzzerInterface.hpp"

class NodeMQTT {
    public:     
        NodeMQTT();
        void begin();
        void begin(NodeMQTTConfig &configuration);
        void handle();
        void addInterface(NodeInterfaceBase*);

        void setBaseTopic(String baseTopic);

        static const char * getMQTTDeviceName();
        void setSystemBuzzer(BuzzerInterface* interface);
        void buzz(int);

    protected:
        bool _isSerialDebugging = false;
        bool _isOnline = true;
        bool _isServiceMode = false;
        byte* _serialBuffer=0;
        const char* _wifiSsid;
        const char* _wifiPassword;
        const char* _mqttServer;
        const char* _mqttUser;
        const char* _mqttPass;
        WiFiClient espClient;
        PubSubClient client;

        //SCHEDULER
        Scheduler _scheduler;
        Task _tWifiConnect;
        Task _tBrokerConnect;
        Task _tReadSerial;

        NodeMQTTConfig _config;
        
        LinkedList<NodeInterfaceBase*> interfaceList;
        String _baseTopic = NodeMQTT::_MQTTDeviceName;

        void reconnectBroker();
        bool isConnectionAlive();
        void reconnectWifi();
        void readSerial();
        void mqttParser(char* topic, byte* payload, unsigned int length);
        void addDefaultInterfaces();

        //STATIC
        static String _MQTTDeviceName;

        //BUILTIN INTERFACES
        HeartbeatInterface* heartbeatInterface;
        NodeConfigInterface* nodeConfigInterface;
        BuzzerInterface* buzzerInterface;

};
#endif //NODEMQTT_H