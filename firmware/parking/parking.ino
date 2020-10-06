#include "parking.h"
#include <SoftwareSerial.h>
  
#define ID 0001
#define RX_PIN 2
#define TX_PIN 3
#define DYN_JSON_DOC_SIZE 1024
#define PARKING_COUNT 1
#define FIRST_PIN 12
#define LAST_PIN 11

SoftwareSerial inputSerial(RX_PIN, TX_PIN);
DynamicJsonDocument doc(DYN_JSON_DOC_SIZE);
Parking *parkings[PARKING_COUNT];

void setup() {
  Serial.begin(115200);
  inputSerial.begin(115200);
  
  defineParkings();
}

void loop() {
  // Read data from SUART 
  String input = readSerialData();
    
  //Add data to JSON array 
  JsonArray controllers = doc.createNestedArray("controllers");
  controllers.add(input);
  
  //Check all sensors
  scanning();
  
  //Get sensors data and add to JSON array
  String sensorsData = getJsonSensorsData();
  controllers.add(sensorsData);
  
  //Print the resulting JSON to string 
  String output;
  serializeJson(doc, output);
  
  //Write data to UART
  writeSerialData(output);
    
  delay(1000);
}

void defineParkings() { 
  int currentPin = FIRST_PIN;
  int index = 0;
    
  while ( index < PARKING_COUNT && currentPin <= LAST_PIN){
    parkings[index] = new Parking(currentPin++,currentPin++);
      
    index++;
  }
}

String readSerialData() {
  String data = "";
  char character;
  
  while (inputSerial.available() > 0) {
    character = inputSerial.read(); 
    data += character; 
  }
  return data;  
}

void writeSerialData(String data) {
  if (data != "") {
    char* CharData;                                    
    data.toCharArray(CharData, data.length());
    Serial.write(CharData);                           
  }
}

void scanning() {
  for(int index = 0; index < PARKING_COUNT; index++) {
    parkings[index]->scan();
  }
}

String getJsonSensorsData() {
  StaticJsonDocument<JSON_DOC_SIZE> currentControllerDoc;
  
  currentControllerDoc["arduinoId"] = ID;
  JsonArray sensors = currentControllerDoc.createNestedArray("sensors");  
  
  for(int index = 0; index < PARKING_COUNT; index++) {
    sensors.add(parkings[index]->getJsonData());
  }
    
  String data = "";
  serializeJson(currentControllerDoc, data);

  return data;
}
