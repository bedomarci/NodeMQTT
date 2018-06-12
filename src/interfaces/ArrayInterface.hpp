#ifndef ARRAYINTERFACE_H
#define ARRAYINTERFACE_H
#include "_NodeInterface.hpp"

template <typename T, unsigned int LENGTH>
class ArrayInterface : public NodeInterface<Array<T, LENGTH>>
{
  public:
    ArrayInterface(String publishTopic, String subscribeTopic);
    ArrayInterface(String topic);
    void init();

  protected:
    Array<T, LENGTH> sample();
    Array<T, LENGTH> fromJSON(JsonObject &rootObject);
    JsonObject &toJSON(Array<T, LENGTH> value, JsonObject &root);
    int cmp(Array<T, LENGTH> oldValue, Array<T, LENGTH> newValue);
    void updatePhisicalInterface(Array<T, LENGTH> newValue);
};

template <typename T, unsigned int LENGTH>
inline ArrayInterface<T, LENGTH>::ArrayInterface(String publishTopic, String subscribeTopic)
    : NodeInterface<Array<T, LENGTH>>(publishTopic, subscribeTopic)
{
}

template <typename T, unsigned int LENGTH>
inline ArrayInterface<T, LENGTH>::ArrayInterface(String topic)
    : ArrayInterface<T, LENGTH>(topic, topic)
{
}

template <typename T, unsigned int LENGTH>
inline void ArrayInterface<T, LENGTH>::init()
{
    this->setSamplingEnabled(false);
    this->setMQTTPublish(true);
}

template <typename T, unsigned int LENGTH>
inline void ArrayInterface<T, LENGTH>::updatePhisicalInterface(Array<T, LENGTH> newValue) {}

template <typename T, unsigned int LENGTH>
inline Array<T, LENGTH> ArrayInterface<T, LENGTH>::sample()
{
    Array<T, LENGTH> arr;
    return arr;
}

template <typename T, unsigned int LENGTH>
inline Array<T, LENGTH> ArrayInterface<T, LENGTH>::fromJSON(JsonObject &rootObject)
{
    Array<T, LENGTH> value;
    JsonArray &jsonArray = rootObject["array"];
    if (jsonArray.size() != LENGTH)
    {
        e(F("Incoming JSON array length mismatch!"));
        return this->read();
    }
    for (int i = 0; i < LENGTH; i++)
    {
        value.item[i] = jsonArray[i].as<T>();
    }
    return value;
}

template <typename T, unsigned int LENGTH>
inline JsonObject &ArrayInterface<T, LENGTH>::toJSON(Array<T, LENGTH> value, JsonObject &root)
{
    JsonArray &array = root.createNestedArray("array");
    for (int i = 0; i < value.length; i++)
    {
        array.add(value.item[i]);
    }
    return root;
}

template <typename T, unsigned int LENGTH>
inline int ArrayInterface<T, LENGTH>::cmp(Array<T, LENGTH> oldValue, Array<T, LENGTH> newValue)
{
    bool hasChange = false;
    for (int i = 0; i < LENGTH; i++)
    {
        hasChange |= oldValue.item[i] != newValue.item[i];
    }

    return hasChange;
}

#endif //ARRAYINTERFACE_H