#pragma once
#ifndef StringInterface_H
#define StringInterface_H
#include "_NodeInterface.hpp"

class StringInterface : public NodeInterface<String>
{
  public:
    StringInterface(String topic);

  protected:
    String fromJSON(JsonObject &rootObject);
    JsonObject &toJSON(String value);
    int cmp(String oldValue, String newValue);
};

inline StringInterface::StringInterface(String topic)
    : NodeInterface<String>(topic, topic)
{
    setSamplingEnabled(false);
    setMQTTPublish(false);
    interfaceName = STRING_NAME;
}

inline String StringInterface::fromJSON(JsonObject &rootObject)
{
    String value = rootObject["data"].as<String>();
    return value;
}

inline JsonObject &StringInterface::toJSON(String value)
{
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["data"] = value;
    return root;
}

inline int StringInterface::cmp(String oldValue, String newValue)
{
    return -1;
}

#endif //StringInterface_H