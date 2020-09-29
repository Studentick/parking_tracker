#include <NewPing.h>
#include <ArduinoJson.h>

#define FIRST_SENSOR_PIN 12
#define SECOND_SENSOR_PIN 11

#define MAX_CLEARANCE 40
#define MIN_CLEARANCE 5
#define MAX_DISTANCE 400
#define FREE_DISTANCE 350
#define FIRST_SENSOR 0
#define SECOND_SENSOR 1

class Parking {
  public:
    Parking(uint8_t _firstSensorPin, uint8_t _secondSensorPin) {
      status = "initialization..";
      firstSensorPin = _firstSensorPin;
      secondSensorPin = _secondSensorPin;
    }
    
    void setClearance(long _minClearance, long _maxClearance) {
      maxClearance = _maxClearance;
      minClearance = _minClearance;
    }

    void scan() {
      unsigned int firstSensorData = getDuration(FIRST_SENSOR);
      unsigned int secondSensorData = getDuration(SECOND_SENSOR);
      
      if(isOccupied(firstSensorData, secondSensorData)) {
        status = "occupied";
        return;
      }
      
      if ((firstSensorData >= FREE_DISTANCE) && (secondSensorData >= FREE_DISTANCE)) {
        status = "free";
        return;
      }      
      
      if ((firstSensorData < minClearance) || (secondSensorData < minClearance)){
        status = "blocked";
        return;
      }
      
      if ((firstSensorData > maxClearance) || (secondSensorData > maxClearance)) {
        status = "obstacle";
        return;
      }

      status = "unknown error";
    }

    String getJsonData() {
      String json = "";
      
      serializeJson(dataToJson(),json);
      return json;
    }  

    String getStatus() {
      return status;
    }  
        
  private: 
    const uint8_t id = 0001;
    int firstSensorPin;
    int secondSensorPin;
    
    unsigned int maxClearance = MAX_CLEARANCE;
    unsigned int minClearance = MIN_CLEARANCE;

    String status = "not initialized";

    //Array of ultrasonic sensors
    NewPing sonar[2]{     
      NewPing(firstSensorPin, firstSensorPin, MAX_DISTANCE),
      NewPing(secondSensorPin, secondSensorPin, MAX_DISTANCE)
      };

    unsigned int getDuration(int index) {
      unsigned int duration;
	    delay(50);
      duration = sonar[index].ping_cm();
      
      return duration;
    }

    boolean isOccupied(int firstSensorData, int secondSensorData){
      boolean firstSensorState = ((firstSensorData > minClearance) && (firstSensorData < maxClearance));
      boolean secondSensorState = ((secondSensorData > minClearance) && (secondSensorData < maxClearance));
      
      return firstSensorState && secondSensorState;
    }     

    StaticJsonDocument<200> dataToJson() {
      StaticJsonDocument<200> doc;
      
      doc["id"] = id;
      doc["status"] = status;
      
      return doc;
    }  
  };

Parking *parking;

void setup() {
  Serial.begin(9600);
  parking = new Parking(FIRST_SENSOR_PIN,SECOND_SENSOR_PIN);
}

void loop() {
    Serial.println("Json: ");
    Serial.println(parking->getJsonData());

    parking->scan();

    Serial.print("Status: ");
    Serial.println(parking->getStatus());
    delay(1000);
}
