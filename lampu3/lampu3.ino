#include <FS.h> //this needs to be first, or it all crashes and burns...
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "config.h"



StaticJsonBuffer<200> jsonBuffer;
char payloadTemp[200];

char routingkey2[40] = "feedback_device";
String statusDevice[8] = {"0", "0", "0", "0", "0", "0", "0", "0"};
int relay1 = D1 ;
int relay2 = D2 ;
int relay3 = D3 ;
int relay4 = D4 ;
int relay5 = D5 ;
int relay6 = D6 ;
int relay7 = D7 ;
int relay8 = D8 ;
char message [100] ;



void setup_wifi() {
  WiFi.macAddress(MAC_array);
  for (int i = 0; i < sizeof(MAC_array) - 1; ++i) {
    sprintf(MAC_char, "%s%02x:", MAC_char, MAC_array[i]);
  }
  sprintf(MAC_char, "%s%02x", MAC_char, MAC_array[sizeof(MAC_array) - 1]);
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  String(mqtt_port).toCharArray(smqtt_port, 5);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", smqtt_port, 5);
  WiFiManagerParameter custom_mqtt_user("user", "mqtt user", mqtt_user, 40);
  WiFiManagerParameter custom_mqtt_password("password", "mqtt password", mqtt_password, 40);
  WiFiManagerParameter custom_mqtt_keywords1("keyword1", "mqtt keyword1", MAC_char, 40);
  //WiFiManagerParameter custom_mqtt_keywords2("keyword2", "mqtt keyword2", mqtt_keywords2, 40);
  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter( & custom_mqtt_server);
  wifiManager.addParameter( & custom_mqtt_port);
  wifiManager.addParameter( & custom_mqtt_user);
  wifiManager.addParameter( & custom_mqtt_password);
  wifiManager.addParameter(&custom_mqtt_keywords1);
  if (!wifiManager.autoConnect(MAC_char, "password")) {
    Serial.println("failed to connect and hit timeout");
    delay(2000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(2000);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



void publish_ulang () {

  //const char tambahan=MAC_char,"Ast";

  String value = String(statusDevice[0] + statusDevice[1] + statusDevice[2] + statusDevice[3] + statusDevice[4] + statusDevice[5] + statusDevice[6] + statusDevice[7]);
  char dataValue[50];
  value.toCharArray(dataValue, sizeof(value));
  String pubmsg = "";
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["mac"] = MAC_char;
  //  root["waktu siram"] =timer;
  root["value"] = dataValue;
  root.printTo(pubmsg);
  if (client.publish(routingkey2 , pubmsg.c_str()) == true) {
    Serial.println("Success sending to RMQ");
  }
  else
  {
    Serial.println("failed sending to RMQ");
  }

  //client.publish(tambahan());
}



/*
   Callback from MQTT
*/
void callback(char * topic, byte * payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");

  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  

  if (message[0] == '1') {
    digitalWrite(relay1, HIGH);
    Serial.println("relay 1 on");
    statusDevice[0] = "1";

  }
  if (message[0] == '0') {
    digitalWrite(relay1, LOW);
    Serial.println("relay 1 off");
    statusDevice[0] = "0";
  }

  //relay 2
  if (message[1] == '1') {
    digitalWrite(relay2, HIGH);
    Serial.println("relay 2 on");
    statusDevice[1] = "1";

  }
  if (message[1] == '0') {
    digitalWrite(relay2, LOW);
    Serial.println("relay 2 off");
    statusDevice[1] = "0";
  }

  //relay 3
  if (message[2] == '1') {
    digitalWrite(relay3, HIGH);
    Serial.println("relay 3 on");
    statusDevice[2] = "1";

  }
  if (message[2] == '0') {
    digitalWrite(relay3, LOW);
    Serial.println("relay 3 off");
    statusDevice[2] = "0";
  }

  //relay 4
  if (message[3] == '1') {
    digitalWrite(relay4, HIGH);
    Serial.println("relay 4 on");
    statusDevice[3] = "1";

  }
  if (message[3] == '0') {
    digitalWrite(relay4, LOW);
    Serial.println("relay 4 off");
    statusDevice[3] = "0";
  }

  //relay 5
  if (message[4] == '1') {
    digitalWrite(relay5, HIGH);
    Serial.println("relay 5 on");
    statusDevice[4] = "1";

  }
  if (message[4] == '0') {
    digitalWrite(relay5, LOW);
    Serial.println("relay 5 off");
    statusDevice[4] = "0";
  }

  //relay 6
  if (message[5] == '1') {
    digitalWrite(relay6, HIGH);
    Serial.println("relay 5 on");
    statusDevice[5] = "1";

  }
  if (message[5] == '0') {
    digitalWrite(relay5, LOW);
    Serial.println("relay 6 off");
    statusDevice[5] = "0";
  }

  //relay 7
  if (message[6] == '1') {
    digitalWrite(relay7, HIGH);
    Serial.println("relay 7 on");
    statusDevice[6] = "1";

  }
  if (message[6] == '0') {
    digitalWrite(relay7, LOW);
    Serial.println("relay 7 off");
    statusDevice[6] = "0";
  }

  //relay 8
  if (message[7] == '1') {
    digitalWrite(relay8, HIGH);
    Serial.println("relay 8 on");
    statusDevice[7] = "1";

  }
  if (message[7] == '0') {
    digitalWrite(relay8, LOW);
    Serial.println("relay 8 off");
    statusDevice[7] = "0";
  }

  publish_ulang();

  delay(50);
}




void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...%s");
    Serial.println(mqtt_server);
    if (client.connect(MAC_char, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      Serial.println(MAC_char);
      client.subscribe(MAC_char);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      if (client.state() == 4) ESP.restart();
      else {
        Serial.println(" try again in 5 seconds");
      }
    }
  }
}




void setup() {
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);
  digitalWrite(D5, LOW);
  digitalWrite(D6, LOW);
  digitalWrite(D7, LOW);
  digitalWrite(D8, LOW);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}




void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}
