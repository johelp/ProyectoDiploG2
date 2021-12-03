#include <Arduino.h>
#include <WiFi.h> //<WiFi101.h>
#include <MQTT.h>

const char ssid[] = "ariel";
const char pass[] = "pollosss";

WiFiClient net;
MQTTClient client;


const int freq =5000; // freq for PWM
const int ledChannel = 0; // channel pwm
const int resolution = 10; // resolution 8,10,12,15

int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

long duration;
float distanceCm;
#define SOUND_SPEED 0.034
unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("arduino")) { //, "public", "public")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("brillo");
  client.subscribe("led1");
  client.subscribe("led2");
  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  // set the brightness of pin 2:
  
  if (topic=="brillo") {
      ledcWrite(ledChannel, payload.toInt());
  }  
  if (topic=="led1") {
    if (payload=="on") digitalWrite(26, HIGH);
    if (payload=="off") digitalWrite(26, LOW);
  }
  if (topic=="led2") {
    if (payload=="on") digitalWrite(27, HIGH);
    if (payload=="off") digitalWrite(27, LOW);
  }
  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

float medicion(){
  // Clears the trigPin
    digitalWrite(12, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(12, HIGH);
    delayMicroseconds(10);
    digitalWrite(12, LOW);
    duration = pulseIn(13, HIGH);
    distanceCm = duration * SOUND_SPEED/2;
    return distanceCm;

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(25,OUTPUT); // LED1
  pinMode(26,OUTPUT); // LED2
  pinMode(27,OUTPUT); // LED3
  pinMode(34,INPUT);  //pulsadores
  pinMode(35,INPUT);  //pulsadores
  pinMode(13,INPUT);  // ECHO
  pinMode(12,OUTPUT); // pulse
  ledcSetup(ledChannel,freq,resolution);
  ledcAttachPin(25,ledChannel); // LED1 como PWM
  //ledcAttachPin(26,ledChannel);
  //ledcAttachPin(27,ledChannel);

  WiFi.begin(ssid, pass);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.
  //
  // MQTT brokers usually use port 8883 for secure connections.
  client.begin("192.168.0.132", 1883, net);
  client.onMessage(messageReceived);

  connect();
 
}

void loop() {
  
  client.loop(); // Si o si tiene que estar en el loop

  if (!client.connected()) {
    connect();
  }

  if ((millis() - lastTime) > timerDelay) {
    lastTime = millis();
    client.publish("distancia", String(medicion()));
  }
 
  
    
    
    
  
    
}