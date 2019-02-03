#include "NodeMQTTUpdateManager.hpp"
#include "misc/helpers.hpp"

#ifdef ESP8266

NodeMQTTUpdateManagerClass::NodeMQTTUpdateManagerClass()
{
    ArduinoOTA.onStart([=]() { onOTAStart(); });
    ArduinoOTA.onEnd([=]() { onOTAEnd(); });
    ArduinoOTA.onError([=](ota_error_t error) { onOTAError(error); });
    ArduinoOTA.onProgress([=](unsigned int progress, unsigned int total) { onOTAProgress(progress, total); });
}

void NodeMQTTUpdateManagerClass::onOTAStart()
{
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
        i(F("Uploading sketch."));
    else // U_SPIFFS
        i(F("Uploading filesystem."));

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
}
void NodeMQTTUpdateManagerClass::onOTAEnd()
{
    i(F("\nEnd"));
}
void NodeMQTTUpdateManagerClass::onOTAError(ota_error_t error)
{
    if (error == OTA_AUTH_ERROR)
        e(F("Auth Failed"));
    else if (error == OTA_BEGIN_ERROR)
        e(F("Begin Failed"));
    else if (error == OTA_CONNECT_ERROR)
        e(F("Connect Failed"));
    else if (error == OTA_RECEIVE_ERROR)
        e(F("Receive Failed"));
    else if (error == OTA_END_ERROR)
        e(F("End Failed"));
}
void NodeMQTTUpdateManagerClass::onOTAProgress(unsigned int progress, unsigned int total)
{
    uint16_t percentage = (progress / (total / 100));
    Logger.logf(INFO, "Progress: %u%%\r", percentage);
}

void NodeMQTTUpdateManagerClass::begin(NodeMQTTConfig *config)
{
    ArduinoOTA.setPassword(config->mqttPassword);
    ArduinoOTA.setHostname(String(DEVICE_NAME + "-" + String(config->baseTopic)).c_str());
    ArduinoOTA.begin();
}

void NodeMQTTUpdateManagerClass::checkForUpload()
{
    ArduinoOTA.handle();
}

void NodeMQTTUpdateManagerClass::checkForUpdates()
{
    String fwURL = String(fwUrlBase);
    fwURL.concat(String(WiFi.macAddress()));
    String fwVersionURL = fwURL;
    fwVersionURL.concat(".version");

    HTTPClient httpClient;
    httpClient.begin(fwVersionURL);
    int httpCode = httpClient.GET();
    if (httpCode == 200)
    {
        String newFWVersion = httpClient.getString();
        int newVersion = newFWVersion.toInt();

        if (newVersion > FW_VERSION)
        {

            String fwImageURL = fwURL;
            fwImageURL.concat(".bin");
            t_httpUpdate_return ret = ESPhttpUpdate.update(fwImageURL);

            switch (ret)
            {
            case HTTP_UPDATE_FAILED:
                e(F("HTTP_UPDATE_FAILD"));
                e(ESPhttpUpdate.getLastErrorString().c_str());
                break;

            case HTTP_UPDATE_NO_UPDATES:
                w(F("HTTP_UPDATE_NO_UPDATES"));
                break;

            case HTTP_UPDATE_OK:
                i(F("HTTP_UPDATE_OK"));
                break;
            }
        }
        else
        {
            i(F("Already on latest version"));
        }
    }
    else
    {
        e("Firmware version check failed, got HTTP response code " + httpCode);
    }
    httpClient.end();
}
#endif
NodeMQTTUpdateManagerClass NodeMQTTUpdateManager;