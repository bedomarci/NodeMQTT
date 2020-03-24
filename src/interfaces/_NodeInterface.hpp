#ifndef NODEINTERFACE_H
#define NODEINTERFACE_H

#define _TASK_INLINE

#include "../misc/config.hpp"
#include "../misc/helpers.hpp"
#include "../constants.hpp"
#include <Arduino.h>
#include <LinkedList.h>
#include <ArduinoJson.h>
#include <TaskScheduler.h>
#include "misc/typedef.hpp"
#include "transports/_AbstractTransport.hpp"
#include "parsers/_AbstractParser.hpp"
#include "../NodeMQTTLogger.hpp"


template<typename T>
class NodeInterface : public NodeInterfaceBase {
public:
    NodeInterface(String publishTopic, String subscribeTopic);

    explicit NodeInterface(String topic);

    void write(T newValue, bool publishable = true);

    T read();

    void onChange(NodeMQTTChangeCallback);

    void writeRaw(String value, bool publishable) override;

    void handle() override;

    void setSamplingRate(unsigned long samplingRate) override;

    void setContext(ApplicationContext *context) override;

    void setInterfaceName(String name);

    void setEnabled(bool enabled = true) override;

    void enable();

    void disable();

    bool isEnabled();

    void setSamplingEnabled(bool enabled);

    bool isSamplingEnabled();

    bool hasMQTTPublish();

    bool hasMQTTSubscribe();

    bool hasInitializedValue() override;

    void setMQTTPublish(bool publish) override;

    void setMQTTSubscribe(bool subscribe) override;

    void setMQTTPublishSubscribe(bool publish, bool subscribe);

    void setBaseTopic(String baseTopic) override;

    void triggerCallback();

    void preventDebugLogging(bool prevent = true);

    void republish() override;

    String getBaseTopic();

    String getSubscribeTopic() override;

    String getPublishTopic() override;

protected:
    NodeMQTTChangeCallback
            _onChangeCallback;

    //JSON
    String toString(JsonObject rootObject);

    //MQTT
    bool _hasMQTTPublish = true;
    bool _hasMQTTSubscribe = true;
    String _interfaceName = INTERFACE_NAME;

    void publish(T value);

    //SAMPLING + SCHEDULER
    Scheduler *getScheduler();

    T currentValue;

    void forceResample();

    ApplicationContext *getContext();

    //VIRTUAL
    virtual T sample();

    virtual void updatePhisicalInterface(T newValue);

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
    // Scheduler *_scheduler = nullptr;
    ApplicationContext *_context = nullptr;
};

template<typename T>
inline NodeInterface<T>::NodeInterface(String publishTopic, String subscribeTopic) {
    _publishTopic = publishTopic;
    _subscribeTopic = subscribeTopic;
    _task.set(_samplingRate, TASK_FOREVER, [this]() { handle(); });
}

template<typename T>
inline NodeInterface<T>::NodeInterface(String topic)
        : NodeInterface(topic, topic) {
}

template<typename T>
inline void NodeInterface<T>::onChange(NodeMQTTChangeCallback onChangeCallback) {
    _onChangeCallback = onChangeCallback;
}

template<typename T>
inline T NodeInterface<T>::sample() {
    return currentValue;
}

template<typename T>
inline void NodeInterface<T>::updatePhisicalInterface(T newValue) {
}

template<typename T>
inline int NodeInterface<T>::cmp(T oldValue, T newValue) {
    return -1;
}

template<typename T>
inline void NodeInterface<T>::write(T newValue, bool publishable) {
    if (!_enabled)
        return;

    if (cmp(newValue, currentValue) != 0 or !_valueInitialized) {
        T oldValue = currentValue;
        currentValue = newValue;
        updatePhisicalInterface(newValue);
        if (publishable) {
            publish(newValue);
        }
        if (!_preventDebugLogging) {
            String sValue = this->valueToString();
            Logger.logf(DEBUG, MSG_VALUE_CHANGED, _interfaceName.c_str(), _publishTopic.c_str(), publishable,
                        sValue.c_str());
        }
        if (_onChangeCallback) {
            _onChangeCallback(oldValue, newValue);
        }
    }
    _valueInitialized = true;
}

template<typename T>
inline T NodeInterface<T>::read() {
    return currentValue;
}

template<typename T>
inline void NodeInterface<T>::publish(T value) {
    if (!getContext() || getContext()->transport == nullptr || !_enabled)
        return;

    if (_hasMQTTPublish && getContext()->transport->isNetworkConnected()) {
        DynamicJsonDocument jsonDocument(JSON_DOCUMENT_SIZE);
        JsonObject root = jsonDocument.to<JsonObject>();
        String msg = toString(toJSON(value, root));
        getContext()->transport->publish(this->getPublishTopic().c_str(), msg.c_str());
    }
}

