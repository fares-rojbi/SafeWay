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
const char* password = "5060708090                      ";    // Your WiFi Password

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

// Initialize the LCD display with I2C address 0x27, 16 columns, and 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2); // Use the LiquidCrystal_I2C library

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize the LCD
  lcd.init(); // Use init() instead of begin()
  lcd.backlight(); // Turn on the backlight
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");

  // Print connecting message  
  Serial.println("Connecting to Wi-Fi...");
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");

  // Update LCD with connection status
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wi-Fi Connected!");

  // Configure Blynk
  Blynk.config(BLYNK_AUTH_TOKEN);

  // Optionally, connect to the Blynk server
  if (Blynk.connect()) {
    Serial.println("Connected to Blynk!");
    lcd.setCursor(0, 1);
    lcd.print("Blynk Connected!");
  } else {
    Serial.println("Failed to connect to Blynk.");
    lcd.setCursor(0, 1);
    lcd.print("Blynk Failed!");
  }

  // Initialize DHT sensor
  dht.begin();
  Serial.println("DHT22 sensor initialized.");
}

void loop() {
  // Keep Blynk connected
  Blynk.run();

  // Read rain sensor value
  int rainValue = analogRead(RAIN_SENSOR_PIN); // Read analog value from rain sensor
  isRaining = (rainValue < 2000); // Adjust threshold based on your sensor

  // Debug: Print rain sensor value and state
  Serial.print("Rain Value: ");
  Serial.print(rainValue);
  Serial.print(" | Is Raining: ");
  Serial.println(isRaining ? "Yes" : "No");

  // Read temperature and humidity from DHT22
  float temperature = dht.readTemperature(); // Read temperature in Celsius
  float humidity = dht.readHumidity(); // Read humidity in percentage

  // Calculate visibility based on rain sensor value
  int visibility = map(rainValue, 0, 4095, 0, 100); // Map rain sensor value to visibility percentage (0-100%)

  // Check if DHT readings are valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    // Print temperature and humidity to Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C | Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Send temperature and humidity to Blynk
    Blynk.virtualWrite(V0, temperature); // Send temperature to virtual pin V0
    Blynk.virtualWrite(V1, humidity); // Send humidity to virtual pin V1
  }

  // Send visibility to Blynk
  Blynk.virtualWrite(V2, visibility); // Send visibility to virtual pin V2

  // Update LCD with speed limit based on conditions
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Speed Limit:");

  if (rainValue < VISIBILITY_THRESHOLD) {
    // Visibility is too low
    lcd.setCursor(0, 1);
    lcd.print("50 KM/H");
    Serial.println("Visibility Low: 50 KM/H");
  } else if (isRaining) {
    // It is raining
    lcd.setCursor(0, 1);
    lcd.print("80 KM/H");
    Serial.println("Raining: 80 KM/H");
  } else {
    // It is not raining
    lcd.setCursor(0, 1);
    lcd.print("120 KM/H");
    Serial.println("Not Raining: 120 KM/H");
  }

  // Check if rain state has changed
  if (isRaining != lastRainState) {
    if (isRaining) {
      // Send notification when rain is detected
      Blynk.logEvent("rain_alarm", "Rain detected! Take action.");
      Serial.println("Rain detected! Notification sent.");
    } else {
      Serial.println("Rain stopped.");
    }
    lastRainState = isRaining; // Update the last rain state
  }

  // Send rain status to Blynk app
  Blynk.virtualWrite(V3, isRaining ? 1 : 0); // Send rain status to virtual pin V3

  // Delay for stability (adjust as needed)
  delay(2000); // Check rain sensor every 2 seconds
}
