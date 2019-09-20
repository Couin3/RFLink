#if defined(MQTT_ACTIVATED) && defined(ESP32) || defined(ESP8266)

#ifdef ESP32
 #include <WiFi.h>
#elif ESP8266
 #include <ESP8266WiFi.h>
#endif

#include <PubSubClient.h>
#include "6_Credentials.h"

// Update these with values suitable for your network.

WiFiClient espClient;
PubSubClient client(espClient);


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }

  randomSeed(micros());

  //Serial.println("");
  //Serial.println("WiFi connected");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
}

void setup_MQTT() {
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  //Serial.print("Message arrived [");
  //Serial.print(topic);
  //Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.write(payload[i]);
  }
  Serial.write('\n');
  //Serial.println();

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
      //Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(MQTT_TOPIC_REC, "hello world");
      // ... and resubscribe
      client.subscribe(MQTT_TOPIC_TRANS);
    } else {
      //Serial.print("failed, rc=");
      //Serial.print(client.state());
      //Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void publishMsg(){
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  client.publish(MQTT_TOPIC_REC, pbuffer);
}

#endif
