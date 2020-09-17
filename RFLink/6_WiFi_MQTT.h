// ************************************* //
// * Arduino Project RFLink-esp        * //
// * https://github.com/couin3/RFLink  * //
// * 2018..2020 Stormteam - Marc RIVES * //
// * More details in RFLink.ino file   * //
// ************************************* //

#ifndef WiFi_MQTT_h
#define WiFi_MQTT_h

#include <Arduino.h>
#include "RFLink.h"

#ifdef ESP32
#include <WiFi.h>
#elif ESP8266
#include <ESP8266WiFi.h>
#endif

// local AP
extern String WIFI_SSID;
extern String WIFI_PSWD;

// static IP
extern String WIFI_IP;
extern String WIFI_DNS;
extern String WIFI_GATEWAY;
extern String WIFI_SUBNET;

extern String WIFI_HOST;
extern byte WIFI_PWR;

#ifdef MQTT_ENABLED
// MQTT Server
extern String MQTT_SERVER;
extern uint16_t MQTT_PORT;
extern String MQTT_ID;
extern String MQTT_USER;
extern String MQTT_PSWD;

// MQTT Topic
extern String MQTT_TOPIC_OUT;
extern String MQTT_TOPIC_IN;

// MQTT options
extern boolean MQTT_RETAINED;
extern char MQTTbuffer[PRINT_BUFFER_SIZE]; // Buffer for MQTT message

void setup_WIFI();
void setup_MQTT();
void reconnect();
void publishMsg();
void checkMQTTloop();
#endif // MQTT_ENABLED

#if (!defined(MQTT_ENABLED))
#if (defined(ESP32) || defined(ESP8266))
void setup_WIFI_OFF();
#endif
#endif

#endif // WiFi_MQTT_h