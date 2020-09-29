#ifndef parking_h
#define parking_h
#include <Arduino.h>
#include <NewPing.h>
#include <ArduinoJson.h>

#define MAX_CLEARANCE 40
#define MIN_CLEARANCE 5
#define MAX_DISTANCE 400
#define FREE_DISTANCE 350
#define SENSOR_COUNT 2
#define FIRST_SENSOR 0
#define SECOND_SENSOR 1
#define JSON_DOC_SIZE 200

class Parking {
public:
	Parking(uint8_t firstSensorPin, uint8_t secondSensorPin);
    void setClearance(long minClearance, long maxClearance);
    void scan();
    String getJsonData();
    String getStatus(); 
private: 
    const uint8_t _id = 0001;
    int _firstSensorPin;
    int _secondSensorPin;    
    unsigned int _maxClearance = MAX_CLEARANCE;
    unsigned int _minClearance = MIN_CLEARANCE;
    String _status = "not initialized";
    //Array of ultrasonic sensors
    NewPing sonar[SENSOR_COUNT]{     
      NewPing(_firstSensorPin, _firstSensorPin, MAX_DISTANCE),
      NewPing(_secondSensorPin, _secondSensorPin, MAX_DISTANCE)
      };
    unsigned int getDuration(int index);
    boolean isOccupied(int firstSensorData, int secondSensorData);
    StaticJsonDocument<JSON_DOC_SIZE> dataToJson();
}; 
	
#endif