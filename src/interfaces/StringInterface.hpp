#ifndef StringInterface_H
#define StringInterface_H
#include "_NodeInterface.hpp"

class StringInterface : public NodeInterface<String>
{
  public:
    StringInterface(String topic);
    StringInterface(String publishTopic, String subscribeTopic);
    void init();
    void valueToString(String &sValue);

  protected:
    String parsedValue;
    String fromJSON(JsonObject &rootObject) override;
    JsonObject &toJSON(String value, JsonObject &root) override;
    int cmp(String oldValue, String newValue) override;
};

inline StringInterface::StringInterface(String publishTopic, String subscribeTopic) : NodeInterface<String>(publishTopic, subscribeTopic)
{
}
inline StringInterface::StringInterface(String topic) : NodeInterface<String>(topic, topic)
{
    this->setSamplingEnabled(false);
    this->setMQTTPublish(false);
    // setMQTTPublish(this->getSubscribeTopic() != this->getPublishTopic());
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
    _interfaceName = STRING_NAME;
}

inline void StringInterface::valueToString(String &sValue)
{
    sValue = currentValue;
}

#endif //StringInterface_H