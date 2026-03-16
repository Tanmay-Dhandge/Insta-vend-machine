#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "WIFI";
const char* password = "PASS";

const char* mqtt_server =
"YOUR_CLUSTER.s1.eu.hivemq.cloud";

WiFiClientSecure espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {

String message;

for(int i=0;i<length;i++)
message += (char)payload[i];

Serial.println(message);

if(message.indexOf("chips")>=0)
{
digitalWrite(5,HIGH);
delay(2000);
digitalWrite(5,LOW);
}

}

void setup(){

Serial.begin(115200);

WiFi.begin(ssid,password);

while(WiFi.status()!=WL_CONNECTED)
delay(500);

client.setServer(mqtt_server,8883);
client.setCallback(callback);

}

void loop(){

if(!client.connected())
{

client.connect("esp32","USERNAME","PASSWORD");

client.subscribe("vending/order");

}

client.loop();

}
