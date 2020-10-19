#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>

#define SSID ""
#define PASSWORD ""

#define MQTT_SERVER "srv1.clusterfly.ru"
#define MQTT_USERNAME "user_32084635"
#define MQTT_PASSWORD "pass_cc1949a7"

#define MQTT_DEVICE_ID "user_32084635_connector_0001"
#define MQTT_TOPIC "user_32084635/device_state"
#define MQTT_TOPIC_TEST "user_32084635/test"

#define MQTT_PORT 9124
#define JSON_DOC_SIZE 200

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

#define MSG_SIZE (10)
char text[MSG_SIZE];

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

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
    if (strcmp(topic , MQTT_TOPIC_TEST) == 0) {
    text[i] = (char)payload[i];
    new_payload = true;
    }
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
    digitalWrite(BUILTIN_LED, LOW);       
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    if (client.connect(MQTT_DEVICE_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(MQTT_TOPIC, "false");
      // ... and resubscribe
      client.subscribe(MQTT_TOPIC);
      client.subscribe(MQTT_TOPIC_TEST);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

String readSerialData() {
  String data = "";
  char character;
  
  while (Serial.available() > 0) {
    character = Serial.read(); 
    data += character; 
  }
  return data;  
}

String readDataError() {
  StaticJsonDocument<JSON_DOC_SIZE> doc;
  
  doc["mqtt_device_id"] = MQTT_DEVICE_ID;
  doc["error_message"] = "Can't read data from device";
  
  String data = "";
  serializeJson(doc, data);

  return data;
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  Serial.flush();
  
  unsigned long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
    
    String jsonData = "";
    jsonData = readSerialData();

    if(jsonData == "") {
      jsonData = readDataError();
    }

    char charBuffer_jsonData[jsonData.length()+1];
    jsonData.toCharArray(charBuffer_jsonData, jsonData.length()+1);

    client.publish(MQTT_TOPIC, charBuffer_jsonData);
  }
}
