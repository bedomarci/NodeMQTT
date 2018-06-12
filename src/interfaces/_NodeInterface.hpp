#pragma once
#ifndef NODEINTERFACE_H
#define NODEINTERFACE_H

#define _TASK_INLINE

#include "misc/config.hpp"
#include "misc/constants.hpp"
#include "misc/helpers.hpp"
#include <Arduino.h>
#include <PubSubClient.h>
#include <LinkedList.h>
#include <ArduinoJson.h>
#include <TaskScheduler.h>
#include <ArduinoJson/Serialization/JsonSerializerImpl.hpp>
#include <functional>
#include "../transport/_AbstractTransport.hpp"





class NodeInterfaceBase
{
  public:
    virtual void setTransport(AbstractTransport *transport) = 0;
    virtual void setScheduler(Scheduler *scheduler) = 0;
    virtual void writeRaw(String value, bool publishable = true) = 0;
    virtual void handle() = 0;
    virtual void setSamplingRate(unsigned long samplingRate) = 0;
    virtual void init() = 0;
    virtual void setEnabled(bool enabled) = 0;
    virtual bool isEnabled() = 0;

    virtual bool hasMQTTPublish() = 0;
    virtual bool hasMQTTSubscribe() = 0;
    virtual void setMQTTPublish(bool publish) = 0;
    virtual void setMQTTSubscribe(bool subscribe) = 0;
    virtual void setBaseTopic(String &baseTopic) = 0;
    virtual String getSubscribeTopic() = 0;
    virtual String getPublishTopic() = 0;
};

template <typename T>
class NodeInterface : public NodeInterfaceBase
{
  public:
    NodeInterface(String publishTopic, String subscribeTopic);
    NodeInterface(String topic);
    void setTransport(AbstractTransport *transport);
    void write(T newValue, bool publishable = true);
    T read();
    void onChange(NodeMQTTChangeCallback);
    void writeRaw(String value, bool publishable);
    void handle();
    void setSamplingRate(unsigned long samplingRate);
    void setScheduler(Scheduler *scheduler);
    void setInterfaceName(String &name);
    void setEnabled(bool enabled = true);
    bool isEnabled();
    void setSamplingEnabled(bool enabled);
    bool hasMQTTPublish();
    bool hasMQTTSubscribe();
    void setMQTTPublish(bool publish);
    void setMQTTSubscribe(bool subscribe);
    void setBaseTopic(String &baseTopic);
    String getSubscribeTopic();
    String getPublishTopic();

  protected:
    NodeMQTTChangeCallback _onChangeCallback;

    //JSON
    JsonObject &fromString(String newValue, DynamicJsonBuffer &buffer);
    String toString(JsonObject &rootObject);

    //MQTT
    bool _hasMQTTPublish = true;
    bool _hasMQTTSubscribe = true;
    String _interfaceName = "INTERFACE";
    AbstractTransport *_transport = nullptr;
    // NodeMQTT _device;
    void publish(T value);

    //SAMPLING + SCHEDULER
    Scheduler *_scheduler = nullptr;
    T currentValue;
    void forceResample();

    //VIRTUAL
    virtual T sample();
    virtual int cmp(T oldValue, T newValue) = 0;
    virtual void updatePhisicalInterface(T newValue);

    virtual T fromJSON(JsonObject &value) = 0;
    virtual JsonObject &toJSON(T value, JsonObject &root) = 0;

    Task _task;

  private:
    bool _samplingEnabled = true;
    bool _forceResample = false;
    bool _enabled = true;
    unsigned long _samplingRate = DEFAULT_SAMPLE_RATE;
    String _publishTopic;
    String _subscribeTopic;
    String _publishFullTopic;
    String _subscribeFullTopic;
};

template <typename T>
inline NodeInterface<T>::NodeInterface(String publishTopic, String subscribeTopic)
{
    //TODO concat base topic
    _publishTopic = publishTopic;
    _subscribeTopic = subscribeTopic;
    _publishFullTopic = publishTopic;
    _subscribeFullTopic = subscribeTopic;
    _task.set(_samplingRate, TASK_FOREVER, [this]() { handle(); });
}

template <typename T>
inline NodeInterface<T>::NodeInterface(String topic)
    : NodeInterface(topic, topic)
{
}

template <typename T>
inline void NodeInterface<T>::setTransport(AbstractTransport *transport)
{
    _transport = transport;
}

template <typename T>
inline void NodeInterface<T>::onChange(NodeMQTTChangeCallback onChangeCallback)
{
    _onChangeCallback = onChangeCallback;
}

