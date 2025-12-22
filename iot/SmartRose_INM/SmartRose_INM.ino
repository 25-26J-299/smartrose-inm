/****************************************************
   NPK 7-in-1 + SHT3x + SD backup + WiFi upload
   ESP32 DevKit V1
   FINAL VERSION (SD FIX + SERIAL PRINTS + REAL TIME)
*****************************************************/

#include <ModbusMaster.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>

// ---------- WiFi ----------
const char* WIFI_SSID = "SLT_FIBRE188";
const char* WIFI_PASSWORD = "20021226";

// ⚠️ Use PC IP, NOT localhost
const char* SERVER_URL = "http://192.168.1.4:8000/api/v1/inm/sensor-data";

// ---------- NTP Time (Sri Lanka UTC +5:30) ----------
const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = 5 * 3600 + 30 * 60;
const int DAYLIGHT_OFFSET_SEC = 0;

// ---------- Modbus / RS485 ----------
ModbusMaster node;
#define RXD2 16
#define TXD2 17
#define RS485_EN 4

// ---------- SHT31 ----------
Adafruit_SHT31 sht31 = Adafruit_SHT31();

// ---------- SD ----------
#define SD_CS_PIN 5
const char* SD_FILENAME = "/sensor_log.csv";

// ---------- RS485 control ----------
void preTransmission() { digitalWrite(RS485_EN, HIGH); }
void postTransmission() { digitalWrite(RS485_EN, LOW); }

// ---------- Get real date & time ----------
String getCurrentDateTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "1970-01-01 00:00:00";
  }
  char buffer[25];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buffer);
}

// ---------- SD ----------
bool sdInit() {
  Serial.print("Initializing SD...");
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println(" failed!");
    return false;
  }
  Serial.println(" ok");
  return true;
}

// ✅ FIXED: re-mount SD before every write
void appendToSD(String line) {
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("⚠️ SD card not available");
    return;
  }

  File file = SD.open(SD_FILENAME, FILE_APPEND);
  if (!file) {
    Serial.println("⚠️ Failed to open log file");
    return;
  }

  file.println(line);
  file.close();
  Serial.println("💾 SD Log Saved");
}

// ---------- Send JSON to backend ----------
void sendToBackend(String json) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("🌐 WiFi: DISCONNECTED → Upload skipped");
    return;
  }

  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(json);

  if (httpCode > 0) {
    Serial.print("☁️ Backend Upload: SUCCESS (HTTP ");
    Serial.print(httpCode);
    Serial.println(")");
  } else {
    Serial.println("❌ Backend Upload: FAILED");
  }

  http.end();
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(RS485_EN, OUTPUT);
  digitalWrite(RS485_EN, LOW);

  Serial2.begin(4800, SERIAL_8N1, RXD2, TXD2);
  node.begin(1, Serial2);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  Wire.begin(21, 22);
  sht31.begin(0x44);

  sdInit();

  if (!SD.exists(SD_FILENAME)) {
    File f = SD.open(SD_FILENAME, FILE_WRITE);
    f.println("datetime,soil_moisture,soil_temp,ec,ph,N,P,K,air_temp,air_hum");
    f.close();
  }

  // ---------- WiFi ----------
  Serial.print("📡 Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n📶 WiFi connected");

  // ---------- NTP ----------
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
  Serial.print("🕒 Syncing time");
  while (getCurrentDateTime().startsWith("1970")) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n🕒 Time synchronized");
}

// ---------- Loop ----------
void loop() {
  uint8_t result = node.readHoldingRegisters(0x0000, 7);

  float moisture = -1, soilTemp = -1, ph = -1;
  int ec = -1, N = -1, P = -1, K = -1;

  if (result == node.ku8MBSuccess) {
    moisture = node.getResponseBuffer(0) / 10.0;
    soilTemp = node.getResponseBuffer(1) / 10.0;
    ec       = node.getResponseBuffer(2);
    ph       = node.getResponseBuffer(3) / 10.0;
    N        = node.getResponseBuffer(4);
    P        = node.getResponseBuffer(5);
    K        = node.getResponseBuffer(6);
  } else {
    Serial.println("⚠️ NPK Modbus Read Error");
  }

  float airTemp = sht31.readTemperature();
  float airHum  = sht31.readHumidity();

  String timestamp = getCurrentDateTime();

  // ---------- SERIAL OUTPUT ----------
  Serial.println("\n=================================");
  Serial.print("📅 Date & Time      : "); Serial.println(timestamp);

  Serial.println("🌱 Soil Sensor (NPK 7-in-1)");
  Serial.print("  Moisture (%)     : "); Serial.println(moisture);
  Serial.print("  Soil Temp (°C)   : "); Serial.println(soilTemp);
  Serial.print("  EC               : "); Serial.println(ec);
  Serial.print("  pH               : "); Serial.println(ph);
  Serial.print("  Nitrogen (N)     : "); Serial.println(N);
  Serial.print("  Phosphorus (P)   : "); Serial.println(P);
  Serial.print("  Potassium (K)    : "); Serial.println(K);

  Serial.println("🌡️ Air Sensor (SHT31)");
  Serial.print("  Air Temp (°C)    : "); Serial.println(airTemp);
  Serial.print("  Air Humidity (%) : "); Serial.println(airHum);

  // ---------- CSV ----------
  String csv = timestamp + "," +
               moisture + "," + soilTemp + "," + ec + "," + ph + "," +
               N + "," + P + "," + K + "," +
               airTemp + "," + airHum;

  delay(50);                 // SPI settle
  appendToSD(csv);           // robust SD write

  // ---------- JSON ----------
  String json = "{";
  json += "\"device_id\":\"esp32_001\",";
  json += "\"timestamp\":\"" + timestamp + "\",";
  json += "\"soil_moisture\":" + String(moisture) + ",";
  json += "\"soil_temp\":" + String(soilTemp) + ",";
  json += "\"ec\":" + String(ec) + ",";
  json += "\"ph\":" + String(ph) + ",";
  json += "\"N\":" + String(N) + ",";
  json += "\"P\":" + String(P) + ",";
  json += "\"K\":" + String(K) + ",";
  json += "\"air_temp\":" + String(airTemp) + ",";
  json += "\"air_hum\":" + String(airHum);
  json += "}";

  sendToBackend(json);

  delay(2000);
}