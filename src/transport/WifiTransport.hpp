
#if !defined(WIFITRANSPORT_H) && defined(WIFI_TRANSPORT)
#define WIFITRANSPORT_H
#include "_AbstractTransport.hpp"
#include "WiFiClient.h"
#include "PubSubCLient.h"
#include "../misc/helpers.hpp"

#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif
#ifdef ESP32
#include <WiFi.h>
#endif

class WifiTransport : public AbstractTransport
{
  public:
    WifiTransport();
    void init();
    void loop();
    void connectNetwork();
    bool isNetworkConnected();
    String getNetworkAddressString();
    bool wasOnline = false;

    void publish(const char *topic, const char *msg);
    void subscribe(const char *topic);

  protected:
    void reconnectBroker();
    void reconnectWifi();
    int32_t getRSSI();
    void logWifiInfo();

    WiFiClient espClient;
    PubSubClient client;

    Task _tWifiConnect;
    Task _tBrokerConnect;

    uint8_t wifiConnectionAttampt = 1;
    uint8_t brokerConnectionAttampt = 1;
};
inline WifiTransport::WifiTransport() : AbstractTransport()
{
    _tWifiConnect.set(WIFI_CONNECT_ATTEMPT_WAITING, TASK_FOREVER, [this]() { reconnectWifi(); });
    _tBrokerConnect.set(MQTT_CONNECT_ATTEMPT_WAITING, TASK_FOREVER, [this]() { reconnectBroker(); });

    client = PubSubClient(espClient);
    client.setCallback([this](char *t, byte *p, unsigned int l) { onMessage(t, p, l); });
}
inline void WifiTransport::init()
{
    _scheduler->addTask(_tWifiConnect);
    _scheduler->addTask(_tBrokerConnect);
    if (_config == nullptr)
    {
        e(("No configuration set for Wifi transport layer!"));
        return;
    }
    client.setServer(_config->mqttServer, _config->mqttPort);
}
inline void WifiTransport::publish(const char *topic, const char *msg)
{
    client.publish(topic, msg);
}
inline void WifiTransport::subscribe(const char *topic)
{
    client.subscribe(topic);
}

inline void WifiTransport::reconnectBroker()
{
    d(DEVICE_NAME + " is attempting to connect to broker. #" + brokerConnectionAttampt);
    onBrokerConnecting();
    if (
        client.connect(DEVICE_NAME.c_str(),
                       (const char *)_config->mqttUser,
                       (const char *)_config->mqttPassword,
                       (const char *)_config->baseTopic,
                       1, false, String(OFFLINE_MESSAGE).c_str()))
    {
        onBrokerConnected();
        _tBrokerConnect.disable();
        brokerConnectionAttampt = 1;
    }
    else
    {
        onBrokerDisconnected();
        d("Failed to connect broker, rc=" + client.state());
    }
    brokerConnectionAttampt++;
}

inline void WifiTransport::reconnectWifi()
{
    onNetworkConnecting();
    int hasBSSID = 0;
    for (int i = 0; i < 6; i++)
    {
        hasBSSID += _config->wifiBssid[i];
    }
    if (hasBSSID)
    {
        d("Connecting to " + String(_config->wifiSsid) + " with BSSID. #" + String(wifiConnectionAttampt));
        WiFi.begin(_config->wifiSsid, _config->wifiPassword, _config->wifiChannel, _config->wifiBssid);
    }
    else
    {
        d("Connecting to " + String(_config->wifiSsid) + ". #" + String(wifiConnectionAttampt));
        WiFi.begin(_config->wifiSsid, _config->wifiPassword);
    }
    wifiConnectionAttampt++;
}

inline void WifiTransport::loop()
{
    if (_config->isOnline)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            if (_tWifiConnect.isEnabled())
            {
                _tWifiConnect.disable();
                onNetworkConnected();
                wifiConnectionAttampt = 1;
                delay(500);
                this->logWifiInfo();
            }
            else
            {
                if (!client.connected())
                {
                    _tBrokerConnect.enableIfNot();
                }
            }
            wasOnline = true;
        }
        else
        {
            if (wasOnline)
                onNetworkDisconnected();
            wasOnline = false;
            _tWifiConnect.enableIfNot();
        }
    }
    noInterrupts();
    client.loop();
    interrupts();
}

inline void WifiTransport::logWifiInfo()
{
    Logger.logf(INFO, ("WiFi connected. IP address: %s"), this->getNetworkAddressString().c_str());
    if (_config->isServiceMode)
    {
        int32_t signalStrength = this->getRSSI();
        Logger.logf(INFO, ("Signal strength: %d dBm"), signalStrength);
        if (signalStrength <= -80)
            fatal("Wifi signal is too weak!");
    }
}

inline int32_t WifiTransport::getRSSI()
{
    byte available_networks = WiFi.scanNetworks();
    for (int network = 0; network < available_networks; network++)
    {
        if (strcmp(WiFi.SSID(network).c_str(), _config->wifiSsid) == 0)
        {
            return WiFi.RSSI(network);
        }
    }
    return 0;
}

inline void WifiTransport::connectNetwork()
{
    _tWifiConnect.enable();
}

inline bool WifiTransport::isNetworkConnected()
{
    return WiFi.status() == WL_CONNECTED && client.connected();
}

inline String WifiTransport::getNetworkAddressString()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return WiFi.localIP().toString();
    }
    else
    {
        return OFFLINE_MESSAGE;
    }
}

#endif //WIFITRANSPORT_H