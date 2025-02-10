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
    received.trim(); // Remove any extra spaces or newlines
    Serial.println("Received: " + received);

    if (received == "ALARM") {
      buzzerStart();
      Serial.println("ALERT: Alarm Button Pressed on Slave!");
      delay(5000);  
      buzzerStop();
    } else if (received == "active") {
      prevTime = millis();
      buzzerStop();
      Serial.println("Activated!");
    } else {
      Serial.println("Data from slave: "+ received);
      Serial.println("De-activated!");
      buzzerStart();
    }
    //BTSerial.println("Onstate"); // Send state update back to slave
  }

  if (millis() - prevTime > interval) {
    Serial.println("Slave Disconnected! Activating buzzer!");
    buzzerStart(); // Trigger buzzer when slave is out of range
  }

}
