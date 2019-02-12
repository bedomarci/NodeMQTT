#pragma once
#ifndef NODEINTERFACE_H
#define NODEINTERFACE_H

#define _TASK_INLINE

#include "misc/config.hpp"
#include "constants.hpp"
#include "misc/helpers.hpp"
#include <Arduino.h>
#include <PubSubClient.h>
#include <LinkedList.h>
#include <ArduinoJson.h>
#include <TaskScheduler.h>
#include <ArduinoJson/Serialization/JsonSerializerImpl.hpp>
#include <functional>
#include "../transport/_AbstractTransport.hpp"
#include "../NodeMQTTLogger.hpp"

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
    virtual void valueToString(String &sValue) = 0;

    virtual bool hasMQTTPublish() = 0;
    virtual bool hasMQTTSubscribe() = 0;
    virtual void setMQTTPublish(bool publish) = 0;
    virtual void setMQTTSubscribe(bool subscribe) = 0;
    virtual void setBaseTopic(String baseTopic) = 0;
    virtual String getBaseTopic() = 0;
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
    void setInterfaceName(String name);
    void setEnabled(bool enabled = true);
    bool isEnabled();
    void setSamplingEnabled(bool enabled);
    bool isSamplingEnabled();
    bool hasMQTTPublish();
    bool hasMQTTSubscribe();
    void setMQTTPublish(bool publish);
    void setMQTTSubscribe(bool subscribe);
    void setMQTTPublishSubscribe(bool publish, bool subscribe);
    void setBaseTopic(String baseTopic);
    void triggerCallback();
    String getBaseTopic();
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
    String _interfaceName = INTERFACE_NAME;
    AbstractTransport *_transport = nullptr;
    void publish(T value);

    //SAMPLING + SCHEDULER
    Scheduler *getScheduler();
    T currentValue;
    // String sCurrentValue;
    void forceResample();
    void preventDebugLogging(bool prevent = true);

    //VIRTUAL
    virtual T sample();
    virtual void updatePhisicalInterface(T newValue);
    // virtual void valueToString(T value, String &sValue);
    //PURE VIRTUAL
    virtual int cmp(T oldValue, T newValue) = 0;
    virtual T fromJSON(JsonObject &value) = 0;
    virtual JsonObject &toJSON(T value, JsonObject &root) = 0;

    Task _task;

  private:
    bool _samplingEnabled = true;
    bool _forceResample = false;
    bool _preventDebugLogging = false;
    bool _enabled = true;
    bool _valueInitialized = false;
    unsigned long _samplingRate = DEFAULT_SAMPLE_RATE;

    String _baseTopic = String();
    String _publishTopic;
    String _subscribeTopic;
    Scheduler *_scheduler = nullptr;
};

template <typename T>
inline NodeInterface<T>::NodeInterface(String publishTopic, String subscribeTopic)
{
    //TODO concat base topic
    _publishTopic = publishTopic;
    _subscribeTopic = subscribeTopic;
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

// template <typename T>
// inline void NodeInterface<T>::valueToString(T value, String &sValue)
// {
//     sValue = String(value);
// }

template <typename T>
inline int NodeInterface<T>::cmp(T oldValue, T newValue)
{
    return -1;
}

template <typename T>
inline void NodeInterface<T>::write(T newValue, bool publishable)
{
    if (!_enabled)
        return;
    if (cmp(newValue, currentValue) != 0 or !_valueInitialized)
    {
        if (!_preventDebugLogging)
        {
            String sValue;
            this->valueToString(sValue);
            Logger.logf(DEBUG, MSG_VALUE_CHANGED, _interfaceName.c_str(), _publishTopic.c_str(), publishable, sValue.c_str());
        }
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
    _valueInitialized = true;
}

template <typename T>
inline T NodeInterface<T>::read()
{
    return currentValue;
}

template <typename T>
inline void NodeInterface<T>::publish(T value)
{
    if (_transport == nullptr || !_enabled)
        return;

    if (_hasMQTTPublish && _transport->isNetworkConnected())
    {
        DynamicJsonBuffer jsonBuffer;
        JsonObject &root = jsonBuffer.createObject();
        String msg = toString(toJSON(value, root));
        _transport->publish(this->getPublishTopic().c_str(), msg.c_str());
        // Logger.logf(DEBUG, MSG_PUBLISHING, msg.c_str());
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
        Logger.logf(DEBUG, MSG_FAILED_TO_PARSE, getSubscribeTopic().c_str());
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
inline void NodeInterface<T>::preventDebugLogging(bool prevent) { _preventDebugLogging = prevent; }

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
inline void NodeInterface<T>::setInterfaceName(String name)
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
inline bool NodeInterface<T>::isSamplingEnabled()
{
    return _samplingEnabled;
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
inline void NodeInterface<T>::setMQTTPublishSubscribe(bool publish, bool subscribe)
{
    _hasMQTTPublish = publish;
    _hasMQTTSubscribe = subscribe;
}
template <typename T>
inline void NodeInterface<T>::setBaseTopic(String baseTopic)
{
    _baseTopic = baseTopic;
    // _subscribeFullTopic = baseTopic + "/" + _subscribeTopic;
    // _publishFullTopic = baseTopic + "/" + _publishTopic;
}
template <typename T>
inline void NodeInterface<T>::triggerCallback()
{
    if (_onChangeCallback)
    {
        _onChangeCallback(currentValue, currentValue);
    }
}
template <typename T>
inline String NodeInterface<T>::getBaseTopic()
{
    return _baseTopic;
}

template <typename T>
inline String NodeInterface<T>::getSubscribeTopic()
{
    // return _subscribeFullTopic;
    return _baseTopic + "/" + _subscribeTopic;
}

template <typename T>
inline String NodeInterface<T>::getPublishTopic()
{
    // return _publishFullTopic;
    return _baseTopic + "/" + _publishTopic;
}

template <typename T>
inline Scheduler *NodeInterface<T>::getScheduler()
{
    return _scheduler;
}
#endif //NODEINTERFACE_H