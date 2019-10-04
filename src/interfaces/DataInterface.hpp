#ifndef DATAINTERFACE_H
#define DATAINTERFACE_H
#include "_NodeInterface.hpp"

template <typename T>
class DataInterface : public NodeInterface<T>
{
public:
  DataInterface(String publishTopic, String subscribeTopic);
  DataInterface(String topic);
  void init();
  String valueToString() override;

protected:
  T fromJSON(JsonObject &rootObject) override;
  JsonObject &toJSON(T value, JsonObject &root) override;
  int cmp(T oldValue, T newValue) override;
  void updatePhisicalInterface(T newValue) override;
};

template <typename T>
inline DataInterface<T>::DataInterface(String publishTopic, String subscribeTopic)
    : NodeInterface<T>(publishTopic, subscribeTopic)
{
  this->setSamplingEnabled(false);
  this->setMQTTPublish(true);
  this->setInterfaceName(DATA_NAME);
}

template <typename T>
inline DataInterface<T>::DataInterface(String topic)
    : DataInterface<T>(topic, topic)
{
}

template <typename T>
inline void DataInterface<T>::updatePhisicalInterface(T newValue) {}

template <typename T>
inline void DataInterface<T>::init()
{
}

template <typename T>
inline T DataInterface<T>::fromJSON(JsonObject &rootObject)
{
  T value = 0;
  if (rootObject["data"].is<T>())
  {
    value = rootObject["data"].as<T>();
  }
  return value;
}

template <typename T>
inline JsonObject &DataInterface<T>::toJSON(T value, JsonObject &root)
{
  root["data"] = value;
  return root;
}

template <typename T>
inline int DataInterface<T>::cmp(T oldValue, T newValue)
{
  return newValue - oldValue;
}

template <typename T>
inline String DataInterface<T>::valueToString()
{
  return String(this->read());
}

#endif //DATAINTERFACE_H