// ************************************* //
// * Arduino Project RFLink-esp        * //
// * https://github.com/couin3/RFLink  * //
// * WiFiManager & FOTA Functionality  * //
// * Proposed by Thierry - IDkonnecT   * //
// ************************************* //

#ifndef _WiFiManager_FOTA_h
#define _WiFiManager_FOTA_h

#include <Arduino.h>
#include "RFLink.h"


#if defined(WIFIMANAGER_ENABLED) || defined(FOTA_ENABLED)
    #include <Preferences.h>
    void Debug(String Texte, boolean NewLine);
#endif 


#ifdef WIFIMANAGER_ENABLED
    #include <WiFiManager.h>

    extern const char *DeviceName;
    extern const char *WiFiManager_SSID;
    extern IPAddress WiFiManager_IP;
    extern IPAddress WiFiManager_Gateway;
    extern IPAddress WiFiManager_SubNet;
    extern String MQTT_SERVER;
    extern String MQTT_PORT;
    extern String MQTT_ID;
    extern String MQTT_USER;
    extern String MQTT_PSWD;
    extern String MQTT_TOPIC_OUT;
    extern String MQTT_TOPIC_IN;

    void WiFiManagerWiFiFail(WiFiManager *myWiFiManager);
    void WiFiManagerConfigOK();
    void WiFiManagerParamOK();
    void SetUpWiFiManager();

#endif // WIFIMANAGER_ENABLED


#ifdef FOTA_ENABLED

    #include <HTTPClient.h>
    #include <HTTPUpdate.h>

    extern String FOTA_FILE;

    void Fota();

#endif // FOTA_ENABLED


#endif // _WiFiManager_FOTA_h