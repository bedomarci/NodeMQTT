// #error "ITT EGY KIS ERROR VAN"
#include "_AbstractTransport.hpp"
#include "../misc/typedef.hpp"

void AbstractTransport::setContext(ApplicationContext *context) {
    _context = context;
}
Scheduler *AbstractTransport::getScheduler()
{
    return _context->scheduler;
}

void AbstractTransport::setNetworkConnectingCallback(NodeMQTTCallback cb)
{
    networkConnectingCallback = cb;
}
void AbstractTransport::setNetworkConnectedCallback(NodeMQTTCallback cb)
{
    networkConnectedCallback = cb;
}
void AbstractTransport::setNetworkDisconnectedCallback(NodeMQTTCallback cb)
{
    networkDisconnectedCallback = cb;
}
void AbstractTransport::setBrokerConnectingCallback(NodeMQTTCallback cb)
{
    brokerConnectingCallback = cb;
}
void AbstractTransport::setBrokerConnectedCallback(NodeMQTTCallback cb)
{
    brokerConnectedCallback = cb;
}
void AbstractTransport::setBrokerDisconnectedCallback(NodeMQTTCallback cb)
{
    brokerDisconnectedCallback = cb;
}
void AbstractTransport::setMessageCallback(NodeMQTTMessageCallback cb)
{
    messageCallback = cb;
}
void AbstractTransport::onNetworkConnecting()
{
    if (networkConnectingCallback != nullptr)
        networkConnectingCallback();
}
void AbstractTransport::onNetworkConnected()
{
    if (networkConnectedCallback != nullptr)
        networkConnectedCallback();
}
void AbstractTransport::onNetworkDisconnected()
{
    if (networkDisconnectedCallback != nullptr)
        networkDisconnectedCallback();
}
void AbstractTransport::onBrokerConnecting()
{
    if (brokerConnectingCallback != nullptr)
        brokerConnectingCallback();
}
void AbstractTransport::onBrokerConnected()
{
    if (brokerConnectedCallback != nullptr)
        brokerConnectedCallback();
}
void AbstractTransport::onBrokerDisconnected()
{
    if (brokerDisconnectedCallback != nullptr)
        brokerDisconnectedCallback();
}
void AbstractTransport::onMessage(char *topic, uint8_t *payload, unsigned int length)
{
    if (messageCallback != nullptr)
        messageCallback(topic, payload, length);
}

String AbstractTransport::getState() {
    return "TRANSPORT STATE N/A";
}