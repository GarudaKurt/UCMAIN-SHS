#include <SoftwareSerial.h>
#include "alarm.h"

SoftwareSerial BTSerial(2, 3); // HC-05 TX to D2, RX to D3

unsigned long prevTime = 0;
const unsigned long interval = 5000;

void setup() {
  Serial.begin(9600);  
  BTSerial.begin(9600);
  initAlarm();
  Serial.println("Master waiting for data...");
}

void loop() {
  if (BTSerial.available()) {
    String received = BTSerial.readStringUntil('\n'); // Read Bluetooth data
    received.trim(); // Remove extra spaces or newlines

    if (received.indexOf("|") >= 0) {
      String trimmedData = received.substring(0, received.indexOf("|")); // Remove "|"
      if (trimmedData == "A") {
        prevTime = millis(); // Reset timeout countdown
        buzzerStop();
        Serial.println("Slave is active!");
      } else {
        Serial.println("Unexpected Data: " + trimmedData);
        buzzerStart();
      }
    } else if (received == "ALARM") {
      buzzerStart();
      Serial.println("ALERT: Alarm Button Pressed on Slave!");
      delay(5000);
      buzzerStop();
    } else {
      Serial.println("Unrecognized Data: " + received);
    }
  }

  if (millis() - prevTime > interval) {
    Serial.println("Slave Disconnected! Activating buzzer!");
    buzzerStart(); // Trigger buzzer when slave is out of range
  }

}
