/*
 * SmartRose INM - Sensor Test
 * Test sketch for validating sensor connections
 */

#define SENSOR_PIN A0

void setup() {
  Serial.begin(115200);
  Serial.println("Sensor Test Started");
}

void loop() {
  int rawValue = analogRead(SENSOR_PIN);
  float voltage = rawValue * (5.0 / 1023.0);
  
  Serial.print("Raw: ");
  Serial.print(rawValue);
  Serial.print(" | Voltage: ");
  Serial.println(voltage);
  
  delay(500);
}





