
#define WIFI_TRANSPORT
#if !defined(WIFITRANSPORT_H) && defined(WIFI_TRANSPORT)
#define WIFITRANSPORT_H

#include "_AbstractTransport.hpp"
#include "WiFiClient.h"
#include "PubSubCLient.h"
#include "../misc/helpers.hpp"

#define MQTT_MAX_PACKET_SIZE NODEMQTT_MAX_PACKET_SIZE

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
    bool wasConnectedToNetwork = false;
    bool wasConnectedToServer = false;

    void publish(const char *topic, const char *msg);
    void subscribe(const char *topic);

  protected:
    void reconnectBroker();
    void reconnectWifi();
    int32_t getRSSI();
    void logWifiInfo();
    bool isIPvalid(uint8_t ip[4]);

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

    WiFi.mode(WIFI_STA);
    WiFi.softAPdisconnect(true);
    WiFi.enableAP(false);

    client = PubSubClient(espClient);
    client.setCallback([this](char *t, byte *p, unsigned int l) { onMessage(t, p, l); });
}
inline void WifiTransport::init()
{
    this->getScheduler()->addTask(_tWifiConnect);
    this->getScheduler()->addTask(_tBrokerConnect);
    if (this->getConfiguration() == nullptr)
    {
        e(("No configuration set for Wifi transport layer!"));
        return;
    }
    client.setServer(this->getConfiguration()->mqttServer, this->getConfiguration()->mqttPort);
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
    String sUUID;
    formatUUID(sUUID);
    Logger.logf(INFO, MSG_BROKER_CONNECTION_ATTEMPT, sUUID.c_str(), brokerConnectionAttampt);
    onBrokerConnecting();
    bool brokerConnected = false;
    if (strlen(this->getConfiguration()->mqttUser))
        brokerConnected = client.connect(sUUID.c_str(), (const char *)this->getConfiguration()->mqttUser, (const char *)this->getConfiguration()->mqttPassword, (const char *)this->getConfiguration()->baseTopic, 1, false, String(OFFLINE_MESSAGE).c_str());
    else
        brokerConnected = client.connect(sUUID.c_str());
    if (brokerConnected)
    {
        i(F("We are all set. Let's go!"));
        onBrokerConnected();
        _tBrokerConnect.disable();
        wasConnectedToServer = true;
        brokerConnectionAttampt = 1;
    }
    else
    {
        onBrokerDisconnected();
        Logger.logf(ERROR, MSG_BROKER_COULD_NOT_CONNECT, client.state());
        brokerConnectionAttampt = 1;
    }
    brokerConnectionAttampt++;
}

inline void WifiTransport::reconnectWifi()
{
    onNetworkConnecting();
    WiFi.disconnect();
    bool useStaticIp = isIPvalid(getConfiguration()->ipAddress);
    IPAddress staticIP(getConfiguration()->ipAddress[0], getConfiguration()->ipAddress[1], getConfiguration()->ipAddress[2], getConfiguration()->ipAddress[3]);   //ESP static ip
    IPAddress gateway(getConfiguration()->gateway[0], getConfiguration()->gateway[1], getConfiguration()->gateway[2], getConfiguration()->gateway[3]);            //IP Address of your WiFi Router (Gateway)
    IPAddress subnet(getConfiguration()->subnetMask[0], getConfiguration()->subnetMask[1], getConfiguration()->subnetMask[2], getConfiguration()->subnetMask[3]); //Subnet mask
    IPAddress dns(getConfiguration()->dns[0], getConfiguration()->dns[1], getConfiguration()->dns[2], getConfiguration()->dns[3]);

    //SET STATIC CONFIGURATION IF AVAILABLE
    if (useStaticIp)
        WiFi.config(staticIP, gateway, subnet, dns);
    

    if (hasBSSID(this->getConfiguration()->wifiBssid))
    {
        d("Connecting to " + String(this->getConfiguration()->wifiSsid) + " with BSSID. #" + String(wifiConnectionAttampt));
        WiFi.begin(this->getConfiguration()->wifiSsid, this->getConfiguration()->wifiPassword, this->getConfiguration()->wifiChannel, this->getConfiguration()->wifiBssid);
    }
    else
    {
        d("Connecting to " + String(this->getConfiguration()->wifiSsid) + ". #" + String(wifiConnectionAttampt));
        WiFi.begin(this->getConfiguration()->wifiSsid, this->getConfiguration()->wifiPassword);
    }

    if (useStaticIp)
        WiFi.config(staticIP, gateway, subnet, dns);

    wifiConnectionAttampt++;

}

inline void WifiTransport::loop()
{
    if (this->getConfiguration()->isOnline)
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
                    if (wasConnectedToServer)
                    {
                        Logger.logf(ERROR, MSG_BROKER_DISCONNECTED, client.state());
                        onBrokerDisconnected();
                        brokerConnectionAttampt = 1;
                    }
                    wasConnectedToServer = false;
                    _tBrokerConnect.enableIfNot();
                }
            }
            wasConnectedToNetwork = true;
        }
        else
        {
            if (wasConnectedToNetwork)
                onNetworkDisconnected();
            wasConnectedToNetwork = false;
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
    if (this->getConfiguration()->isServiceMode)
    {
        int32_t signalStrength = this->getRSSI();
        Logger.logf(INFO, F("Signal strength: %d dBm"), signalStrength);
        if (signalStrength <= -80)
            fatal(F("Wifi signal is too weak!"));
    }
}

inline int32_t WifiTransport::getRSSI()
{
    byte available_networks = WiFi.scanNetworks();
    for (int network = 0; network < available_networks; network++)
    {
        if (strcmp(WiFi.SSID(network).c_str(), this->getConfiguration()->wifiSsid) == 0)
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

inline bool WifiTransport::isIPvalid(uint8_t ip[4])
{
    int sum = 0;
    for (int i = 0; i < 4; i++)
    {
        sum += ip[i];
    }
    return sum;
}

#endif //WIFITRANSPORT_H