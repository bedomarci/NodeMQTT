#if !defined(WIFITRANSPORT_H) && (NODEMQTT_TRANSPORT == WIFI_TRANSPORT)
#define WIFITRANSPORT_H

#include "_AbstractTransport.hpp"
#include "WiFiClient.h"
#include "PubSubCLient.h"
#include "../misc/helpers.hpp"
#include <NodeMQTTEventHandler.hpp>

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
    bool wasConnectedToServer = false;

    void publish(const char *topic, const char *msg);

    void subscribe(const char *topic);

protected:
    void loadConfiguration();

    void registerConfiguration();

    void reconnectBroker();

    void reconnectWifi();

    static int32_t getRSSI();

    int32_t RSSIToPercentage(int32_t rssi);

    void logWifiInfo();

    bool isIPvalid(uint8_t ip[4]);

    void setOutputPower(float power);

    void setSleepMode();

    CLIENT_CLASS *espClient;
    PubSubClient *client;

    Task _tWifiConnect;
    Task _tBrokerConnect;

    uint8_t wifiConnectionAttampt = 1;
    uint8_t brokerConnectionAttampt = 1;

    String wifiSsid;
    uint8_t wifiBssid[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    String wifiPassword;
    int wifiChannel = DEFAULT_WIFI_CHANNEL;
    uint8_t ipAddress[4] = {0, 0, 0, 0};
    uint8_t gateway[4] = {0, 0, 0, 0};
    uint8_t dns[4] = {0, 0, 0, 0};
    uint8_t subnetMask[4] = {0, 0, 0, 0};
    String mqttServer;
    String mqttUser;
    String mqttPassword;
    int mqttPort = DEFAULT_MQTT_PORT;
    String baseTopic;
};

inline WifiTransport::WifiTransport() : AbstractTransport() {
    registerConfiguration();
    _tWifiConnect.set(WIFI_CONNECT_ATTEMPT_WAITING, TASK_FOREVER, [this]() { reconnectWifi(); });
    _tBrokerConnect.set(MQTT_CONNECT_ATTEMPT_WAITING, TASK_FOREVER, [this]() { reconnectBroker(); });
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

    WiFi.mode(WIFI_STA);
    WiFi.softAPdisconnect(true);
    WiFi.enableAP(false);
    this->setOutputPower(WIFI_TRANSMISSION_POWER);
    WiFi.persistent(false);
    this->setSleepMode();
    // WiFi.setPhyMode(WIFI_PHY_MODE_11B);

    espClient = new CLIENT_CLASS();
    client = new PubSubClient(*espClient);
    client->setCallback([this](char *t, byte *p, unsigned int l) { this->onMessage(t, p, l); });
    client->setServer(this->mqttServer.c_str(), this->mqttPort);
}

inline void WifiTransport::registerConfiguration() {
    parseIpAddress((char *) DEFAULT_IP_ADDRESS, this->ipAddress);
    parseIpAddress((char *) DEFAULT_GATEWAY, this->gateway);
    parseIpAddress((char *) DEFAULT_SUBNET, this->subnetMask);
    parseIpAddress((char *) DEFAULT_DNS, this->dns);
    parseMacAddress((char *) DEFAULT_WIFI_BSSID, this->wifiBssid);

    NodeMQTTConfigManager.registerStringProperty(PROP_WIFI_SSID, (const char *) ATTR_WIFISSID, DEFAULT_WIFI_SSID);
    NodeMQTTConfigManager.registerMACProperty(PROP_WIFI_BSSID, (const char *) ATTR_WIFIBSSID, this->wifiBssid);
    NodeMQTTConfigManager.registerStringProperty(PROP_WIFI_PASSWORD, (const char *) ATTR_WIFIPASS, DEFAULT_WIFI_PASSWORD);
    NodeMQTTConfigManager.registerIntProperty(PROP_WIFI_CHANNEL, (const char *) ATTR_WIFICHANNEL, DEFAULT_WIFI_CHANNEL);
    NodeMQTTConfigManager.registerIPProperty(PROP_WIFI_IPADDRESS, (const char *) ATTR_IPADDRESS, this->ipAddress);
    NodeMQTTConfigManager.registerIPProperty(PROP_WIFI_SUBNET, (const char *) ATTR_SUBNET, this->subnetMask);
    NodeMQTTConfigManager.registerIPProperty(PROP_WIFI_GATEWAY, (const char *) ATTR_GATEWAY, this->gateway);
    NodeMQTTConfigManager.registerIPProperty(PROP_WIFI_DNS, (const char *) ATTR_DNS, this->dns);

    NodeMQTTConfigManager.registerStringProperty(PROP_MQTT_SERVER, (const char *) ATTR_MQTTSERV, DEFAULT_MQTT_SERVER);
    NodeMQTTConfigManager.registerIntProperty(PROP_MQTT_PORT, (const char *) ATTR_MQTTPORT, DEFAULT_MQTT_PORT);
    NodeMQTTConfigManager.registerStringProperty(PROP_MQTT_USER, (const char *) ATTR_MQTTUSER, DEFAULT_MQTT_USER);
    NodeMQTTConfigManager.registerStringProperty(PROP_MQTT_PASSWORD, (const char *) ATTR_MQTTPASS, DEFAULT_MQTT_PASSWORD);

}