template <typename T>
inline T NodeInterface<T>::sample()
{
    return currentValue;
}

template <typename T>
inline void NodeInterface<T>::updatePhisicalInterface(T newValue)
{
}

template <typename T>
inline int NodeInterface<T>::cmp(T oldValue, T newValue)
{
    return -1;
}

template <typename T>
inline void NodeInterface<T>::write(T newValue, bool publishable)
{
    if (cmp(newValue, currentValue) != 0)
    {
        D_INTR("Value changed on " + _interfaceName + " @ " + _publishTopic);
        T oldValue = currentValue;
        currentValue = newValue;
        updatePhisicalInterface(newValue);
        if (publishable)
        {
            publish(newValue);
        }
        if (_onChangeCallback)
        {
            _onChangeCallback(oldValue, newValue);
        }
    }
}

template <typename T>
inline T NodeInterface<T>::read()
{
    return currentValue;
}

template <typename T>
inline void NodeInterface<T>::publish(T value)
{
    if (_transport == nullptr)
        return;

    if (_hasMQTTPublish && _transport->isNetworkConnected())
    {
        DynamicJsonBuffer jsonBuffer;
        JsonObject &root = jsonBuffer.createObject();
        String msg = toString(toJSON(value, root));
        _transport->publish(_publishFullTopic.c_str(), msg.c_str());
        D_MQTT("Publishing: " + msg);
    }
}

template <typename T>
inline void NodeInterface<T>::writeRaw(String newValue, bool publishable)
{
    DynamicJsonBuffer jsonBuffer;
    JsonObject &rootObject = fromString(newValue, jsonBuffer);
    if (rootObject.success())
        write(fromJSON(rootObject), publishable);
    else
        e("Failed to parse JSON data @ " + getSubscribeTopic());
}

template <typename T>
inline void NodeInterface<T>::handle()
{
    //SAMPLING
    if (_samplingEnabled)
    {
        do
        {
            _forceResample = false;
            write(sample());
        } while (_forceResample);
    }
}

template <typename T>
inline void NodeInterface<T>::forceResample() { _forceResample = true; }

template <typename T>
inline JsonObject &NodeInterface<T>::fromString(String newValue, DynamicJsonBuffer &buffer)
{
    return buffer.parseObject(newValue);
}

template <typename T>
inline String NodeInterface<T>::toString(JsonObject &rootObject)
{
    String jsonStr;
    rootObject.printTo(jsonStr);
    return jsonStr;
}

template <typename T>
inline void NodeInterface<T>::setSamplingRate(unsigned long samplingRate)
{
    _samplingRate = samplingRate;
    _task.setInterval(samplingRate);
}

template <typename T>
inline void NodeInterface<T>::setScheduler(Scheduler *scheduler)
{
    _scheduler = scheduler;
    _scheduler->addTask(_task);
    if (_enabled)
    {
        _task.enableIfNot();
    }
}

template <typename T>
inline void NodeInterface<T>::setInterfaceName(String &name)
{
    _interfaceName = name;
}

template <typename T>
inline void NodeInterface<T>::setEnabled(bool en)
{
    if (en == _enabled)
        return;
    _enabled = en;
    (_enabled) ? _task.enableIfNot() : _task.disable();
}
template <typename T>
inline bool NodeInterface<T>::isEnabled()
{
    return _enabled;
}

template <typename T>
inline void NodeInterface<T>::setSamplingEnabled(bool en)
{
    _samplingEnabled = en;
}

template <typename T>
inline bool NodeInterface<T>::hasMQTTPublish()
{
    return _hasMQTTPublish;
}

template <typename T>
inline bool NodeInterface<T>::hasMQTTSubscribe()
{
    return _hasMQTTSubscribe;
}
template <typename T>
inline void NodeInterface<T>::setMQTTPublish(bool publish)
{
    _hasMQTTPublish = publish;
}
template <typename T>
inline void NodeInterface<T>::setMQTTSubscribe(bool subscribe)
{
    _hasMQTTSubscribe = subscribe;
}
template <typename T>
inline void NodeInterface<T>::setBaseTopic(String &baseTopic)
{
    _subscribeFullTopic = baseTopic + "/" + _subscribeTopic;
    _publishFullTopic = baseTopic + "/" + _publishTopic;
}
template <typename T>
inline String NodeInterface<T>::getSubscribeTopic()
{
    return _subscribeFullTopic;
}

template <typename T>
inline String NodeInterface<T>::getPublishTopic()
{
    return _publishFullTopic;
}
#endif //NODEINTERFACE_H