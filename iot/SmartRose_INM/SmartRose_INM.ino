/****************************************************
   NPK 7-in-1 + SHT3x + SD Store-and-Forward + Archive
   ESP32 DevKit V1
   FINAL PRODUCTION VERSION (FIXED & SAFE)
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
const char* WIFI_SSID     = "SLT_FIBRE188";
const char* WIFI_PASSWORD = "20021226";
const char* SERVER_URL    = "http://192.168.1.2:8000/api/v1/inm/sensor-data";

// ---------- Device Identity ----------
// IMPORTANT: Each physical INM ESP32 board must have a UNIQUE device_id.
// This value MUST match the device_serial_number registered by the admin
// in the SmartRose admin dashboard for this device.
// When flashing a new board, change this to the next ID (e.g. INM-002, INM-003).
// Never use the same ID on two boards — their data will mix in the database.

//const char* DEVICE_ID = "INM-001";
const char* DEVICE_ID = "esp32_001";

// ---------- Time ----------
const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = 5 * 3600 + 30 * 60;

// ---------- RS485 ----------
ModbusMaster node;
#define RXD2 16
#define TXD2 17
#define RS485_EN 4

// ---------- SHT31 ----------
Adafruit_SHT31 sht31 = Adafruit_SHT31();

// ---------- SD ----------
#define SD_CS_PIN 5
const char* SD_PENDING = "/pending.csv";
const char* SD_ARCHIVE = "/archive.csv";
const char* SD_TEMP    = "/temp.csv";

// ---------- RS485 ----------
void preTransmission()  { digitalWrite(RS485_EN, HIGH); }
void postTransmission() { digitalWrite(RS485_EN, LOW); }

// ---------- Time (ISO 8601 FIX) ----------
String getCurrentDateTime() {
  struct tm t;
  if (!getLocalTime(&t)) return "1970-01-01T00:00:00";
  char buf[25];
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &t); // ✅ FIX
  return String(buf);
}

// ---------- CSV → JSON ----------
String csvToJson(String csv) {
  String v[10];
  int i = 0;

  while (csv.length() && i < 10) {
    int p = csv.indexOf(',');
    if (p == -1) { v[i++] = csv; break; }
    v[i++] = csv.substring(0, p);
    csv = csv.substring(p + 1);
  }

  return "{"
    "\"device_id\":\"" + String(DEVICE_ID) + "\","
    "\"timestamp\":\"" + v[0] + "\","
    "\"soil_moisture\":" + v[1] + ","
    "\"soil_temp\":" + v[2] + ","
    "\"ec\":" + v[3] + ","
    "\"ph\":" + v[4] + ","
    "\"N\":" + v[5] + ","
    "\"P\":" + v[6] + ","
    "\"K\":" + v[7] + ","
    "\"air_temp\":" + v[8] + ","
    "\"air_hum\":" + v[9] +
  "}";
}

// ---------- Upload ONE record ----------
bool uploadCsvLine(const String& csv) {
  if (WiFi.status() != WL_CONNECTED) return false;

  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/json");

  int code = http.POST(csvToJson(csv));
  http.end();

  Serial.print("🌐 HTTP code: ");
  Serial.println(code);

  return (code == 200 || code == 201);
}

// ---------- Upload OLD data ----------
void sendPendingFromSD() {
  if (WiFi.status() != WL_CONNECTED) return;

  File in = SD.open(SD_PENDING, FILE_READ);
  if (!in) return;

  File out = SD.open(SD_TEMP, FILE_WRITE);
  if (!out) {
    in.close();
    return;
  }

  // Copy header
  String header = in.readStringUntil('\n');
  out.println(header);

  bool anyUploaded = false;

  while (in.available()) {
    String line = in.readStringUntil('\n');
    line.trim();

    if (line.length() == 0 || line.startsWith("datetime")) continue;

    Serial.print("⬆️ [OLD] Uploading → ");
    Serial.println(line);

    if (uploadCsvLine(line)) {
      Serial.println("☁️ [OLD] Uploaded OK");
      anyUploaded = true;
    } else {
      Serial.println("⚠️ [OLD] Failed → keeping");
      out.println(line);
      break; // stop on first failure
    }

    delay(150);
  }

  in.close();
  out.close();

  if (anyUploaded) {
    SD.remove(SD_PENDING);
    SD.rename(SD_TEMP, SD_PENDING);
    Serial.println("🗂️ pending.csv updated");
  } else {
    SD.remove(SD_TEMP);
  }
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

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("❌ SD init failed");
    return;
  }
  Serial.println("✅ SD ready");

  if (!SD.exists(SD_PENDING)) {
    File f = SD.open(SD_PENDING, FILE_WRITE);
    f.println("datetime,soil_moisture,soil_temp,ec,ph,N,P,K,air_temp,air_hum");
    f.close();
  }

  if (!SD.exists(SD_ARCHIVE)) {
    File f = SD.open(SD_ARCHIVE, FILE_WRITE);
    f.println("datetime,soil_moisture,soil_temp,ec,ph,N,P,K,air_temp,air_hum");
    f.close();
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("📡 Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n📶 WiFi Connected");

  configTime(GMT_OFFSET_SEC, 0, NTP_SERVER);
}

// ---------- Loop ----------
void loop() {
  node.readHoldingRegisters(0x0000, 7);

  float moisture = node.getResponseBuffer(0) / 10.0;
  float soilTemp = node.getResponseBuffer(1) / 10.0;
  int ec         = node.getResponseBuffer(2);
  float ph       = node.getResponseBuffer(3) / 10.0;
  int N          = node.getResponseBuffer(4);
  int P          = node.getResponseBuffer(5);
  int K          = node.getResponseBuffer(6);

  float airTemp = sht31.readTemperature();
  float airHum  = sht31.readHumidity();


  String ts = getCurrentDateTime();
  String csv = ts + "," + moisture + "," + soilTemp + "," + ec + "," +
               ph + "," + N + "," + P + "," + K + "," +
               airTemp + "," + airHum;

  Serial.print("📊 [NEW] ");
  Serial.println(csv);

  // ---- ARCHIVE ALWAYS ----
  File archive = SD.open(SD_ARCHIVE, FILE_APPEND);
  archive.println(csv);
  archive.close();

  // ---- TRY IMMEDIATE UPLOAD ----
  if (!uploadCsvLine(csv)) {
    File pending = SD.open(SD_PENDING, FILE_APPEND);
    pending.println(csv);
    pending.close();
    Serial.println("💾 [NEW] Saved to pending");
  } else {
    Serial.println("🚀 [NEW] Uploaded immediately");
  }

  // ---- RETRY OLD ----
  sendPendingFromSD();

  delay(120000); // adjust as needed

  //delay(300000);   // 5 minutes
  //delay(120000);   // 2 minutes
  //delay(2000)  //2 seconds
}
