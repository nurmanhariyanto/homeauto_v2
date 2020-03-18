/*
    Develop by      :
    Email           :
    Project         :
    Version         :
*/
//library
/*
   Library
   Version:
   <ESP8266WiFi.h> 1.0.0
   <DNSServer.h> 1.1.0
   <ESP8266WebServer.h> 1.0.0
   <WiFiManager.h> 0.12.0i
   <ArduinoJson.h> 5.13.2
   <PubSubClient.h> 2.6.0
*/

/*
 * Librarry
 */
#include <FS.h> //this needs to be first, or it all crashes and burns...
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "config.h"
#include <EEPROM.h>
/////////////////////////////////////////////////////////


/*
   Initiate Variable
*/
StaticJsonBuffer<200> jsonBuffer;
void writeString(char add, String data);
String read_String(char add);
int loop_count  = 0 ; //loop count loop watchdog
char payloadTemp[200];
int soilSensor = A0;
int powerPin = 15;
String  recivedData;
char registerstatus[40] = "";
char aktivasistatus[40] = "";
char routingkey2[40] = "feedback_device";
int relay1 = D1 ;
int relay2 = D2 ;
int relay3 = D3 ;
int relay4 = D4 ;
int relay5 = D5 ;
int relay6 = D6 ;
int relay7 = D7 ;
int relay8 = D8 ;
///////////////////////////////////////////////////////////////////////////////


/*
   Setup wifi
*/
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
///////////////////////////////////////////////////////////////////////////////


/*
 * Callback from MQTT
 */
void callback(char * topic, byte * payload, unsigned int length) {
  char message [7] ;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  String convertMsg = String(message) ;
  String data = convertMsg.substring(5);
  int timer = data.toInt();
  Serial.println(message);

  //relay 1
  if (message[0] == '1') {
    digitalWrite(relay1, HIGH);
    Serial.println("relay 1 mati");
  } else {
    digitalWrite(relay1, LOW);
    Serial.println("relay 1 idup");
  }
  //relay 2
  if (message[1] == '1') {
    digitalWrite(relay2, HIGH);
    Serial.println("relay 2 mati");
    //    publish_ulang();
  } else {
    digitalWrite(relay2, LOW);
    Serial.println("relay 2 idup");
    //    publish_ulang();
  }

  //relay 3
  if (message[2] == '1') {
    digitalWrite(relay3, HIGH);
    Serial.println("relay 3 mati");
    //    publish_ulang();
  } else {
    digitalWrite(relay3, LOW);
    Serial.println("relay 3 idup");
    //    publish_ulang();
  }

  //relay 4
  if (message[3] == '1') {
    digitalWrite(relay4, HIGH);
    Serial.println("relay 4 mati");
    //    publish_ulang();
  } else {
    digitalWrite(relay4, LOW);
    Serial.println("relay 4 idup");
    //    publish_ulang();
  }

  //relay 5
  if (message[4] == '1') {
    digitalWrite(relay5, HIGH);
    Serial.println("relay 5 mati");
  } else {
    digitalWrite(relay5, LOW);
    Serial.println("relay 5 idup");
  }
  //relay 6
  if (message[5] == '1') {
    digitalWrite(relay6, HIGH);
    Serial.println("relay 6 mati");
    //    publish_ulang();
  } else {
    digitalWrite(relay6, LOW);
    Serial.println("relay 6 idup");
    //    publish_ulang();
  }

  //relay 7
  if (message[6] == '1') {
    digitalWrite(relay7, HIGH);
    Serial.println("relay 7 mati");
    //    publish_ulang();
  } else {
    digitalWrite(relay7, LOW);
    Serial.println("relay 7 idup");
    //    publish_ulang();
  }

  //relay 8
  if (message[7] == '1') {
    digitalWrite(relay8, HIGH);
    Serial.println("relay 7 mati");
    //    publish_ulang();
  } else {
    digitalWrite(relay8, LOW);
    Serial.println("relay 7 idup");
    //    publish_ulang();
  }

  delay (timer);
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);
  digitalWrite(relay5, HIGH);
  digitalWrite(relay6, HIGH);
  digitalWrite(relay7, HIGH);
  digitalWrite(relay8, HIGH);
  Serial.println("relay mati");
  //  publish_ulang();
  if (message >= 0) {
    publish_ulang();
  } else {
    Serial.println("0");
  }
}

/////////////////////////////////////////////////////////////////////

/*
 * Reconnect wifi and MQTT
 */
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...%s");
    Serial.println(mqtt_server);
    // Attempt to connect
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
        // Wait 5 seconds before retrying
        //delay(10000);
      }
    }
  }
}

/////////////////////////////////////////////////////////


/*
 * Setup pin mode, serial,ETC
 */
void setup() {
  //setup pin mode
  //  pinMode(soilSensor, INPUT_PULLUP);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  pinMode(powerPin, OUTPUT);
  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);
  digitalWrite(D3, HIGH);
  digitalWrite(D4, HIGH);
  Serial.begin(115200);
  EEPROM.begin(512);
  Serial.println(F("Booting...."));
  //read config wifi,mqtt dan yang lain
  ReadConfigFile();
  setup_wifi();
  SaveConfigFile();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(aktivasi);
}
////////////////////////////////////////////////////////

/*
 * Loop main
 */

void loop() {
  recivedData = read_String(10);
  Serial.println("Ini datanya:");
  Serial.println(recivedData);
  delay(1000);
  //  int pesan=1;
  if (recivedData == "1") {
    client.setCallback(callback);
    client.loop();
  } else {
    client.setCallback(aktivasi);
    client.loop();
    Serial.println("Device Belum Teraktivasi");
  }
  if (!client.connected()) {
    reconnect();
  }
}
/////////////////////////////////////////////////////////


/*
 * Publish message feedback from device
 */
void publish_ulang () {

  //const char tambahan=MAC_char,"Ast";
  String berhasil = "berhasil";
  String pubmsg = "";
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["mac"] = MAC_char;
  //  root["waktu siram"] =timer;
  root["pesan"] = berhasil;
  root.printTo(pubmsg);
  Serial.println(pubmsg);
  Serial.println(MAC_char);
  client.publish(routingkey2 , pubmsg.c_str());
  //client.publish(tambahan());
  delay(3000);
}
/////////////////////////////////////////////////////////


/*
 * Write data ke ROM device
 */
void writeString(char add, String data) {
  int _size = data.length();
  int i;
  for (i = 0; i < _size; i++)
  {
    EEPROM.write(add + i, data[i]);
  }
  EEPROM.write(add + _size, '\0');
  EEPROM.commit();
}
///////////////////////////////////////////


/*
 * Read data dari ROM device
 */
String read_String(char add) {
  int i;
  char data[100];
  int len = 0;
  unsigned char k;
  k = EEPROM.read(add);
  while (k != '\0' && len < 500) {
    k = EEPROM.read(add + len);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  return String(data);
}
///////////////////////////////////////////

/*
 * Aktivasi device
 */
void aktivasi(char * topic, byte * payload, unsigned int length) {
  String pesan;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  for (int i = 0; i < length; i++) {
    pesan = (char)payload[i];
    Serial.print(pesan);
    writeString(10, pesan);
  }
}
///////////////////////////////////////////
