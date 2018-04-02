#define DEBUG_INTERFACE
#include <Arduino.h>
#include <NodeMQTT.hpp>
#include <BuzzerInterface.hpp>

NodeMQTT thisNode;

#define BUZZERPIN 8

BuzzerInterface* buzzer = new BuzzerInterface("buzzer", BUZZERPIN);
void callback(int oldVal, int newVal);
NodeMQTTConfig config;
int i = 0;


void setup() {
    Serial.begin(115200);

    buzzer->onChange(callback);
    thisNode.addInterface(buzzer);
    thisNode.setSystemBuzzer(buzzer);
    
    thisNode.begin();
}

void loop() {
    thisNode.handle();
}

void callback(int oldVal, int newVal) {
    Serial.print("old: ");
    Serial.print(oldVal);
    Serial.print(" new: ");
    Serial.println(newVal);
}