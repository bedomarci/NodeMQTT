#include "NodeMQTTUpdateManager.hpp"
#include "misc/helpers.hpp"
#include "misc/typedef.hpp"

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

NodeMQTTUpdateManagerClass::NodeMQTTUpdateManagerClass() {
    ArduinoOTA.onStart([=]() { onOTAStart(); });
    ArduinoOTA.onEnd([=]() { onOTAEnd(); });
    ArduinoOTA.onError([=](ota_error_t error) { onOTAError(error); });
    ArduinoOTA.onProgress([=](unsigned int progress, unsigned int total) { onOTAProgress(progress, total); });
}

void NodeMQTTUpdateManagerClass::init() {
    this->fwUrlBase = NodeMQTTConfigManager.getStringProperty(PROP_SYS_FWURL);
    ArduinoOTA.setPassword(NodeMQTTConfigManager.getStringProperty(PROP_MQTT_PASSWORD).c_str());
    ArduinoOTA.setHostname(String(String(UUID) + "-" + NodeMQTTConfigManager.getStringProperty(PROP_SYS_BASETOPIC)).c_str());
    ArduinoOTA.begin();
}

void NodeMQTTUpdateManagerClass::boot() {
    NodeMQTTConfigManager.registerStringProperty(PROP_SYS_FWURL, (const char *) ATTR_FWURL, DEFAULT_FIRMWARE_URL);
}

void NodeMQTTUpdateManagerClass::onOTAStart() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
        i(F("Uploading sketch."));
    else // U_SPIFFS
        i(F("Uploading filesystem."));

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
}

void NodeMQTTUpdateManagerClass::onOTAEnd() {
    i(F("\nEnd"));
}

void NodeMQTTUpdateManagerClass::onOTAError(ota_error_t error) {
    if (error == OTA_AUTH_ERROR)
        e(F("OTA update error: auth failed"));
    else if (error == OTA_BEGIN_ERROR)
        e(F("OTA update error: begin failed"));
    else if (error == OTA_CONNECT_ERROR)
        e(F("OTA update error: connect failed"));
    else if (error == OTA_RECEIVE_ERROR)
        e(F("OTA update error: receive failed"));
    else if (error == OTA_END_ERROR)
        e(F("OTA update error: end failed"));
}

void NodeMQTTUpdateManagerClass::onOTAProgress(unsigned int progress, unsigned int total) {
    uint16_t percentage = (progress / (total / 100));
    Logger.logf(INFO, "Progress: %u%%\r", percentage);
}

void NodeMQTTUpdateManagerClass::checkForUpload() {
    ArduinoOTA.handle();
}

void NodeMQTTUpdateManagerClass::checkForUpdates() {
    String fwURL = String(this->fwUrlBase);
    fwURL.concat(NodeMQTTConfigManager.getStringProperty(PROP_SYS_BASETOPIC));
    String fwVersionURL = fwURL;
    fwVersionURL.concat(F(".ver"));
    Logger.logf(INFO, F("Checking for new update at %s."), fwVersionURL.c_str());

    HTTPClient httpClient;
    httpClient.begin(client, fwVersionURL);
    int httpCode = httpClient.GET();
    if (httpCode == 200) {
        String newFWVersion = httpClient.getString();
        int newVersion = newFWVersion.toInt();

        if (newVersion > FIRMWARE_BUILD_TIME) {
            String fwImageURL = fwURL;
            Logger.logf(INFO, F("Newer version available: %s."), newFWVersion.c_str());
            fwImageURL.concat(F(".bin"));
            Logger.logf(INFO, F("Downloading firmware from %s."), fwImageURL.c_str());

            yield();
#if defined(ARDUINO_ARCH_ESP8266)
            WiFiClient updateClient;
            t_httpUpdate_return ret = UPDATE.update(updateClient, fwImageURL);
#elif defined(ARDUINO_ARCH_ESP32)
            WiFiClient updateClient;
            t_httpUpdate_return ret = UPDATE.update(updateClient, fwImageURL);
#ifdef LED_BUILTIN
            UPDATE.setLedPin(LED_BUILTIN, LOW);
#endif
#endif

            switch (ret) {
                case HTTP_UPDATE_FAILED:
                    e(F("HTTP_UPDATE_FAILD"));
                    e(UPDATE.getLastErrorString().c_str());
                    break;

                case HTTP_UPDATE_NO_UPDATES:
                    w(F("HTTP_UPDATE_NO_UPDATES"));
                    break;

                case HTTP_UPDATE_OK:
                    i(F("HTTP_UPDATE_OK"));
                    break;
            }
        } else {
            i(F("Already on latest version."));
        }
    } else {
        Logger.logf(ERROR, F("Firmware version check failed, got HTTP response code %d."), httpCode);
    }
    httpClient.end();
}


#endif
NodeMQTTUpdateManagerClass NodeMQTTUpdateManager;