template<typename T>
inline void NodeInterface<T>::republish() {
    this->publish(this->read());
}

template<typename T>
inline void NodeInterface<T>::writeRaw(String newValue, bool publishable) {
    DynamicJsonDocument jsonDocument(JSON_DOCUMENT_SIZE);
    auto error = deserializeJson(jsonDocument, newValue);
    if (error) {
        Logger.logf(DEBUG, MSG_FAILED_TO_PARSE, getSubscribeTopic().c_str());
    } else {
        JsonObject rootObject = jsonDocument.as<JsonObject>();
        write(fromJSON(rootObject), publishable);
    }
}

template<typename T>
inline void NodeInterface<T>::handle() {
    //SAMPLING
    if (_samplingEnabled) {
        do {
            _forceResample = false;
            write(sample());
        } while (_forceResample);
    }
    yield();
}

template<typename T>
inline void NodeInterface<T>::forceResample() { this->_forceResample = true; }

template<typename T>
inline void NodeInterface<T>::preventDebugLogging(bool prevent) {
    this->_preventDebugLogging = prevent;
}

template<typename T>
inline String NodeInterface<T>::toString(JsonObject rootObject) {
    String jsonStr;
    serializeJson(rootObject, jsonStr);
    return jsonStr;
}

template<typename T>
inline void NodeInterface<T>::setSamplingRate(unsigned long samplingRate) {
    _samplingRate = samplingRate;
    _task.setInterval(samplingRate);
}

template<typename T>
inline void NodeInterface<T>::setContext(ApplicationContext *context) {
    _context = context;
    this->getScheduler()->addTask(_task);
    if (_enabled) {
        _task.enableIfNot();
    }
}

template<typename T>
inline ApplicationContext *NodeInterface<T>::getContext() {
    if (!_context)
        Logger.logf(FATAL, MSG_CONTEXT_IS_MISSING, _interfaceName.c_str(), this->getPublishTopic().c_str());
    return _context;
}

template<typename T>
inline void NodeInterface<T>::setInterfaceName(String name) {
    _interfaceName = name;
}

template<typename T>
inline void NodeInterface<T>::setEnabled(bool en) {
    if (en == _enabled)
        return;
    _enabled = en;
    (_enabled) ? _task.enableIfNot() : _task.disable();
}

template<typename T>
inline void NodeInterface<T>::enable() { this->setEnabled(true); }

template<typename T>
inline void NodeInterface<T>::disable() { this->setEnabled(false); }

template<typename T>
inline bool NodeInterface<T>::isEnabled() {
    return _enabled;
}

template<typename T>
inline void NodeInterface<T>::setSamplingEnabled(bool en) {
    _samplingEnabled = en;
}

template<typename T>
inline bool NodeInterface<T>::isSamplingEnabled() {
    return _samplingEnabled;
}

template<typename T>
inline bool NodeInterface<T>::hasMQTTPublish() {
    return _hasMQTTPublish;
}

template<typename T>
inline bool NodeInterface<T>::hasMQTTSubscribe() {
    return _hasMQTTSubscribe;
}

template<typename T>
inline bool NodeInterface<T>::hasInitializedValue() {
    return _valueInitialized;
}

template<typename T>
inline void NodeInterface<T>::setMQTTPublish(bool publish) {
    _hasMQTTPublish = publish;
}

template<typename T>
inline void NodeInterface<T>::setMQTTSubscribe(bool subscribe) {
    _hasMQTTSubscribe = subscribe;
}

template<typename T>
inline void NodeInterface<T>::setMQTTPublishSubscribe(bool publish, bool subscribe) {
    _hasMQTTPublish = publish;
    _hasMQTTSubscribe = subscribe;
}

template<typename T>
inline void NodeInterface<T>::setBaseTopic(String baseTopic) {
    _baseTopic = baseTopic;
}

template<typename T>
inline void NodeInterface<T>::triggerCallback() {
    if (_onChangeCallback) {
        _onChangeCallback(currentValue, currentValue);
    }
}

template<typename T>
inline String NodeInterface<T>::getBaseTopic() {
    return _baseTopic;
}

template<typename T>
inline String NodeInterface<T>::getSubscribeTopic() {
    return _baseTopic + "/" + _subscribeTopic;
}

template<typename T>
inline String NodeInterface<T>::getPublishTopic() {
    return _baseTopic + "/" + _publishTopic;
}

template<typename T>
inline Scheduler *NodeInterface<T>::getScheduler() {
    return getContext()->scheduler;
}

#endif //NODEINTERFACE_H