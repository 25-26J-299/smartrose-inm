/*
 * SmartRose INM - Intelligent Nutrition Management
 * Main firmware for nutrient monitoring and control
 */

// Pin definitions
#define SENSOR_PIN A0
#define PUMP_PIN 2

// Global variables
float nutrientLevel = 0.0;

void setup() {
  Serial.begin(115200);
  pinMode(PUMP_PIN, OUTPUT);
  
  Serial.println("SmartRose INM Initialized");
}

void loop() {
  // Read sensor data
  nutrientLevel = analogRead(SENSOR_PIN);
  
  // Process and send data
  Serial.print("Nutrient Level: ");
  Serial.println(nutrientLevel);
  
  delay(1000);
}

