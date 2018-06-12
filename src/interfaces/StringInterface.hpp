#ifndef StringInterface_H
#define StringInterface_H
#include "_NodeInterface.hpp"

class StringInterface : public NodeInterface<String>
{
  public:
    StringInterface(String topic);
    void init();

  protected:
    String parsedValue;
    String fromJSON(JsonObject &rootObject);
    JsonObject &toJSON(String value, JsonObject &root);
    int cmp(String oldValue, String newValue);
};

inline StringInterface::StringInterface(String topic)
    : NodeInterface<String>(topic, topic)
{
    _interfaceName = STRING_NAME;
}

inline String StringInterface::fromJSON(JsonObject &rootObject)
{
    parsedValue = rootObject["data"] | currentValue.c_str();
    return parsedValue;
}

inline JsonObject &StringInterface::toJSON(String value, JsonObject &root)
{
    root["data"] = value;
    return root;
}

inline int StringInterface::cmp(String oldValue, String newValue)
{
    return -1;
}

inline void StringInterface::init()
{
    setSamplingEnabled(false);
    setMQTTPublish(false);
}

#endif //StringInterface_H