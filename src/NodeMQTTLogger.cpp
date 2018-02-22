#include "NodeMQTTLogger.hpp"

 void NodeMQTTLoggerClass::log(const char * message, LOG_LEVEL level){
    Serial.println(message);
    //MQTT LOG
}
 void NodeMQTTLoggerClass::log(int message, LOG_LEVEL level){
    Serial.println(message);
    //MQTT LOG
}
 void NodeMQTTLoggerClass::log(const __FlashStringHelper * message, LOG_LEVEL level){
    Serial.println(message);
    //MQTT LOG
}
 void NodeMQTTLoggerClass::log(String& message, LOG_LEVEL level){
    Serial.println(message);
    //MQTT LOG
}

NodeMQTTLoggerClass Logger;