inline void WifiTransport::loadConfiguration() {
    this->wifiSsid = NodeMQTTConfigManager.getStringProperty(PROP_WIFI_SSID);
    this->wifiPassword = NodeMQTTConfigManager.getStringProperty(PROP_WIFI_PASSWORD);
    this->wifiChannel = NodeMQTTConfigManager.getIntProperty(PROP_WIFI_CHANNEL);
    NodeMQTTConfigManager.getIPProperty(PROP_WIFI_IPADDRESS, this->ipAddress);
    NodeMQTTConfigManager.getIPProperty(PROP_WIFI_SUBNET, this->subnetMask);
    NodeMQTTConfigManager.getIPProperty(PROP_WIFI_GATEWAY, this->gateway);
    NodeMQTTConfigManager.getIPProperty(PROP_WIFI_DNS, this->dns);
    NodeMQTTConfigManager.getMACProperty(PROP_WIFI_BSSID, this->wifiBssid);
    this->mqttServer = NodeMQTTConfigManager.getStringProperty(PROP_MQTT_SERVER);
    this->mqttPort = NodeMQTTConfigManager.getIntProperty(PROP_MQTT_PORT);
    this->mqttUser = NodeMQTTConfigManager.getStringProperty(PROP_MQTT_USER);
    this->mqttPassword = NodeMQTTConfigManager.getStringProperty(PROP_MQTT_PASSWORD);
    this->baseTopic = NodeMQTTConfigManager.getStringProperty(PROP_SYS_BASETOPIC);
}

inline void WifiTransport::publish(const char *topic, const char *msg) {
    bool published = client->publish(topic, msg);
    if (!published) {
        Logger.logf(L_DEBUG, F("Failed to publish payload on [%s] topic. Length: %d, Buffer: %d"), topic, strlen(msg), MQTT_MAX_PACKET_SIZE);
    }
}

inline void WifiTransport::subscribe(const char *topic) {
    client->subscribe(topic);
}

inline void WifiTransport::reconnectBroker() {
    String sUUID;
    formatUUID(sUUID);
    Logger.logf(L_INFO, MSG_BROKER_CONNECTION_ATTEMPT, sUUID.c_str(), brokerConnectionAttampt);
    event(EVENT_SERVER_CONNECTING);
    bool brokerConnected = false;
    if (this->mqttUser.length())
        brokerConnected = client->connect(sUUID.c_str(), this->mqttUser.c_str(), this->mqttPassword.c_str(), this->baseTopic.c_str(), 1, false, String(OFFLINE_MESSAGE).c_str());
    else
        brokerConnected = client->connect(sUUID.c_str());
    if (brokerConnected) {
        i(F("We are all set. Let's go!"));
        event(EVENT_SERVER_CONNECTED);
        _tBrokerConnect.disable();
        wasConnectedToServer = true;
        brokerConnectionAttampt = 1;
    } else {
        event(EVENT_SERVER_DISCONNECTED);
        Logger.logf(L_ERROR, MSG_BROKER_COULD_NOT_CONNECT, client->state());
    }
    brokerConnectionAttampt++;
}

