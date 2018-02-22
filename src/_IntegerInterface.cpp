
#include "_IntegerInterface.hpp"
IntegerInterface::IntegerInterface(String publishTopic, String subscribeTopic)
    : NodeInterface<int>(publishTopic, subscribeTopic)
{
    
}


int IntegerInterface::fromJSON (JsonObject& rootObject) {
    int value = rootObject["data"].as<int>();
    return value;
}

JsonObject& IntegerInterface::toJSON (int value){
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["data"] = value;
    return root;
}


int IntegerInterface::cmp(int oldValue, int newValue){
    return newValue-oldValue;
}
