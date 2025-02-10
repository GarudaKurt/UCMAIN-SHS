#include <SoftwareSerial.h>
#include "alarm.h"

SoftwareSerial BTSerial(2, 3); // HC-05 TX to D2, RX to D3
const int buttonPin = 4;
bool buttonPressed = false;
unsigned long prevTime = 0;
const unsigned long interval = 1000;

void setup() {
  Serial.begin(9600);  
  BTSerial.begin(9600);
  initAlarm();
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.println("Slave is waiting for button press...");
}

void loop() {
  if (digitalRead(buttonPin) == LOW && !buttonPressed) {
    buttonPressed = true;
    BTSerial.println("ALARM");
    Serial.println("ALARM Sent!");
    delay(500);
  } else if (digitalRead(buttonPin) == HIGH) {
    buttonPressed = false;
  }

  // if (BTSerial.available()) {
  //   String received = BTSerial.readStringUntil('\n');
  //   received.trim();
  //   Serial.println("Received from Master: " + received);

  //   if (received == "Onstate") {
  //     buzzerStop();
  //   } else {
  //     buzzerStart();
  //   }

  if (millis() - prevTime > interval) {
    BTSerial.println("active");
    prevTime = millis();
  }

}