inline void WifiTransport::reconnectWifi() {
    event(EVENT_NETWORK_CONNECTING);
    WiFi.disconnect();
    bool useStaticIp = isIPvalid(this->ipAddress);
    IPAddress localIp(this->ipAddress[0], this->ipAddress[1], this->ipAddress[2], this->ipAddress[3]);   //ESP static ip
    IPAddress gateway(this->gateway[0], this->gateway[1], this->gateway[2], this->gateway[3]);            //IP Address of your WiFi Router (Gateway)
    IPAddress subnet(this->subnetMask[0], this->subnetMask[1], this->subnetMask[2], this->subnetMask[3]); //Subnet mask
    IPAddress dns(this->dns[0], this->dns[1], this->dns[2], this->dns[3]);

    //    SET STATIC CONFIGURATION IF AVAILABLE
    if (useStaticIp) {
        WiFi.config(localIp, gateway, subnet, dns);
    } else {
        WiFi.config(localIp, localIp, localIp, localIp); //resets for dhcp in case of 0.0.0.0
    }

    if (hasBSSID(this->wifiBssid)) {
        Logger.logf(L_DEBUG, MSG_CONNECT_TO_WIFI_BSSID, this->wifiSsid.c_str(), wifiConnectionAttampt);
        WiFi.begin(this->wifiSsid.c_str(), this->wifiPassword.c_str(), this->wifiChannel, this->wifiBssid);
    } else {
        Logger.logf(L_DEBUG, MSG_CONNECT_TO_WIFI, this->wifiSsid.c_str(), wifiConnectionAttampt);
        WiFi.begin(this->wifiSsid.c_str(), this->wifiPassword.c_str());
    }

    wifiConnectionAttampt++;
}

inline void WifiTransport::loop() {
    if (NodeMQTTConfigManager.getBoolProperty(PROP_SYS_ONLINE)) {
        if (WiFi.status() == WL_CONNECTED) {
            if (_tWifiConnect.isEnabled()) {
                _tWifiConnect.disable();
                event(EVENT_NETWORK_CONNECTED);
                wifiConnectionAttampt = 1;
                delay(500);
                this->logWifiInfo();
            } else {
                if (!client->connected()) {
                    if (wasConnectedToServer) {
                        Logger.logf(L_ERROR, MSG_BROKER_DISCONNECTED, client->state());
                        event(EVENT_SERVER_DISCONNECTED);
                        brokerConnectionAttampt = 1;
                    }
                    wasConnectedToServer = false;
                    _tBrokerConnect.enableIfNot();
                }
            }
            wasConnectedToNetwork = true;
        } else {
            if (wasConnectedToNetwork)
                event(EVENT_NETWORK_DISCONNECTED);
            wasConnectedToNetwork = false;
            _tWifiConnect.enableIfNot();
        }
    }
    noInterrupts();
    client->loop();
    interrupts();
}

inline void WifiTransport::logWifiInfo() {
    Logger.logf(L_INFO, F("WiFi connected. IP address: %s"), this->getNetworkAddressString().c_str());
    if (NodeMQTTConfigManager.getBoolProperty(PROP_SYS_SERVICEMODE)) {
        int32_t signalStrength = WifiTransport::getRSSI();
        Logger.logf(L_DEBUG, F("Signal strength: %d dBm, TX power: %.2f dBm"), signalStrength, WIFI_TRANSMISSION_POWER);
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
    return WiFi.status() == WL_CONNECTED && client->connected();
}


inline String WifiTransport::getNetworkAddressString() {
    if (WiFi.status() == WL_CONNECTED) {
        return WiFi.localIP().toString();
    } else {
        return OFFLINE_MESSAGE;
    }
}

inline bool WifiTransport::isIPvalid(uint8_t ip[4]) {
    int sum = 0;
    for (int i = 0; i < 4; i++) {
        sum += ip[i];
    }
    return sum;
}

inline String WifiTransport::getState() {
    char s[64];
    uint32_t rssi = WifiTransport::getRSSI();
    sprintf(s, PSTR("IP: %s; RX pwr: %d dBm (%d%%); TX pwr: %.2f dBm"), this->getNetworkAddressString().c_str(), rssi,
            RSSIToPercentage(rssi), WIFI_TRANSMISSION_POWER);
    return String(s);
}

#endif //WIFITRANSPORT_H