// ************************************* //
// * Arduino Project RFLink-esp        * //
// * https://github.com/couin3/RFLink  * //
// * WiFiManager & FOTA Functionality  * //
// * Proposed by Thierry - IDkonnecT   * //
// ************************************* //

#include <Arduino.h>
#include "RFLink.h"
#ifdef WIFIMANAGER_ENABLED

#include "10_WiFiManager_FOTA.h"

Preferences preferences;
WiFiManager wifiManager;
boolean SaveParameters=false;

void Debug(String Texte="", boolean NewLine=true)
{
 if (NewLine) Serial.println(Texte);
 else Serial.print(Texte);
}

void ReadPreferences()
{
  preferences.begin(DeviceName, true);
  FOTA_FILE = preferences.getString("FOTA_FILE", FOTA_FILE);
  MQTT_SERVER = preferences.getString("MQTT_SERVER", MQTT_SERVER);
  MQTT_PORT = preferences.getString("MQTT_PORT", MQTT_PORT);
  MQTT_ID = preferences.getString("MQTT_ID", MQTT_ID);
  MQTT_USER = preferences.getString("MQTT_USER", MQTT_USER);
  MQTT_PSWD = preferences.getString("MQTT_PSWD", MQTT_PSWD);
  MQTT_TOPIC_OUT = preferences.getString("MQTT_TOPIC_OUT", MQTT_TOPIC_OUT);
  MQTT_TOPIC_IN = preferences.getString("MQTT_TOPIC_IN", MQTT_TOPIC_IN);
  preferences.end();
}

void WiFiManagerWiFiFail(WiFiManager *myWiFiManager)
{
  Debug();
  Debug("********************************");
  Debug("Usual WiFi network not available");
  Debug("Reconfigure by connecting to WiFi : '", false);
  Debug(myWiFiManager->getConfigPortalSSID()+"' on page 'http://"+WiFi.softAPIP().toString()+"'");
  Debug("********************************");
  Debug();
}

void WiFiManagerConfigOK()
{
  Debug("Wifi Configuration saved !");
}

void WiFiManagerParamOK()
{
  SaveParameters = true;
}

void SetUpWiFiManager()
{
  ReadPreferences();
  Debug();
	Debug("WiFi connexion ongoing...");
  //wifiManager.resetSettings();
  wifiManager.setCountry("US"); // US, JP, CN
  wifiManager.setWiFiAPChannel(7);
  //wifiManager.setCaptivePortalEnable(false); // Disable captive portal

  wifiManager.setScanDispPerc(false);          // Affiche le RSSI en % ald BarreGraph
  //wifiManager.setClass("invert");             // Dark theme
  //wifiManager.setCustomHeadElement("<style>html{filter: invert(100%); -webkit-filter: invert(100%);}</style>"); // Custom HTML, CSS or Javascript 
  
  WiFiManagerParameter custom_text("<p></p>");
  wifiManager.addParameter(&custom_text);
   WiFiManagerParameter fota_file("FOTA", "Lien http pour Mise à Jour Firware (.bin)", FOTA_FILE.c_str(), 80); // Variable, description, défaut, longueur, style
    wifiManager.addParameter(&fota_file);
  WiFiManagerParameter mqtt_server("Server", "Serveur MQTT", MQTT_SERVER.c_str(), 40); // Variable, description, défaut, longueur, style
    wifiManager.addParameter(&mqtt_server);
  WiFiManagerParameter mqtt_port("Port", "Port MQTT", MQTT_PORT.c_str(), 5); // Variable, description, défaut, longueur, style
    wifiManager.addParameter(&mqtt_port);
  WiFiManagerParameter mqtt_id("ID", "ID MQTT", MQTT_ID.c_str(), 32); // Variable, description, défaut, longueur, style
    wifiManager.addParameter(&mqtt_id);
  WiFiManagerParameter mqtt_user("User", "User MQTT", MQTT_USER.c_str(), 32); // Variable, description, défaut, longueur, style
    wifiManager.addParameter(&mqtt_user);
  WiFiManagerParameter mqtt_pswd("Pswd", "PassWord MQTT", MQTT_PSWD.c_str(), 32); // Variable, description, défaut, longueur, style
    wifiManager.addParameter(&mqtt_pswd);
  WiFiManagerParameter mqtt_topic_out("Topic_Out", "Topic_Out MQTT", MQTT_TOPIC_OUT.c_str(), 64); // Variable, description, défaut, longueur, style
    wifiManager.addParameter(&mqtt_topic_out);
  WiFiManagerParameter mqtt_topic_in("Topic_In", "Topic_In MQTT", MQTT_TOPIC_IN.c_str(), 64); // Variable, description, défaut, longueur, style
    wifiManager.addParameter(&mqtt_topic_in);
  
  wifiManager.setMinimumSignalQuality(5);
  wifiManager.setAPStaticIPConfig(WiFiManager_IP, WiFiManager_Gateway, WiFiManager_SubNet); // IP, Gateway, SubNet
  wifiManager.setAPCallback(WiFiManagerWiFiFail);
  wifiManager.setSaveConfigCallback(WiFiManagerConfigOK);
  wifiManager.setSaveParamsCallback(WiFiManagerParamOK);
  wifiManager.setConfigPortalTimeout(5*60);
  // wifiManager.startConfigPortal(DeviceName) // Lancement manuel, ne pas utiliser avec wifiManager.autoConnect
  //std::vector<const char *> menu = {"wifi","wifinoscan","info","param","close","sep","erase","restart","exit"};
  std::vector<const char *> menu = {"wifi","param","sep"};
  wifiManager.setMenu(menu);
  
  if(!wifiManager.autoConnect(WiFiManager_SSID)) { Debug("Erreur WiFi !"); }
  
  // Save Parameters
  if (SaveParameters)
    {
    // Préférences to Flash
    //https://github.com/espressif/arduino-esp32/blob/master/libraries/Preferences/src/Preferences.h
    //preferences.clear();
    //preferences.remove("VariableName");
    //Serial.println (preferences.freeEntries());
    preferences.begin(DeviceName, false); // ("Namespace", Read-Only)
    preferences.putString("FOTA_FILE", fota_file.getValue());
    preferences.putString("MQTT_SERVER", mqtt_server.getValue());
    preferences.putString("MQTT_PORT", mqtt_port.getValue());
    preferences.putString("MQTT_ID", mqtt_id.getValue());
    preferences.putString("MQTT_USER", mqtt_user.getValue());
    preferences.putString("MQTT_PSWD", mqtt_pswd.getValue());
    preferences.putString("MQTT_TOPIC_OUT", mqtt_topic_out.getValue());
    preferences.putString("MQTT_TOPIC_IN", mqtt_topic_in.getValue());
    preferences.end();
    Debug("Parameters saved !");
    }

  ReadPreferences();

  #ifdef FOTA_ENABLED
  Fota();
  #endif
}

