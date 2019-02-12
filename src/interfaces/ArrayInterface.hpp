#ifndef ARRAYINTERFACE_H
#define ARRAYINTERFACE_H
#include "_NodeInterface.hpp"
#include "../misc/helpers.hpp"

template <typename T, uint16_t LENGTH>
class ArrayInterface : public NodeInterface<Array<T, LENGTH>>
{
  public:
    ArrayInterface(String publishTopic, String subscribeTopic);
    ArrayInterface(String topic);
    void init();
    void writeItem(T newValue, uint16_t index, bool publishable = true);
    void fill(T value);
    void valueToString(String &sValue);

  protected:
    virtual Array<T, LENGTH> sample() override;
    virtual Array<T, LENGTH> fromJSON(JsonObject &rootObject) override;
    virtual JsonObject &toJSON(Array<T, LENGTH> value, JsonObject &root) override;
    virtual int cmp(Array<T, LENGTH> oldValue, Array<T, LENGTH> newValue) override;
    virtual void updatePhisicalInterface(Array<T, LENGTH> newValue) override;
};

template <typename T, uint16_t LENGTH>
inline ArrayInterface<T, LENGTH>::ArrayInterface(String publishTopic, String subscribeTopic)
    : NodeInterface<Array<T, LENGTH>>(publishTopic, subscribeTopic)
{
}

template <typename T, uint16_t LENGTH>
inline ArrayInterface<T, LENGTH>::ArrayInterface(String topic)
    : ArrayInterface<T, LENGTH>(topic, topic)
{
    this->setSamplingEnabled(false);
    this->setMQTTPublish(true);
    this->setInterfaceName(ARRAY_NAME);
}

template <typename T, uint16_t LENGTH>
inline void ArrayInterface<T, LENGTH>::init()
{
}

template <typename T, uint16_t LENGTH>
inline void ArrayInterface<T, LENGTH>::writeItem(T newItem, uint16_t index, bool publishable)
{
    Array<T, LENGTH> newValue = this->read();
    newValue[index] = newItem;
    this->write(newValue, publishable);
}

template <typename T, uint16_t LENGTH>
inline void ArrayInterface<T, LENGTH>::updatePhisicalInterface(Array<T, LENGTH> newValue) {}

template <typename T, uint16_t LENGTH>
inline Array<T, LENGTH> ArrayInterface<T, LENGTH>::sample()
{
    Array<T, LENGTH> arr;
    return arr;
}

template <typename T, uint16_t LENGTH>
inline Array<T, LENGTH> ArrayInterface<T, LENGTH>::fromJSON(JsonObject &rootObject)
{
    Array<T, LENGTH> value;
    bool canParse = false;
    //Check for array value
    //ex.: {array:[0,1,2,3]}
    if (rootObject.containsKey("array"))
    {
        JsonArray &jsonArray = rootObject["array"];
        if (jsonArray.size() != LENGTH)
        {
            e(F("Incoming JSON array length mismatch!"));
            return this->read();
        }
        for (int i = 0; i < LENGTH; i++)
        {
            value[i] = jsonArray[i].as<T>();
        }
        canParse = true;
    }
    //Check for single array items
    //ex.: {1: "a", 2: "b"}
    else
    {
        value = this->read();
        for (int i = 0; i < LENGTH; i++)
        {
            String sIdx = String(i);
            if (rootObject.containsKey(sIdx))
            {
                value[i] = rootObject[sIdx].as<T>();
                canParse = true;
            }
        }
    }
    if (!canParse)
    {
        e(F("Could not parse data from JSON!"));
    }
    return value;
}

template <typename T, uint16_t LENGTH>
inline JsonObject &ArrayInterface<T, LENGTH>::toJSON(Array<T, LENGTH> value, JsonObject &root)
{
    JsonArray &array = root.createNestedArray("array");
    for (int i = 0; i < value.length; i++)
    {
        array.add(value.item[i]);
    }
    return root;
}

template <typename T, uint16_t LENGTH>
inline int ArrayInterface<T, LENGTH>::cmp(Array<T, LENGTH> oldValue, Array<T, LENGTH> newValue)
{
    bool hasChange = false;
    for (int i = 0; i < LENGTH; i++)
    {
        hasChange |= oldValue.item[i] != newValue.item[i];
    }

    return hasChange;
}
template <typename T, uint16_t LENGTH>
inline void ArrayInterface<T, LENGTH>::fill(T value)
{
    Array<T, LENGTH> arr;
    array_fill<T, LENGTH>(arr, value);
    this->write(arr);
}

template <typename T, uint16_t LENGTH>
inline void ArrayInterface<T, LENGTH>::valueToString(String &sValue) {
    Array<T, LENGTH> arr = this->read();
    sValue = array_toString<T, LENGTH>(arr); //TODO KONVERTALAS
}

#endif //ARRAYINTERFACE_H