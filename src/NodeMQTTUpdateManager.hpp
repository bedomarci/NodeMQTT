#ifndef NODEUPDATEMANAGER_H
#define NODEUPDATEMANAGER_H

#include <Arduino.h>
#include "NodeMQTTConfigManager.hpp"
#include "misc/config.hpp"
#include "constants.hpp"
#include "misc/helpers.hpp"
#include "misc/NodeMQTTComponent.hpp"


#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  #include <ArduinoOTA.h>
#endif

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266HTTPUpdate.h>
#endif

#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#endif

class NodeMQTTUpdateManagerClass : public NodeMQTTComponent {
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
protected:
    String fwUrlBase = DEFAULT_FIRMWARE_URL;
  HTTPClient *httpClient;
  CLIENT_CLASS *client;
  void onOTAStart();
  void onOTAEnd();
  void onOTAError(ota_error_t error);
  void onOTAProgress(unsigned int progress, unsigned int total);

public:
  NodeMQTTUpdateManagerClass();

    void init() override;

    void boot() override;
  void checkForUpdates();
  void checkForUpload();
#endif
};

extern NodeMQTTUpdateManagerClass NodeMQTTUpdateManager;

#endif //NODEUPDATEMANAGER_H