#ifndef NODEUPDATEMANAGER_H
#define NODEUPDATEMANAGER_H

#include "misc/config.hpp"
#include "misc/constants.hpp"
#include "misc/helpers.hpp"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266HTTPUpdate.h>


class NodeMQTTUpdateManagerClass {
    private:
        const char* fwUrlBase = FIRMWARE_URL_BASE;
        const int FW_VERSION = FIRMWARE_VERSION;
        HTTPClient httpClient;
    public:
        void checkForUpdates();
};

inline
void NodeMQTTUpdateManagerClass::checkForUpdates() {
    String fwURL = String( fwUrlBase );
    fwURL.concat( String(WiFi.macAddress()) );
    String fwVersionURL = fwURL;
    fwVersionURL.concat( ".version" );
  

    HTTPClient httpClient;
    httpClient.begin( fwVersionURL );
    int httpCode = httpClient.GET();
    if( httpCode == 200 ) {
        String newFWVersion = httpClient.getString();
        int newVersion = newFWVersion.toInt();

        if( newVersion > FW_VERSION ) {

        String fwImageURL = fwURL;
        fwImageURL.concat( ".bin" );
        t_httpUpdate_return ret = ESPhttpUpdate.update( fwImageURL );

        switch(ret) {
            case HTTP_UPDATE_FAILED: 
            e("HTTP_UPDATE_FAILD");
            e(ESPhttpUpdate.getLastErrorString().c_str());
            break;

            case HTTP_UPDATE_NO_UPDATES:
            i("HTTP_UPDATE_NO_UPDATES");
            break;
        }
        }
        else {
        i( "Already on latest version" );
        }
    }
    else {
        e( "Firmware version check failed, got HTTP response code " + httpCode );
    }
    httpClient.end();
}

extern NodeMQTTUpdateManagerClass NodeMQTTUpdateManager;

#endif //NODEUPDATEMANAGER_H