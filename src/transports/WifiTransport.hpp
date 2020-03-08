#if !defined(WIFITRANSPORT_H) && (NODEMQTT_TRANSPORT == WIFI_TRANSPORT)
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

class WifiTransport : public AbstractTransport {
public:
    WifiTransport();
    void init();
    void loop();
    void connectNetwork();
    bool isNetworkConnected();
    String getNetworkAddressString();
    String getState();
    bool wasConnectedToNetwork = false;
    bool wasConnectedToServer  = false;

    void publish(const char *topic, const char *msg);
    void subscribe(const char *topic);

protected:
    void loadConfiguration();
    void reconnectBroker();
    void reconnectWifi();
    static int32_t getRSSI();
    int32_t RSSIToPercentage(int32_t rssi);
    void logWifiInfo();
    bool isIPvalid(uint8_t ip[4]);
    void setOutputPower(float power);
    void setSleepMode();

    WiFiClient   espClient;
    PubSubClient client;

    Task _tWifiConnect;
    Task _tBrokerConnect;

    uint8_t wifiConnectionAttampt   = 1;
    uint8_t brokerConnectionAttampt = 1;

    String wifiSsid;
    uint8_t wifiBssid[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    String wifiPassword;
    int wifiChannel;
    uint8_t ipAddress[4] = {1, 2, 3, 4};
};

inline WifiTransport::WifiTransport() : AbstractTransport() {


    _tWifiConnect.set(WIFI_CONNECT_ATTEMPT_WAITING, TASK_FOREVER, [this]() { reconnectWifi(); });
    _tBrokerConnect.set(MQTT_CONNECT_ATTEMPT_WAITING, TASK_FOREVER, [this]() { reconnectBroker(); });

    WiFi.mode(WIFI_STA);
    WiFi.softAPdisconnect(true);
    WiFi.enableAP(false);
    this->setOutputPower(WIFI_TRANSMISSION_POWER);
    WiFi.persistent(false);
    this->setSleepMode();

    // WiFi.setPhyMode(WIFI_PHY_MODE_11B);

    client = PubSubClient(espClient);
    client.setCallback([this](char *t, byte *p, unsigned int l) { this->onMessage(t, p, l); });
}

inline void WifiTransport::setOutputPower(float power) {
#if (ESP8266)
    WiFi.setOutputPower(power);
#endif
#if (ESP32)
    esp_wifi_set_max_tx_power(power);
#endif
}

inline void WifiTransport::setSleepMode() {
#if (ESP8266)
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
#endif
#if (ESP32)
    //TODO find wifi not sleep equivalent
#endif
}

inline void WifiTransport::init() {
    loadConfiguration();
    this->getScheduler()->addTask(_tWifiConnect);
    this->getScheduler()->addTask(_tBrokerConnect);
    if (this->getConfiguration() == nullptr) {
        e(("No configuration set for Wifi transport layer!"));
        return;
    }
    client.setServer(this->getConfiguration()->mqttServer, this->getConfiguration()->mqttPort);
}

inline void WifiTransport::loadConfiguration() {

    NodeMQTTConfigManager.registerStringProperty(PROP_WIFI_SSID, (const char *) ATTR_WIFISSID, DEFAULT_WIFI_SSID);
    NodeMQTTConfigManager.registerMACProperty(PROP_WIFI_BSSID, (const char *) ATTR_WIFIBSSID, this->wifiBssid);
    NodeMQTTConfigManager.registerStringProperty(PROP_WIFI_PASSWORD, (const char *) ATTR_WIFIPASS,
                                                 DEFAULT_WIFI_PASSWORD);
    NodeMQTTConfigManager.registerIntProperty(PROP_WIFI_CHANNEL, (const char *) ATTR_WIFICHANNEL, DEFAULT_WIFI_CHANNEL);
    NodeMQTTConfigManager.registerIPProperty(PROP_WIFI_IPADDRESS, (const char *) ATTR_IPADDRESS, this->ipAddress);
    this->wifiSsid = NodeMQTTConfigManager.getStringProperty(PROP_WIFI_SSID);
    this->wifiPassword = NodeMQTTConfigManager.getStringProperty(PROP_WIFI_PASSWORD);
    this->wifiChannel = NodeMQTTConfigManager.getIntProperty(PROP_WIFI_CHANNEL);
    NodeMQTTConfigManager.getIPProperty(PROP_WIFI_IPADDRESS, this->ipAddress);
    NodeMQTTConfigManager.getMACProperty(PROP_WIFI_BSSID, this->wifiBssid);

    //TODO nodemqtt begin utan kell hivni a gettert, kulonben nem lesz elerheto az adat

}

inline void WifiTransport::publish(const char *topic, const char *msg) {
    client.publish(topic, msg);
}

inline void WifiTransport::subscribe(const char *topic) {
    client.subscribe(topic);
}

inline void WifiTransport::reconnectBroker() {
    String sUUID;
    formatUUID(sUUID);
    Logger.logf(INFO, MSG_BROKER_CONNECTION_ATTEMPT, sUUID.c_str(), brokerConnectionAttampt);
    this->onBrokerConnecting();
    bool brokerConnected = false;
    if (strlen(this->getConfiguration()->mqttUser))
        brokerConnected = client.connect(sUUID.c_str(), (const char *) this->getConfiguration()->mqttUser,
                                         (const char *) this->getConfiguration()->mqttPassword,
                                         (const char *) this->getConfiguration()->baseTopic, 1, false,
                                         String(OFFLINE_MESSAGE).c_str());
    else
        brokerConnected = client.connect(sUUID.c_str());
    if (brokerConnected) {
        i(F("We are all set. Let's go!"));
        this->onBrokerConnected();
        _tBrokerConnect.disable();
        wasConnectedToServer    = true;
        brokerConnectionAttampt = 1;
    } else {
        this->onBrokerDisconnected();
        Logger.logf(ERROR, MSG_BROKER_COULD_NOT_CONNECT, client.state());
        brokerConnectionAttampt = 1;
    }
    brokerConnectionAttampt++;
}

inline void WifiTransport::reconnectWifi() {
    this->onNetworkConnecting();
    WiFi.disconnect();
    NodeMQTTConfig *c = this->getConfiguration();
    bool      useStaticIp = isIPvalid(c->ipAddress);
    IPAddress localIp(c->ipAddress[0], c->ipAddress[1], c->ipAddress[2], c->ipAddress[3]);   //ESP static ip
    IPAddress gateway(c->gateway[0], c->gateway[1], c->gateway[2],
                      c->gateway[3]);            //IP Address of your WiFi Router (Gateway)
    IPAddress subnet(c->subnetMask[0], c->subnetMask[1], c->subnetMask[2], c->subnetMask[3]); //Subnet mask
    IPAddress dns(c->dns[0], c->dns[1], c->dns[2], c->dns[3]);
    
    //    SET STATIC CONFIGURATION IF AVAILABLE
    if (useStaticIp)
        WiFi.config(localIp, gateway, subnet, dns);
    else {
        WiFi.config(localIp, localIp, localIp, localIp); //resets for dhcp in case of 0.0.0.0
    }

    if (hasBSSID(c->wifiBssid)) {
        Logger.logf(DEBUG, MSG_CONNECT_TO_WIFI_BSSID, c->wifiSsid, wifiConnectionAttampt);
        WiFi.begin(c->wifiSsid, c->wifiPassword, c->wifiChannel, c->wifiBssid);
    } else {
        Logger.logf(DEBUG, MSG_CONNECT_TO_WIFI, c->wifiSsid, wifiConnectionAttampt);
        WiFi.begin(c->wifiSsid, c->wifiPassword);
    }

//    if (useStaticIp)
//        WiFi.config(staticIP, gateway, subnet, dns);

    wifiConnectionAttampt++;

}

inline void WifiTransport::loop() {
    if (this->getConfiguration()->isOnline) {
        if (WiFi.status() == WL_CONNECTED) {
            if (_tWifiConnect.isEnabled()) {
                _tWifiConnect.disable();
                this->onNetworkConnected();
                wifiConnectionAttampt = 1;
                delay(500);
                this->logWifiInfo();
            } else {
                if (!client.connected()) {
                    if (wasConnectedToServer) {
                        Logger.logf(ERROR, MSG_BROKER_DISCONNECTED, client.state());
                        this->onBrokerDisconnected();
                        brokerConnectionAttampt = 1;
                    }
                    wasConnectedToServer = false;
                    _tBrokerConnect.enableIfNot();
                }
            }
            wasConnectedToNetwork = true;
        } else {
            if (wasConnectedToNetwork)
                this->onNetworkDisconnected();
            wasConnectedToNetwork = false;
            _tWifiConnect.enableIfNot();
        }
    }
    noInterrupts();
    client.loop();
    interrupts();
}

inline void WifiTransport::logWifiInfo() {
    Logger.logf(INFO, F("WiFi connected. IP address: %s"), this->getNetworkAddressString().c_str());
    if (this->getConfiguration()->isServiceMode) {
        int32_t signalStrength = WifiTransport::getRSSI();
        Logger.logf(DEBUG, F("Signal strength: %d dBm, TX power: %.2f dBm"), signalStrength, WIFI_TRANSMISSION_POWER);
        if (signalStrength <= -80)
            fatal(F("Wifi signal is too weak!"));
    }
}

inline int32_t WifiTransport::getRSSI() {
    return WiFi.RSSI();
}

inline int32_t WifiTransport::RSSIToPercentage(int32_t rssi) {
    return 2 * (rssi + 100);
}

inline void WifiTransport::connectNetwork() {
    _tWifiConnect.enable();
}

inline bool WifiTransport::isNetworkConnected() {
    return WiFi.status() == WL_CONNECTED && client.connected();
}


inline String WifiTransport::getNetworkAddressString() {
    if (WiFi.status() == WL_CONNECTED) {
        return WiFi.localIP().toString();
    } else {
        return OFFLINE_MESSAGE;
    }
}

inline bool WifiTransport::isIPvalid(uint8_t ip[4]) {
    int      sum = 0;
    for (int i   = 0; i < 4; i++) {
        sum += ip[i];
    }
    return sum;
}

inline String WifiTransport::getState() {
    char     s[64];
    uint32_t rssi = WifiTransport::getRSSI();
    sprintf(s, PSTR("IP: %s; RX pwr: %d dBm (%d%%); TX pwr: %.2f dBm"), this->getNetworkAddressString().c_str(), rssi,
            RSSIToPercentage(rssi), WIFI_TRANSMISSION_POWER);
    return String(s);
}

#endif //WIFITRANSPORT_H