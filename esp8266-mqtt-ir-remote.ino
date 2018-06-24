#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <PubSubClient.h>

#include <IRremoteESP8266.h>
#include <IRsend.h>
 
#define IR_SEND_PIN D2

#define DELAY_BETWEEN_COMMANDS 1000

IRsend irsend(IR_SEND_PIN);

const char* ssid = "";
const char* password = "";
const char* mqtt_server = "";
const char* mqtt_user_id = "";
const char* mqtt_topic = "";
const char* mqtt_user = "";
const char* mqtt_password = "";

WiFiClient espClient;
PubSubClient client(espClient);

uint16_t clean[15]  = {3000,1000,1000,3000,1000,3000, 1000,3000,3000,1000, 1000,3000, 1000,3000,1000};
uint16_t power[15]  = {3000,1000,1000,3000,1000,3000, 1000,3000,3000,1000, 1000,3000, 3000,1000,1000};  
uint16_t dock[15]   = {3000,1000,1000,3000,1000,3000, 1000,3000,3000,1000, 3000,1000, 3000,1000,3000};


void callback(char* topic, byte* payload, unsigned int length) {
 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] ");

 String strObject;

 for (int i=0;i<length;i++) {
  strObject += (char)payload[i];
 }

    Serial.println(strObject);
    if(strObject.equals("clean")) {
       Serial.println("Clean");
       roomba_send(136);
    }

  if(strObject.equals("power")) {
       Serial.println("power");
       roomba_send(138);
    }
  
  Serial.println();
}

void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {

 // Attempt to connect
 if (client.connect(mqtt_user_id, mqtt_user, mqtt_password)) {
  Serial.println("connected");
 
  client.subscribe(mqtt_topic);
 } else {
  Serial.print("failed, rc=");
  Serial.print(client.state());
  Serial.println(" try again in 5 seconds");
  // Wait 5 seconds before retrying
  delay(5000);
  }
 }
}

void setup(void){
  irsend.begin();
 
  Serial.begin(115200);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

 
}



void loop()
{
 if (!client.connected()) {
  reconnect();
 }
 client.loop();
}


//TODO pulse multiple times

void roomba_send(int code)
{

  int length = 8;
  uint16_t raw[length*2];
  uint16_t one_pulse = 3000;
  uint16_t one_break = 1000;
  uint16_t zero_pulse = one_break;
  uint16_t zero_break = one_pulse;

  int arrayposition = 0;
  // Serial.println("");
  for (int counter = length-1; counter >= 0; --counter) {
    if(code & (1<<counter)) {
      // Serial.print("1");
      raw[arrayposition] = one_pulse;
      raw[arrayposition+1] = one_break;
    }
    else {
      // Serial.print("0");
      raw[arrayposition] = zero_pulse;
      raw[arrayposition+1] = zero_break;
    }
    arrayposition = arrayposition + 2;
  }
  for (int i = 0; i < 3; i++) {
    irsend.sendRaw(raw, 15, 38);
    delay(50);
  }
  Serial.println("");

  Serial.print("Raw timings:");
   for (int z=0; z<length*2; z++) {
   Serial.print(" ");
   Serial.print(raw[z]);
   }
   Serial.print("\n\n");
}
