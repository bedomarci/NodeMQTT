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

protected:
  T fromJSON(JsonObject &rootObject);
  JsonObject &toJSON(T value, JsonObject &root);
  int cmp(T oldValue, T newValue);
  void updatePhisicalInterface(T newValue);
};

template <typename T>
inline DataInterface<T>::DataInterface(String publishTopic, String subscribeTopic)
    : NodeInterface<T>(publishTopic, subscribeTopic)
{
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
  this->setSamplingEnabled(false);
  this->setMQTTPublish(true);
}

template <typename T>
inline T DataInterface<T>::fromJSON(JsonObject &rootObject)
{
  T value = 0;
  if (rootObject["data"].is<T>())
    value = rootObject["data"].as<T>();
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

#endif //BUTTONINTERFACE_H