// ************************************* //
// * Arduino Project RFLink-esp        * //
// * https://github.com/couin3/RFLink  * //
// * 2018..2020 Stormteam - Marc RIVES * //
// * 2020 Schmutzm, Autoconnect Stuff  * //
// * 2020 Allexum, Web "Send" button   * //
// * More details in RFLink.ino file   * //
// ************************************* //

#include <Arduino.h>
#include "RFLink.h"

#include "1_Radio.h"
#include "4_Display.h" // To allow displaying the last message
#include "5_Plugin.h"
#include "6_WiFi_MQTT.h"
#include "8_OLED.h"
#include "9_FileConfig.h"

#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#define WIFI_FILE "/WiFi.json"
#define MQTT_FILE "/MQTT.json"
#define PINOUT_FILE "/Pinout.json"
#define DEBUG_FILE "/Debug.json"

void load_JSON_config()
{
    File file;
    StaticJsonDocument<1024> doc;
    DeserializationError error;

    Serial.print(F("Mount LittleFS : "));
    if (!LittleFS.begin())
        Serial.println(F("failed"));
    else
    {
        Serial.println(F("done"));

        Serial.print(F("Opening file "));
        Serial.print(WIFI_FILE);
        Serial.print(" : ");
        file = LittleFS.open(WIFI_FILE, "r");
        if (!file)
            Serial.println(F("failed"));
        else
        {
            Serial.println(F("done"));

            // Deserialize the JSON document
            error = deserializeJson(doc, file);

            if (error)
                Serial.println(F("Failed to read file, using default configuration"));

            // Close the file (Curiously, File's destructor doesn't close the file)
            file.close();

            // Copy values from the JsonDocument to the Config

            // local AP
            WIFI_SSID = doc["WIFI_SSID"] | "";
            WIFI_PSWD = doc["WIFI_PSWD"] | "";

            // static IP
            WIFI_IP = doc["WIFI_IP"] | "";
            WIFI_DNS = doc["WIFI_DNS"] | "";
            WIFI_GATEWAY = doc["WIFI_GATEWAY"] | "";
            WIFI_SUBNET = doc["WIFI_SUBNET"] | "";

            // More options
            // WIFI_HostName;
            WIFI_PWR = doc["WIFI_PWR"] | 20;

            doc.clear();
            doc.garbageCollect();
        }

        Serial.print(F("Opening file "));
        Serial.print(MQTT_FILE);
        Serial.print(" : ");
        file = LittleFS.open(MQTT_FILE, "r");
        if (!file)
            Serial.println(F("failed"));
        else
        {
            Serial.println(F("done"));

            // Deserialize the JSON document
            error = deserializeJson(doc, file);

            if (error)
                Serial.println(F("Failed to read file, using default configuration"));

            // Close the file (Curiously, File's destructor doesn't close the file)
            file.close();

            // Copy values from the JsonDocument to the Config

            // MQTT Server
            MQTT_SERVER = doc["MQTT_SERVER"] | "";
            MQTT_PORT = doc["MQTT_PORT"] | 1883;
            MQTT_ID = doc["MQTT_ID"] | "RFLink";
            MQTT_USER = doc["MQTT_USER"] | "";
            MQTT_PSWD = doc["MQTT_PSWD"] | "";

            // MQTT Topic
            MQTT_TOPIC_OUT = doc["MQTT_TOPIC_OUT"] | "/RFLink/msg";
            MQTT_TOPIC_IN = doc["MQTT_TOPIC_IN"] | "/RFLink/cmd";

            // MQTT options
            // MQTT_LOOP_MS
            MQTT_RETAINED = doc["MQTT_RETAINED"] | false;

            doc.clear();
            doc.garbageCollect();
        }

        Serial.print(F("Opening file "));
        Serial.print(PINOUT_FILE);
        Serial.print(" : ");
        File file = LittleFS.open(PINOUT_FILE, "r");
        if (!file)
            Serial.println(F("failed"));
        else
        {
            Serial.println(F("done"));

            // Deserialize the JSON document
            error = deserializeJson(doc, file);

            if (error)
                Serial.println(F("Failed to read file, using default configuration"));

            // Close the file (Curiously, File's destructor doesn't close the file)
            file.close();

            // Copy values from the JsonDocument to the Config

            // RFLink Pinout
            PIN_RF_RX_PMOS = String2GPIO(doc["PIN_RF_RX_PMOS"]) | PIN_RF_RX_PMOS_0;
            PIN_RF_RX_NMOS = String2GPIO(doc["PIN_RF_RX_NMOS"]) | PIN_RF_RX_NMOS_0;
            PIN_RF_RX_VCC = String2GPIO(doc["PIN_RF_RX_VCC"]) | PIN_RF_RX_VCC_0;
            PIN_RF_RX_GND = String2GPIO(doc["PIN_RF_RX_GND"]) | PIN_RF_RX_GND_0;
            PIN_RF_RX_NA = String2GPIO(doc["PIN_RF_RX_NA"]) | PIN_RF_RX_NA_0;
            PIN_RF_RX_DATA = String2GPIO(doc["PIN_RF_RX_DATA"]) | PIN_RF_RX_DATA_0;
            PIN_RF_TX_PMOS = String2GPIO(doc["PIN_RF_TX_PMOS"]) | PIN_RF_TX_PMOS_0;
            PIN_RF_TX_NMOS = String2GPIO(doc["PIN_RF_TX_NMOS"]) | PIN_RF_TX_NMOS_0;
            PIN_RF_TX_VCC = String2GPIO(doc["PIN_RF_TX_VCC"]) | PIN_RF_TX_VCC_0;
            PIN_RF_TX_GND = String2GPIO(doc["PIN_RF_TX_GND"]) | PIN_RF_TX_GND_0;
            PIN_RF_TX_NA = String2GPIO(doc["PIN_RF_TX_NA"]) | PIN_RF_TX_NA_0;
            PIN_RF_TX_DATA = String2GPIO(doc["PIN_RF_TX_DATA"]) | PIN_RF_TX_DATA_0;
            PULLUP_RF_RX_DATA = doc["PIN_RF_TX_DATA"] | false;

            // OLED Pinout
            PIN_OLED_GND = String2GPIO(doc["PIN_OLED_GND"]) | PIN_OLED_GND_0;
            PIN_OLED_VCC = String2GPIO(doc["PIN_OLED_VCC"]) | PIN_OLED_VCC_0;
            PIN_OLED_SCL = String2GPIO(doc["PIN_OLED_SCL"]) | PIN_OLED_SCL_0;
            PIN_OLED_SDA = String2GPIO(doc["PIN_OLED_SDA"]) | PIN_OLED_SDA_0;

            doc.clear();
            doc.garbageCollect();
        }
    }
    LittleFS.end();
}