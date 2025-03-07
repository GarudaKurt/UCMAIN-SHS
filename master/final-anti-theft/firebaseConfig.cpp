#include "firebaseConfig.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <FirebaseESP32.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <Arduino.h>

#define WIFI_SSID "il2wrk_main"
#define WIFI_PASSWORD "il2wrk2025"
#define API_KEY "AIzaSyBjhAuT0ozWZRXIEYXg3x97f6vAHJaKU5c"
#define DATABASE_URL "https://ucb-smart-irrigation-default-rtdb.firebaseio.com/"
#define USER_EMAIL "test@gmail.com"
#define USER_PASSWORD "test123"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void initFirebase() {
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi connection failed!");
    while(1);
  } 
  Serial.println("WiFi connection success!");
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;

  Firebase.reconnectWiFi(true);
  fbdo.setBSSLBufferSize(4096, 1024);
  Firebase.begin(&config, &auth);
  Firebase.setDoubleDigits(5);
  Serial.println("Successfully initialize firebase connection...");
}

void sendEvents(bool activate) {
  if (Firebase.ready()) {
    Serial.println("[DEBUG] Sending event to Firebase...");
    if (!Firebase.setBool(fbdo, "monitoring/connected", activate)) {
      Serial.println("Failed to send data to Firebase!");
    } else {
      Serial.println("Successfully sent event to Firebase!");
    }

  } else {
    Serial.println("Firebase not ready to send events!");
  }
}

bool readEvents() {
  // Non-blocking: Check Firebase every 2 seconds

  if (Firebase.ready()) {
    String value;
    bool success = Firebase.getString(fbdo, "monitoring/buzzer");
    if (success) {  
      value = fbdo.stringData();
      if (value == "ON") {
        // Update Firebase to turn off the buzzer flag
        if (!Firebase.setString(fbdo, "monitoring/buzzer", "OFF")) {
          Serial.println("Failed to update Firebase to OFF state!");
        }
        return true;
      }
    } else {
      Serial.println("Failed to read data from Firebase!");
    }
  } else {
    Serial.println("Firebase not ready for reading events!");
  }
  return false;
}
