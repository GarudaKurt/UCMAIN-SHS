#include <ArduinoBLE.h>
#include "firebaseConfig.h"

BLEService myService("12345678-1234-1234-1234-123456789012");
BLEStringCharacteristic myCharacteristic("87654321-4321-4321-4321-210987654321", BLERead | BLEWrite, 20);

const int RSSI_THRESHOLD = -100;

static bool isDeviceFar = false;
static bool wasDisconnected = false;
static bool isBuzzerActive = false;
static bool wasConnected = false;

unsigned long prevTime = 0;
unsigned long lastCheckTime = 0;
unsigned long buzzerStartTime = 0; 

#define pinBuzzer 15

void setup() {
  Serial.begin(115200);
  while (!Serial);
  pinMode(pinBuzzer, OUTPUT);
  initFirebase();

  if (!BLE.begin()) {
    Serial.println("Failed to initialize BLE!");
    while (1);
  }
  Serial.println("BLE initialized");

  BLE.setLocalName("ESP32_BLE_Device");
  BLE.setAdvertisedService(myService);

  myService.addCharacteristic(myCharacteristic);
  BLE.addService(myService);

  myCharacteristic.writeValue("Hello BLE!");
  BLE.advertise();

  Serial.println("Waiting for BLE connections...");
}

void loop() {
  BLEDevice central = BLE.central();

  if (millis() - lastCheckTime >= 2000) {
    lastCheckTime = millis();
    if (readEvents()) { 
      Serial.println("[DEBUG] Firebase triggered the buzzer!");
      activateBuzzer();
    }
  }

  if (isBuzzerActive && millis() - buzzerStartTime >= 5000) {
    digitalWrite(pinBuzzer, LOW);
    isBuzzerActive = false;
  }

  if (central) {
    if (!wasConnected) { 
      wasConnected = true;
      Serial.print("Connected to: ");
      Serial.println(central.address());
      sendEvents(true);
    }

    if (wasDisconnected) { 
      wasDisconnected = false;
      Serial.println("Device reconnected within range.");
      sendEvents(true);  
    }

    if (millis() - prevTime >= 1000) {
      prevTime = millis();  
      int rssi = central.rssi(); 
      Serial.print("Current RSSI: ");
      Serial.println(rssi);

      if (rssi < RSSI_THRESHOLD) {
        Serial.println("Warning: Device is more than 4 meters away!");
        isDeviceFar = true;
        activateBuzzer(); 
        sendEvents(true);
        wasDisconnected = true;
      } else {
        isDeviceFar = false;
      }

      if (myCharacteristic.written()) {
        String value = myCharacteristic.value();
        Serial.print("Received: ");
        Serial.println(value);
        myCharacteristic.writeValue("Received: " + value);
      }
    }
  } else if (wasConnected) {
    wasConnected = false;
    wasDisconnected = true;
    Serial.println("Device disconnected!");
    sendEvents(false); 
    activateBuzzer();
  }
}

void activateBuzzer() {
  digitalWrite(pinBuzzer, HIGH);
  delay(5000);
  digitalWrite(pinBuzzer, LOW);
  isBuzzerActive = true;
}
