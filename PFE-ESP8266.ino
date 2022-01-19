#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NewPing.h>
#include <Ultrasonic.h>
#include "DHT.h"        

#define DHTPIN D4   
#define DHTTYPE DHT11   
#define wifi_ssid "HUAWEI y5"
#define wifi_password "123456HH"
#define temperature_topic "sensor/temperature"  
#define humidity_topic "sensor/humidity"        
#define distance_topic "sensor/distance"

char message_buff[100];
long lastMsg = 0;   
long lastRecu = 0;
bool debug = false;  
int distance;

DHT dht(DHTPIN, DHTTYPE);     
WiFiClient espClient;
PubSubClient client(espClient);
IPAddress ip;               

int leftMotorForward = D5;     
int rightMotorForward = D7;  
int leftMotorBackward = D3;    
int rightMotorBackward = D6;  

int rightMotorENB = D8; 
int leftMotorENB = D2;

Ultrasonic ultrasonic(D0,D1);

void setup() {
  Serial.begin(9600);     
  pinMode(D0,OUTPUT);         
  setup_wifi();           
  client.setClient(espClient) ;
  client.setServer("192.168.43.252",1883); 
  client.setCallback(callback);     
  dht.begin();
  client.subscribe("load1");
  pinMode(leftMotorForward, OUTPUT);
  pinMode(rightMotorForward, OUTPUT); 
  pinMode(leftMotorBackward, OUTPUT);  
  pinMode(rightMotorBackward, OUTPUT);

  pinMode(leftMotorENB, OUTPUT); 
  pinMode(rightMotorENB, OUTPUT);
 
}


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connexion a ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connexion WiFi etablie ");
  Serial.print("=> Addresse IP : ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connexion au serveur MQTT...");
    if (client.connect("EspClient")) { 
      Serial.println("OK");
    } else {
      Serial.print("KO, erreur : ");
      Serial.print(client.state());
      Serial.println(" On attend 5 secondes avant de recommencer");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  long now = millis();
  if (now - lastMsg > 100 * 10) {
    lastMsg = now;
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    distance = ultrasonic.read(CM);


    client.publish(temperature_topic, String(t).c_str(), true);  
    client.publish(humidity_topic, String(h).c_str(), true);
    client.publish(distance_topic, String(distance).c_str(), true);
   }
  if (now - lastRecu > 100 ) {
    lastRecu = now;
  }
  if  ( distance <= 8 ) {
  digitalWrite(leftMotorENB,LOW);
  digitalWrite(rightMotorENB,LOW);
  digitalWrite(leftMotorBackward,LOW);
  digitalWrite(rightMotorBackward,LOW);
  digitalWrite(leftMotorForward,LOW);
  digitalWrite(rightMotorForward,LOW); 
  
  }
}

 


void callback(char* topic, byte* payload, unsigned int length) {

  int i = 0;
    Serial.println("Message recu =>  topic: " + String(topic));
    Serial.print(" | longueur: " + String(length,DEC));
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  
  String msgString = String(message_buff);


  if ( msgString == "1" ) {
    digitalWrite(D0,HIGH);  
  digitalWrite(leftMotorENB,HIGH);
  digitalWrite(rightMotorENB,HIGH);
  digitalWrite(leftMotorBackward,LOW);
  digitalWrite(rightMotorBackward,LOW);
  digitalWrite(leftMotorForward,HIGH);
  digitalWrite(rightMotorForward,HIGH); 
  } 
  else if ( msgString == "2" ){
  digitalWrite(leftMotorENB,HIGH);
  digitalWrite(rightMotorENB,HIGH);
  digitalWrite(leftMotorBackward,HIGH);
  digitalWrite(rightMotorBackward,HIGH);
  digitalWrite(leftMotorForward,LOW);
  digitalWrite(rightMotorForward,LOW); 
  }
  else if  ( msgString == "3" ) {
  digitalWrite(leftMotorENB,HIGH);
  digitalWrite(rightMotorENB,HIGH);
  digitalWrite(leftMotorBackward,LOW);
  digitalWrite(rightMotorBackward,LOW);
  digitalWrite(leftMotorForward,HIGH);
  digitalWrite(rightMotorForward,LOW); 
  }
   else if  ( msgString == "4" ) {
  digitalWrite(leftMotorENB,HIGH);
  digitalWrite(rightMotorENB,HIGH);
  digitalWrite(leftMotorBackward,LOW);
  digitalWrite(rightMotorBackward,LOW);
  digitalWrite(leftMotorForward,LOW);
  digitalWrite(rightMotorForward,HIGH); 
  }
  else if  ( msgString == "5" ) {
  digitalWrite(leftMotorENB,LOW);
  digitalWrite(rightMotorENB,LOW);
  digitalWrite(leftMotorBackward,LOW);
  digitalWrite(rightMotorBackward,LOW);
  digitalWrite(leftMotorForward,LOW);
  digitalWrite(rightMotorForward,LOW); 
  }
}
