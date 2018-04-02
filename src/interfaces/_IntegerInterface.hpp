#pragma once
#ifndef INTEGERINTERFACE_H
#define INTEGERINTERFACE_H
#include "_NodeInterface.hpp"

class IntegerInterface : public NodeInterface<int>
{
  public:
    IntegerInterface(String publishTopic, String subscribeTopic);

  protected:
    int fromJSON(JsonObject &rootObject);
    JsonObject &toJSON(int value);
    int cmp(int oldValue, int newValue);
};

inline IntegerInterface::IntegerInterface(String publishTopic, String subscribeTopic)
    : NodeInterface<int>(publishTopic, subscribeTopic)
{
    _samplingEnabled = false;
}

inline int IntegerInterface::fromJSON(JsonObject &rootObject)
{
    int value = rootObject["data"].as<int>();
    return value;
}

inline JsonObject &IntegerInterface::toJSON(int value)
{
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["data"] = value;
    return root;
}

inline int IntegerInterface::cmp(int oldValue, int newValue)
{
    return newValue - oldValue;
}

#endif //INTEGERINTERFACE_H