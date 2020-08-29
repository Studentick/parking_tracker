
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>


const int buttonPin = 4;
int led = 2;
// Update these with values suitable for your network.

const char* ssid = "";
const char* password = "";
const char* mqtt_server = "srv1.clusterfly.ru";
int mqtt_port = 9124;
const char* mqtt_username = "user_32084635";
const char* mqtt_password = "pass_cc1949a7";
const char* mqtt_device_id = "user_32084635_parking_0001";
const char* mqtt_topic = "user_32084635/occupied";
const char* mqtt_topic_test = "user_32084635/change_state";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];


#define MSG_SIZE (10)
char text[MSG_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

boolean new_payload = false;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    if (strcmp(topic , mqtt_topic_test) == 0) {
    text[i] = (char)payload[i];
    //Serial.print(text);
    new_payload = true;
    }
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    //String clientId = "ESP8266Client-";
    //clientId += String(random(0xffff), HEX);
    // Attempt to connect
    
    if (client.connect(mqtt_device_id, mqtt_username, mqtt_password)) {
//    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(mqtt_topic, "false");
      // ... and resubscribe
      client.subscribe(mqtt_topic);
      client.subscribe(mqtt_topic_test);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  pinMode(led, OUTPUT);   
  pinMode(buttonPin, INPUT);   

}

int buttonState = 0; 
int timeSinceLastRead = 0;
boolean state_changed = true;
boolean state = false;
boolean last_state = false;
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
    ++value;

    if (state_changed){
    snprintf (msg, MSG_BUFFER_SIZE, "light state is %ld", buttonState);
    //snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(mqtt_topic, msg);
    state_changed = false;
    }

    //
  }
  
//buttonState = digitalRead(buttonPin);

  last_state = state;
  


char* t = "true";
char* f = "false";
  if(new_payload == true){
     Serial.print("Publish changes: ");
     Serial.println(text);
      if (strcmp(text, t) == 0) {
        buttonState = HIGH;
         digitalWrite(led, HIGH);
      }
     else {
      buttonState = LOW;
        digitalWrite(led, LOW); 
      }
     for (int i = 0; i < 10; i++) {
      text[i] = '\0';
     }
     Serial.print("Clear: ");
     Serial.println(text);
     new_payload = false;
    }

      // проверяем, не нажата ли кнопка.
  // если нажата, переводим buttonState в HIGH:
  if (buttonState == HIGH) {     
    // включаем светодиод:    
    digitalWrite(led, HIGH);
    state = true;  
  } 
  else {
    // выключаем светодиод:
     digitalWrite(led, LOW); 
     state = false;
  }
  if(last_state != state) {
  state_changed = true;
  }
}
