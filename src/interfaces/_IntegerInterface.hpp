#pragma once
#ifndef INTEGERINTERFACE_H
#define INTEGERINTERFACE_H
#include "_NodeInterface.hpp"

class IntegerInterface : public NodeInterface<int>
{
  public:
    IntegerInterface(String publishTopic, String subscribeTopic);
    virtual void init();

  protected:
    int fromJSON(JsonObject &rootObject);
    JsonObject &toJSON(int value, JsonObject &root);
    int cmp(int oldValue, int newValue);
    virtual int sample() = 0;
    virtual void updatePhisicalInterface(int newValue) = 0;
};

inline IntegerInterface::IntegerInterface(String publishTopic, String subscribeTopic)
    : NodeInterface<int>(publishTopic, subscribeTopic)
{
}

inline int IntegerInterface::fromJSON(JsonObject &rootObject)
{
    int value = rootObject["data"].as<int>();
    return value;
}

inline JsonObject &IntegerInterface::toJSON(int value, JsonObject &root)
{
    root["data"] = value;
    return root;
}

inline int IntegerInterface::cmp(int oldValue, int newValue)
{
    return newValue - oldValue;
}

inline void IntegerInterface::init()
{
    setEnabled();
}

#endif //INTEGERINTERFACE_H