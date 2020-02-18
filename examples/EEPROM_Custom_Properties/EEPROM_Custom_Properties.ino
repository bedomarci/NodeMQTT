#include <Arduino.h>
#include "../../src/NodeMQTT.hpp"
#include "../../src/NodeMQTTConfigManager.hpp"

NodeMQTT thisNode;




void setup() {

    NodeMQTTConfigManager.registerIntProperty(1,"elso",5);
    NodeMQTTConfigManager.registerStringProperty(2,"masodik","asdasd");

    thisNode.begin();
}

void loop() {
    thisNode.handle();
}