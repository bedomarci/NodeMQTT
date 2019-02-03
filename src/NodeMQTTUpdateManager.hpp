#ifndef NODEUPDATEMANAGER_H
#define NODEUPDATEMANAGER_H

#include <Arduino.h>
#include "NodeMQTTConfigManager.hpp"
#include <ArduinoOTA.h>
#include "misc/config.hpp"
#include "constants.hpp"
#include "misc/helpers.hpp"

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266HTTPUpdate.h>
#endif

class NodeMQTTUpdateManagerClass
{
#ifdef ESP8266
protected:
  const char *fwUrlBase = FIRMWARE_URL_BASE;
  const int FW_VERSION = FIRMWARE_VERSION;
  HTTPClient httpClient;
  void onOTAStart();
  void onOTAEnd();
  void onOTAError(ota_error_t error);
  void onOTAProgress(unsigned int progress, unsigned int total);

public:
  NodeMQTTUpdateManagerClass();
  void begin(NodeMQTTConfig *config);
  void checkForUpdates();
  void checkForUpload();
#endif
};

extern NodeMQTTUpdateManagerClass NodeMQTTUpdateManager;

#endif //NODEUPDATEMANAGER_H