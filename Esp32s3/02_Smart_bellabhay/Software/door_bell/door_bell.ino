#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define TOUCH_PIN 4
#define DOORBELL_PIN 19
#define relay 14

#define WLAN_SSID       "जय हनुमान"
#define WLAN_PASS       "jaishreeram"  // Your password

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "official_abhay_ghasal"             // Replace it with your username
#define AIO_KEY         "aio_dAbd79Jr8ujuaF9FKeko3Ne1HGli"   // Replace with your Project Auth Key

/************ Global State (you don't need to change this!) ******************/

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);


Adafruit_MQTT_Subscribe Door_Lock = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/relay");

void MQTT_connect();

void setup() {
  Serial.begin(115200);

  
  pinMode(relay, OUTPUT);
  pinMode(TOUCH_PIN, INPUT);
  // digitalWrite(DOORBELL_PIN, LOW);
  // digitalWrite(DOOR_LOCK_PIN, LOW);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("JAI SHREE RAM");
  }
  Serial.println();
  Serial.println("Connecting to WiFi");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // mqtt.subscribe(&DoorbellFeed);
  mqtt.subscribe(&Door_Lock);
}

void loop() {
  MQTT_connect();

  if(digitalRead(TOUCH_PIN) == HIGH){
    digitalWrite(DOORBELL_PIN, HIGH);
    delay(1000);
    digitalWrite(DOORBELL_PIN, LOW);
  }else{
    digitalWrite(DOORBELL_PIN, LOW);
  }
  
  
  

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(20000))) {
    if (subscription == &Door_Lock) {
      Serial.print(F("LOCK OPENED "));
      Serial.println((char *)Door_Lock.lastread);
      int Door_Lock_State = atoi((char *)Door_Lock.lastread);
      digitalWrite(relay, Door_Lock_State);
      // digitalWrite()
      
      // if (strcmp((char *)LockFeed.lastread, "unlock") == 0) {
      //   Serial.println("Received unlock signal. Opening lock...");
      //   digitalWrite(LOCK_PIN, HIGH);
      //   delay(1000);
      //   digitalWrite(LOCK_PIN, LOW);
      //   Serial.println("Lock opened.");
      // }
    }
  }
}

bool touchSensorTouched() {
  return digitalRead(TOUCH_PIN) == HIGH;
}

void MQTT_connect() {
  int8_t ret;
  
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;

  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);
    retries--;
    if (retries == 3) {
      while (1); // Infinite loop in case of connection failure
    }
  }
  Serial.println("MQTT Connected!");
}
