#include <Wire.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <FirebaseESP32.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define WIFI_SSID "il2wrk"
#define WIFI_PASSWORD "il2wrk2025"
#define API_KEY "AIzaSyBjhAuT0ozWZRXIEYXg3x97f6vAHJaKU5c"
#define DATABASE_URL "https://ucb-smart-irrigation-default-rtdb.firebaseio.com/"
#define USER_EMAIL "test@gmail.com"
#define USER_PASSWORD "test123"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

unsigned long prevMillis = 0;
const int buttonPin = 15;
bool buttonPressed = false;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void initFirebase() {
    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
    
    config.api_key = API_KEY;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    config.database_url = DATABASE_URL;
    config.token_status_callback = tokenStatusCallback;

    Firebase.reconnectWiFi(true);
    fbdo.setBSSLBufferSize(4096, 1024);
    Firebase.begin(&config, &auth);
    Firebase.setDoubleDigits(5);
}

void readSensor() {
    
  if (Firebase.ready()) {
    Serial.println("[DEBUG] pass here!");

    String latitude, longitude, city, region, cntr;

    bool success = Firebase.getString(fbdo, "monitoring/latitude") &&
                   Firebase.getString(fbdo, "monitoring/longitude") &&
                   Firebase.getString(fbdo, "monitoring/city") &&
                   Firebase.getString(fbdo, "monitoring/region") &&
                   Firebase.getString(fbdo, "monitoring/cntr");
    if (success) {
      Firebase.getString(fbdo, "monitoring/latitude");
      latitude = fbdo.stringData();
      Firebase.getString(fbdo, "monitoring/longitude");
      longitude = fbdo.stringData();
      Firebase.getString(fbdo, "monitoring/city");
      city = fbdo.stringData();
      Firebase.getString(fbdo, "monitoring/region");
      region = fbdo.stringData();
      Firebase.getString(fbdo, "monitoring/cntr");
      cntr = fbdo.stringData().toInt();

      Serial.printf("Received from Firebase: Lat: %s | Long: %s | City: %s | Region: %s | Cntr: %s.\n",
          latitude.c_str(), longitude.c_str(), city.c_str(), region.c_str(), cntr.c_str());

      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Tracking Device:");
      display.println("Lat: " + latitude);
      display.println("Long: " + longitude);
      display.println("City: " + city);
      display.println("Reg: " + region);
      display.println("Cntr: " + cntr);
      display.display();

    } else {
      Serial.println("Failed to retrieve data from Firebase!");
      display.clearDisplay();
      display.setCursor(0, 10);
      display.println("Firebase Error!");
      display.display();
    }
  }

}

void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22); // SDA, SCL for ESP32
  SerialBT.begin("SlaveDevice");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println("Connecting to WiFi...");
  display.display();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi connection failed!");
    return;
  }
  Serial.printf("WiFi Connected | IP: %s\n", WiFi.localIP().toString().c_str());
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("WiFi Connected!");
  display.display();

  pinMode(buttonPin, INPUT_PULLUP);
  initFirebase();
}


void loop() {
  if (millis() - prevMillis > 1500) {
    prevMillis = millis();
    readSensor();
  }
  
  if(millis() - prevMillis > 1000) {
    prevMillis = millis();
    SerialBT.println("A");
  }

  if (digitalRead(buttonPin) == LOW && !buttonPressed) {
    buttonPressed = true;
    SerialBT.println("ALARM");
    Serial.println("ALARM Sent!");
    delay(500);
  } else if (digitalRead(buttonPin) == HIGH) {
    buttonPressed = false;
  }
}
