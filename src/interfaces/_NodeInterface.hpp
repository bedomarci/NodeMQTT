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
#include <functional>

#define ONCHANGE_CALLBACK_SIGNATURE std::function<void(T, T)>

class NodeInterfaceBase
{
  public:
    virtual void setClient(PubSubClient &client) = 0;
    virtual void setScheduler(Scheduler &scheduler) = 0;
    virtual String readRaw() = 0;
    virtual void writeRaw(String value, bool publishable = true) = 0;
    virtual void handle() = 0;
    virtual void setSampleRate(unsigned long sampleRate) = 0;
    virtual void init() = 0;

    virtual bool hasMQTTPublish() = 0;
    virtual bool hasMQTTSubscribe() = 0;
    virtual void setMQTTPublish(bool publish) = 0;
    virtual void setMQTTSubscribe(bool subscribe) = 0;
    virtual void setBaseTopic(String &baseTopic) = 0;
    virtual String getSubscribeTopic() = 0;
};

template <typename T>
class NodeInterface : public NodeInterfaceBase
{
  public:
    NodeInterface(String publishTopic, String subscribeTopic);
    void setClient(PubSubClient &client);
    void write(T newValue, bool publishable = true);
    T read();
    void onChange(ONCHANGE_CALLBACK_SIGNATURE);
    String readRaw();
    void writeRaw(String value, bool publishable);
    void handle();
    void setSampleRate(unsigned long sampleRate);
    void setScheduler(Scheduler &scheduler);
    void setInterfaceName(String &name);
    void init();
    void setEnabled(bool enabled);
    bool isEnabled();
    void setSamplingEnabled(bool enabled);
    bool hasMQTTPublish();
    bool hasMQTTSubscribe();
    void setMQTTPublish(bool publish);
    void setMQTTSubscribe(bool subscribe);
    void setBaseTopic(String &baseTopic);
    String getSubscribeTopic();

  protected:
    ONCHANGE_CALLBACK_SIGNATURE _onChangeCallback;

    //JSON
    JsonObject &fromString(String newValue);
    String toString(JsonObject &rootObject);

    //MQTT
    bool _hasMQTTPublish = true;
    bool _hasMQTTSubscribe = true;
    String interfaceName = "INTERFACE";
    PubSubClient _client;
    // NodeMQTT _device;
    void publish(T value);

    //SAMPLING + SCHEDULER
    Scheduler _scheduler;
    T currentValue;
    bool _samplingEnabled = true;
    bool _enabled = true;

    //VIRTUAL
    virtual T sample();
    virtual int cmp(T oldValue, T newValue) = 0;
    virtual void updatePhisicalInterface(T newValue);

    virtual T fromJSON(JsonObject &value) = 0;
    virtual JsonObject &toJSON(T value) = 0;

  private:
    unsigned long _sampleRate = DEFAULT_SAMPLE_RATE;
    Task _task;
    DynamicJsonBuffer jsonBuffer;
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
    _task.set(_sampleRate, TASK_FOREVER, [this]() { handle(); });
}

template <typename T>
inline void NodeInterface<T>::setClient(PubSubClient &client)
{
    _client = client;
}

template <typename T>
inline void NodeInterface<T>::onChange(ONCHANGE_CALLBACK_SIGNATURE onChangeCallback)
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
        PINT("Value changed on " + interfaceName);
        T oldValue = currentValue;
        currentValue = newValue;
        updatePhisicalInterface(newValue);
        if (publishable)
            publish(newValue);
        if (_onChangeCallback)
            _onChangeCallback(oldValue, newValue);
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
    if (_hasMQTTPublish && _client.connected())
    {
        String msg = toString(toJSON(value));
        //ITT VALAMI NEM STIMMEL
        _client.publish(_publishFullTopic.c_str(), msg.c_str());
        PMQTT("publishing=");
        PMQTT(msg);
    }
}

template <typename T>
inline void NodeInterface<T>::writeRaw(String newValue, bool publishable)
{
    write(fromJSON(fromString(newValue)), publishable);
}

template <typename T>
inline String NodeInterface<T>::readRaw()
{
    return toString(toJSON(read()));
}

template <typename T>
inline void NodeInterface<T>::handle()
{
    //SAMPLING
    if (_samplingEnabled)
        write(sample());
}

template <typename T>
inline JsonObject &NodeInterface<T>::fromString(String newValue)
{
    JsonObject &rootObject = jsonBuffer.parseObject(newValue);
    if (!rootObject.success())
        e("Failed to parse JSON data!");
    return rootObject;
}

template <typename T>
inline String NodeInterface<T>::toString(JsonObject &rootObject)
{
    String jsonStr;
    rootObject.printTo(jsonStr);
    return jsonStr;
}

template <typename T>
inline void NodeInterface<T>::setSampleRate(unsigned long sampleRate)
{
    _sampleRate = sampleRate;
    _task.setInterval(sampleRate);
}

template <typename T>
inline void NodeInterface<T>::setScheduler(Scheduler &scheduler)
{
    _scheduler = scheduler;
    _scheduler.addTask(_task);
    if (_enabled)
        _task.enableIfNot();
}

template <typename T>
inline void NodeInterface<T>::init()
{
}

template <typename T>
inline void NodeInterface<T>::setInterfaceName(String &name)
{
    interfaceName = name;
}

template <typename T>
inline void NodeInterface<T>::setEnabled(bool en = true)
{
    if (en == _enabled)
    {
        return;
    }
    _enabled = en;
    if (_enabled)
    {
        _task.enableIfNot();
    }
    else
    {
        _task.disable();
    }
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

#endif //NODEINTERFACE_H