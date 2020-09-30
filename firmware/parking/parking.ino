#include "parking.h"
  

#define FIRST_SENSOR_PIN 12
#define SECOND_SENSOR_PIN 11

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
