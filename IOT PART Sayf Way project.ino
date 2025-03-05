#define BLYNK_TEMPLATE_ID "TMPL2Gr3qwDb7"
#define BLYNK_TEMPLATE_NAME "SafeWAY"
#define BLYNK_AUTH_TOKEN "eoEpuHJF2CoI0rstb28SerwkQsmHetxI"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Include the LiquidCrystal_I2C library
#include <DHT.h> // Include the DHT library

// Replace with your Wi-Fi credentials
const char* ssid = "..";     // Your WiFi SSID
const char* password = "5060708090";    // Your WiFi Password

// Rain sensor pin
#define RAIN_SENSOR_PIN 32 // Analog pin connected to the rain sensor

// DHT22 sensor pin and setup
#define DHTPIN 0 // Digital pin connected to the DHT22
#define DHTTYPE DHT22 // DHT22 sensor type
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor

// Variables to track rain detection
bool isRaining = false;
bool lastRainState = false;

// Visibility threshold
#define VISIBILITY_THRESHOLD 1000 // Adjust based on your sensor

// Accident notification variables
bool hasAccident = false;
unsigned long accidentStartTime = 0;
const unsigned long ACCIDENT_DISPLAY_DURATION = 10000; // 10 seconds

// Initialize the LCD display with I2C address 0x27, 16 columns, and 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2); // Use the LiquidCrystal_I2C library

// Function declarations
void initializeSerial();
void initializeLCD();
void connectWiFi();
void connectBlynk();
void initializeSensors();
void readRainSensor();
void readDHTData();
void updateLCD();
void sendDataToBlynk();
void checkRainStateChange();
void displayAccidentMessage();
void checkAccidentStatus();

void setup() {
  initializeSerial();
  initializeLCD();
  connectWiFi();
  connectBlynk();
  initializeSensors();
}

void loop() {
  Blynk.run();
  
  readRainSensor();
  readDHTData();
  checkAccidentStatus();
  updateLCD();
  sendDataToBlynk();
  checkRainStateChange();
  
  delay(2000);
}

void initializeSerial() {
  Serial.begin(115200);
  Serial.println("Initializing SafeWAY system...");
}

void initializeLCD() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");
}

void connectWiFi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConnected to Wi-Fi!");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wi-Fi Connected!");
}

void connectBlynk() {
  Blynk.config(BLYNK_AUTH_TOKEN);
  
  if (Blynk.connect()) {
    Serial.println("Connected to Blynk!");
    lcd.setCursor(0, 1);
    lcd.print("Blynk Connected!");
    
    // Subscribe to accident notification from Blynk
    Blynk.virtualWrite(V4, 0); // Initialize accident status
  } else {
    Serial.println("Failed to connect to Blynk.");
    lcd.setCursor(0, 1);
    lcd.print("Blynk Failed!");
  }
}

void initializeSensors() {
  dht.begin();
  Serial.println("DHT22 sensor initialized.");
}

void readRainSensor() {
  int rainValue = analogRead(RAIN_SENSOR_PIN);
  isRaining = (rainValue < 2000);
  
  Serial.print("Rain Value: ");
  Serial.print(rainValue);
  Serial.print(" | Is Raining: ");
  Serial.println(isRaining ? "Yes" : "No");
}

void readDHTData() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C | Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
    
    Blynk.virtualWrite(V0, temperature);
    Blynk.virtualWrite(V1, humidity);
  }
}

void updateLCD() {
  if (hasAccident) {
    return; // Don't update LCD if showing accident message
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Speed Limit:");
  
  int rainValue = analogRead(RAIN_SENSOR_PIN);
  
  if (rainValue < VISIBILITY_THRESHOLD) {
    lcd.setCursor(0, 1);
    lcd.print("50 KM/H");
    Serial.println("Visibility Low: 50 KM/H");
  } else if (isRaining) {
    lcd.setCursor(0, 1);
    lcd.print("80 KM/H");
    Serial.println("Raining: 80 KM/H");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("120 KM/H");
    Serial.println("Not Raining: 120 KM/H");
  }
}

void sendDataToBlynk() {
  int rainValue = analogRead(RAIN_SENSOR_PIN);
  int visibility = map(rainValue, 0, 4095, 0, 100);
  
  Blynk.virtualWrite(V2, visibility);
  Blynk.virtualWrite(V3, isRaining ? 1 : 0);
}

void checkRainStateChange() {
  if (isRaining != lastRainState) {
    if (isRaining) {
      Blynk.logEvent("rain_alarm", "Rain detected! Take action.");
      Serial.println("Rain detected! Notification sent.");
    } else {
      Serial.println("Rain stopped.");
    }
    lastRainState = isRaining;
  }
}

void displayAccidentMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACCIDENT ALERT!");
  lcd.setCursor(0, 1);
  lcd.print("Please Slow Down!");
  
  // Send notification to Blynk
  Blynk.logEvent("accident_alert", "Accident detected! Please proceed with caution.");
  Serial.println("Accident alert displayed!");
}

void checkAccidentStatus() {
  // Check if accident status has changed
  if (Blynk.virtualRead(V4) == 1 && !hasAccident) {
    hasAccident = true;
    accidentStartTime = millis();
    displayAccidentMessage();
  }
  
  // Check if accident display duration has elapsed
  if (hasAccident && (millis() - accidentStartTime >= ACCIDENT_DISPLAY_DURATION)) {
    hasAccident = false;
    Blynk.virtualWrite(V4, 0); // Reset accident status
  }
}
