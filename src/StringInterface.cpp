
#include "StringInterface.hpp"
StringInterface::StringInterface(String publishTopic, String subscribeTopic)
    : NodeInterface<String>(publishTopic, subscribeTopic)
{
    samplingEnabled = false;
    hasMQTTPublish = false;
}


String StringInterface::fromJSON (JsonObject& rootObject) {
    String value = rootObject["data"].as<String>();
    return value;
}

JsonObject& StringInterface::toJSON (String value){
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["data"] = value;
    return root;
}


int StringInterface::cmp(String oldValue, String newValue){
    return !(oldValue==newValue);
}
