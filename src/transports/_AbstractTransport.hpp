#ifndef ABSTRACTTRANSPORT_H
#define ABSTRACTTRANSPORT_H
#include "../NodeMQTTConfigManager.hpp"
#include "../misc/helpers.hpp"
#include "../misc/typedef.hpp"
#include "TaskScheduler.h"
#include "../NodeMQTT.hpp"

#include "Client.h"

class AbstractTransport
{
  public:
    virtual void init() = 0;
    virtual void loop() = 0;
    virtual void connectNetwork() = 0;
    virtual bool isNetworkConnected() = 0;
    virtual String getNetworkAddressString() = 0;

    virtual void publish(const char *topic, const char *msg) = 0;
    virtual void subscribe(const char *topic) = 0;

    void setContext(ApplicationContext *context);

    void setMessageCallback(NodeMQTTMessageCallback);

    void setNetworkConnectingCallback(NodeMQTTCallback);
    void setNetworkConnectedCallback(NodeMQTTCallback);
    void setNetworkDisconnectedCallback(NodeMQTTCallback);

    void setBrokerConnectingCallback(NodeMQTTCallback);
    void setBrokerConnectedCallback(NodeMQTTCallback);
    void setBrokerDisconnectedCallback(NodeMQTTCallback);

    void onMessage(char *, uint8_t *, unsigned int);
    void onNetworkConnecting();
    void onNetworkConnected();
    void onNetworkDisconnected();
    void onBrokerConnecting();
    void onBrokerConnected();
    void onBrokerDisconnected();

    Scheduler *getScheduler();
    NodeMQTTConfig *getConfiguration();

  protected:
    // Scheduler *_scheduler;
    // NodeMQTTConfig *_config;

  private:
    ApplicationContext *_context;
    NodeMQTTMessageCallback messageCallback;
    NodeMQTTCallback networkConnectingCallback;
    NodeMQTTCallback networkConnectedCallback;
    NodeMQTTCallback networkDisconnectedCallback;
    NodeMQTTCallback brokerConnectingCallback;
    NodeMQTTCallback brokerConnectedCallback;
    NodeMQTTCallback brokerDisconnectedCallback;
};

inline void AbstractTransport::setContext(ApplicationContext *context) {
    _context = context;
}
inline Scheduler *AbstractTransport::getScheduler()
{
    return _context->scheduler;
}
inline NodeMQTTConfig *AbstractTransport::getConfiguration()
{
    return _context->configuration;
}
inline void AbstractTransport::setNetworkConnectingCallback(NodeMQTTCallback cb)
{
    networkConnectingCallback = cb;
}
inline void AbstractTransport::setNetworkConnectedCallback(NodeMQTTCallback cb)
{
    networkConnectedCallback = cb;
}
inline void AbstractTransport::setNetworkDisconnectedCallback(NodeMQTTCallback cb)
{
    networkDisconnectedCallback = cb;
}
inline void AbstractTransport::setBrokerConnectingCallback(NodeMQTTCallback cb)
{
    brokerConnectingCallback = cb;
}
inline void AbstractTransport::setBrokerConnectedCallback(NodeMQTTCallback cb)
{
    brokerConnectedCallback = cb;
}
inline void AbstractTransport::setBrokerDisconnectedCallback(NodeMQTTCallback cb)
{
    brokerDisconnectedCallback = cb;
}
inline void AbstractTransport::setMessageCallback(NodeMQTTMessageCallback cb)
{
    messageCallback = cb;
}
inline void AbstractTransport::onNetworkConnecting()
{
    if (networkConnectingCallback != nullptr)
        networkConnectingCallback();
}
inline void AbstractTransport::onNetworkConnected()
{
    if (networkConnectedCallback != nullptr)
        networkConnectedCallback();
}
inline void AbstractTransport::onNetworkDisconnected()
{
    if (networkDisconnectedCallback != nullptr)
        networkDisconnectedCallback();
}
inline void AbstractTransport::onBrokerConnecting()
{
    if (brokerConnectingCallback != nullptr)
        brokerConnectingCallback();
}
inline void AbstractTransport::onBrokerConnected()
{
    if (brokerConnectedCallback != nullptr)
        brokerConnectedCallback();
}
inline void AbstractTransport::onBrokerDisconnected()
{
    if (brokerDisconnectedCallback != nullptr)
        brokerDisconnectedCallback();
}
inline void AbstractTransport::onMessage(char *topic, uint8_t *payload, unsigned int length)
{
    if (messageCallback != nullptr)
        messageCallback(topic, payload, length);
}

#endif //ABSTRACTTRANSPORT_H