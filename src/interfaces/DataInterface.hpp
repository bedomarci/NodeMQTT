#ifndef DATAINTERFACE_H
#define DATAINTERFACE_H
#include "_NodeInterface.hpp"

template <typename T>
class DataInterface : public NodeInterface<T>
{
  public:
    DataInterface(String topic);

  protected:
    void updatePhisicalInterface(int newValue);
};

template <typename T>
inline DataInterface<T>::DataInterface(String topic, uint8_t btnPin, unsigned long debounceDelay) : NodeInterface<T>(topic, topic)
{
    setSamplingEnabled(false);
    setMQTTPublish(true);
}

template <typename T>
inline void DataInterface<T>::updatePhisicalInterface(int newValue)
{
}

#endif //BUTTONINTERFACE_H