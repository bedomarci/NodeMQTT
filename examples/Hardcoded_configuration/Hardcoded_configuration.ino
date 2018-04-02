#include <Arduino.h>
#include <NodeMQTT.hpp>
#include <RelayInterface.hpp>
#include <BuzzerInterface.hpp>

NodeMQTT thisNode;
NodeMQTTConfig config;


void setup() {
    Serial.begin(115200);

   
    strcpy(config.wifiSsid, "your ssid");
    strcpy(config.wifiPassword, "your pass");
    strcpy(config.mqttServer, "mqtt server ip address"); //192.168.0.100
    strcpy(config.baseTopic, "base topic"); // your topics will look like: /[basetopic]/[interface topic]/[interface value]
    config.configVersion=1; //incremental numbering 

    thisNode.begin(config);
}

void loop() {
    thisNode.handle();
}
