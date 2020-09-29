#include "parking.h"
#include <Arduino.h>
#include <NewPing.h>
#include <ArduinoJson.h>
 
Parking::Parking(uint8_t firstSensorPin, uint8_t secondSensorPin) {
	_status = "initialization..";
    _firstSensorPin = firstSensorPin;
    _secondSensorPin = secondSensorPin;
}
  
void Parking::setClearance(long minClearance, long maxClearance) {
    _maxClearance = maxClearance;
    _minClearance = minClearance;
} 

void Parking::scan() {
    unsigned int firstSensorData = getDuration(FIRST_SENSOR);
    unsigned int secondSensorData = getDuration(SECOND_SENSOR);
      
    if(isOccupied(firstSensorData, secondSensorData)) {
      _status = "occupied";
      return;
    }
      
    if ((firstSensorData >= FREE_DISTANCE) && (secondSensorData >= FREE_DISTANCE)) {
      _status = "free";
      return;
    }      
      
    if ((firstSensorData < _minClearance) || (secondSensorData < _minClearance)){
      _status = "blocked";
      return;
    }
      
    if ((firstSensorData > _maxClearance) || (secondSensorData > _maxClearance)) {
      _status = "obstacle";
      return;
    }

    _status = "unknown error";
}
  
String Parking::getJsonData() {
    String json = "";
      
    serializeJson(dataToJson(),json);
    return json;
}  

String Parking::getStatus() {
    return _status;
}

unsigned int Parking::getDuration(int index) {
    unsigned int duration;
    duration = sonar[index].ping_cm();
      
    return duration;
}

boolean Parking::isOccupied(int firstSensorData, int secondSensorData) {
    boolean firstSensorState = ((firstSensorData > _minClearance) && (firstSensorData < _maxClearance));
    boolean secondSensorState = ((secondSensorData > _minClearance) && (secondSensorData < _maxClearance));
      
    return firstSensorState && secondSensorState;
}   
  
StaticJsonDocument<JSON_DOC_SIZE> Parking::dataToJson() {
    StaticJsonDocument<JSON_DOC_SIZE> doc;
      
    doc["id"] = _id;
    doc["status"] = _status;
      
    return doc;
} 