#endif // WIFIMANAGER_ENABLE


#ifdef FOTA_ENABLED
void Fota()
  {
  if ((WiFi.status() == WL_CONNECTED))
    {
    // Read Reference of installed Firmware
    preferences.begin(DeviceName, true);
    String FirmWareCourant = preferences.getString("FirmWare");
    preferences.end();
    WiFiClient client;

    //Look for FirmWare Update
    String FirmWareDispo="";
    const char * headerKeys[] = {"Last-Modified"};
    const size_t numberOfHeaders = 1;
    HTTPClient http;
    http.begin(FOTA_FILE);
    http.collectHeaders(headerKeys, numberOfHeaders);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
      FirmWareDispo=http.header((size_t)0);
      //int size = http.getSize();
      //if (size==-1) Serial.println("Taille inconnue");
      //else Serial.println("Taille Firmware ="+String(size));
    }
    else Debug("FOTA : Server Error : " + http.errorToString(httpCode));
    http.end();
    Debug();
    Debug("FOTA : Firware available = " + FirmWareDispo);

    //Download process
    if (FirmWareCourant==FirmWareDispo)
      { Debug("FOTA : already UpToDate !"); }
    else
      {
      //httpUpdate.setLedPin(Led_Pin, LOW); // Value for LED ON
      t_httpUpdate_return ret;
      Debug();
      Debug("*********************");
      Debug("FOTA : DOWNLOADING...");
      httpUpdate.rebootOnUpdate(false);
      ret = httpUpdate.update(client, FOTA_FILE);
      switch (ret)
        {
        case HTTP_UPDATE_FAILED:
          Debug(String("FOTA : Uploading Error !") + httpUpdate.getLastError() + httpUpdate.getLastErrorString().c_str());
          break;
        case HTTP_UPDATE_NO_UPDATES:
          Debug("FOTA : UpDate not Available");
          break;
        case HTTP_UPDATE_OK:
          Debug("FOTA : Update OK !!!");
          Debug("*********************");
          Debug();
          preferences.begin(DeviceName, false);
          preferences.putString("FirmWare", FirmWareDispo);
          preferences.end();
          delay(5000);
          ESP.restart();
          break;
        }
      }
    }
  }
  #endif // FOTA_ENABLED