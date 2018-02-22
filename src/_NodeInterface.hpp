#pragma once
#ifndef NODEINTERFACE_H
#define NODEINTERFACE_H

#define _TASK_INLINE

#include "misc/config.hpp"
#include <Arduino.h>
#include <PubSubClient.h>
#include <LinkedList.h>
#include <ArduinoJson.h>
#include <TaskScheduler.h>
#include <functional>

#define ONCHANGE_CALLBACK_SIGNATURE  std::function<void(T, T)>

class NodeInterfaceBase{
public:
    String _publishTopic;
    String _subscribeTopic;
    // Scheduler _scheduler;

    bool hasMQTTPublish = true;
    bool hasMQTTSubscribe = true;

    virtual void setClient(PubSubClient client)=0;
    virtual String readRaw()=0;
    virtual void writeRaw(String value, bool publishable = true)=0;
    virtual void handle()=0;
    virtual void setSampleRate(unsigned long sampleRate)=0;
    virtual void setScheduler(Scheduler& scheduler)=0;
    virtual void init()=0;
};

template <typename T>
class NodeInterface : public NodeInterfaceBase {
public:
    NodeInterface(String publishTopic, String subscribeTopic);
    void setClient(PubSubClient client);
    void write(T newValue, bool publishable = true);
    T read ();
    void onChange(ONCHANGE_CALLBACK_SIGNATURE);
    //void subscribe();
    String readRaw();
    void writeRaw(String value, bool publishable);
    void handle();
    void setSampleRate(unsigned long sampleRate);
    void setScheduler(Scheduler& scheduler);
    void setInterfaceName(String& name);
    void init();
    void enable(bool enabled);
protected:
    ONCHANGE_CALLBACK_SIGNATURE _onChangeCallback;
    
    //JSON
    JsonObject& fromString(String newValue);
    String toString(JsonObject& rootObject);
    
    //MQTT
    String interfaceName = "INTERFACE";
    PubSubClient _client;
    void publish(T value);
    
    //SAMPLING + SCHEDULER
    Scheduler * _scheduler;
    T currentValue;
    bool samplingEnabled = true;
    bool _enabled = true;
    
    //VIRTUAL
    virtual T sample()=0;
    virtual int cmp(T oldValue, T newValue)=0;
    virtual void updatePhisicalInterface(T newValue)=0;
    
    virtual T fromJSON (JsonObject& value) =0;
    virtual JsonObject& toJSON (T value) =0;
private:
    unsigned long _sampleRate = DEFAULT_SAMPLE_RATE;
    Task _task;
};


template <typename T>
inline
NodeInterface<T>::NodeInterface(String publishTopic,String subscribeTopic)
{
    //TODO concat base topic
    _publishTopic = publishTopic;
    _subscribeTopic = subscribeTopic;
    _task.set(_sampleRate, TASK_FOREVER, [this](){handle();});
    
}

template <typename T>
inline
void NodeInterface<T>::setClient(PubSubClient client) {
    _client = client;
}

template <typename T>
inline
void NodeInterface<T>::onChange(ONCHANGE_CALLBACK_SIGNATURE onChangeCallback)
{
    _onChangeCallback = onChangeCallback;
}

template <typename T>
inline
T NodeInterface<T>::sample() {
    return currentValue;
}

template <typename T>
inline
int NodeInterface<T>::cmp(T oldValue, T newValue){
    return -1;
}

template <typename T>
inline
void NodeInterface<T>::write(T newValue, bool publishable) {
    if (cmp(newValue, currentValue) != 0) {
        PINT("Value changed on "+interfaceName);
        T oldValue = currentValue;
        currentValue = newValue;
        updatePhisicalInterface(newValue);
        if (publishable) publish(newValue);
        if (_onChangeCallback) _onChangeCallback(oldValue, newValue);
    }
}

template <typename T>
inline
T NodeInterface<T>::read() {
    return currentValue;
}

template <typename T>
inline
void NodeInterface<T>::publish(T value) {
    if (hasMQTTPublish && _client.connected()) {
        String msg = toString(toJSON(value));
        _client.publish(_publishTopic.c_str(),msg.c_str());
        PMQTT("publishing=");
        PMQTT(msg);
    }
}

template <typename T>
inline
void NodeInterface<T>::writeRaw(String newValue, bool publishable) {
    write(fromJSON(fromString(newValue)), publishable);
}


template <typename T>
inline
String  NodeInterface<T>::readRaw() {
    return toString(toJSON(read()));
}

template <typename T>
inline
void NodeInterface<T>::handle() {
    //SAMPLING
    if (samplingEnabled) write(sample());
}

template <typename T>
inline
JsonObject& NodeInterface<T>::fromString(String newValue) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& rootObject = jsonBuffer.parseObject(newValue);
    return rootObject;
}

template <typename T>
inline
String NodeInterface<T>::toString(JsonObject& rootObject) {
    String jsonStr;
    rootObject.printTo(jsonStr);
    return jsonStr;
}

template <typename T>
inline
void NodeInterface<T>::setSampleRate(unsigned long sampleRate) {
    _sampleRate = sampleRate;
    _task.setInterval(sampleRate);
}

template <typename T>
inline
void NodeInterface<T>::setScheduler(Scheduler& scheduler) {
    _scheduler = &scheduler;
    _scheduler->addTask(_task);
    if (_enabled) _task.enable();
}

template <typename T>
inline
void NodeInterface<T>::init() {
}

template <typename T>
inline
void NodeInterface<T>::setInterfaceName(String& name) {
    interfaceName = name;
}

template <typename T>
inline
void NodeInterface<T>::enable(bool en) {
    _enabled = en;
    if(en) {
        _task.enableIfNot();
    } else {
        _task.disable();
    }

}

#endif //NODEINTERFACE_H