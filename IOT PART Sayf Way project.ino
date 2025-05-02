#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>

#define BLYNK_TEMPLATE_ID "TMPL2Gr3qwDb7"
#define BLYNK_TEMPLATE_NAME "SafeWAY"
#define BLYNK_AUTH_TOKEN "eoEpuHJF2CoI0rstb28SerwkQsmHetxI"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// LCD I2C
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // Adresse I2C 0x27 (changer si nécessaire)

// Wi-Fi credentials
const char* ssid = "ORANGE_8778";
const char* password = "74MvbuiG";

// Rain sensor pin
#define RAIN_SENSOR_PIN 34

// DHT22 sensor pin and setup
#define DHTPIN 33
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Variables to track rain detection
bool isRaining = false;
bool lastRainState = false;

// Visibility threshold
#define VISIBILITY_THRESHOLD 1000

void setup() {
  Serial.begin(115200);

  // LCD Init
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("SafeWAY Init...");
  
  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
  lcd.setCursor(0, 1);
  lcd.print("WiFi Connected");
  delay(1500);
  lcd.clear();

  // Start Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  // Initialize DHT sensor
  dht.begin();
  Serial.println("DHT22 sensor initialized.");
  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  delay(1500);
  lcd.clear();
}

void loop() {
  Blynk.run();

  // Read rain sensor value
  int rainValue = analogRead(RAIN_SENSOR_PIN);
  isRaining = (rainValue < 2000); // Adjust this value for your specific sensor

  Serial.print("Rain Value: ");
  Serial.print(rainValue);
  Serial.print(" | Is Raining: ");
  Serial.println(isRaining ? "Yes" : "No");

  // Read temperature and humidity from DHT22
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  int visibility = map(rainValue, 0, 4095, 0, 100);

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error");
  } else {
    // Serial output
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C | Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // LCD display
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(String(temperature, 1) + "C " + String(humidity, 0) + "%");
    lcd.setCursor(0, 1);
    lcd.print("Visi:" + String(visibility) + "% ");
    lcd.print(isRaining ? "Rain" : "Clear");

    // Blynk
    Blynk.virtualWrite(V0, temperature);
    Blynk.virtualWrite(V1, humidity);
  }

  // Send visibility value to Blynk
  Blynk.virtualWrite(V2, visibility);

  // Log event if rain state changes
  if (isRaining != lastRainState) {
    if (isRaining) {
      Blynk.logEvent("rain_alarm", "Rain detected! Take action.");
      Serial.println("Rain detected! Notification sent.");
    } else {
      Serial.println("Rain stopped.");
    }
    lastRainState = isRaining;
  }

  // Send rain state to Blynk
  Blynk.virtualWrite(V3, isRaining ? 1 : 0);

  delay(2000);
}
