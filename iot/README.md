# IoT - SmartRose INM

## Overview
ESP32-based firmware for the Intelligent Nutrition Management system. Reads real sensors and POSTs data to the backend API.

## Hardware Requirements

### Microcontroller
- **ESP32 DevKit** (recommended: ESP32-WROOM-32)

### Sensors
| Sensor | Type | Interface | Purpose |
|--------|------|-----------|---------|
| NPK Sensor | RS485 Modbus | UART | Nitrogen, Phosphorus, Potassium |
| pH Sensor | Analog | ADC | Soil acidity |
| EC Sensor | Analog | ADC | Electrical Conductivity |
| Soil Moisture | Capacitive | ADC | Soil water content |
| DS18B20 | Digital | OneWire | Soil temperature |
| DHT22 | Digital | GPIO | Air temperature & humidity |

## Wiring Diagram

```
ESP32 Pin Connections:
─────────────────────────────────────────────────────
│ ESP32 Pin │ Sensor              │ Wire Color    │
─────────────────────────────────────────────────────
│ GPIO 34   │ Soil Moisture (OUT) │ Yellow        │
│ GPIO 35   │ pH Sensor (OUT)     │ Yellow        │
│ GPIO 32   │ EC Sensor (OUT)     │ Yellow        │
│ GPIO 4    │ DHT22 (DATA)        │ Yellow        │
│ GPIO 5    │ DS18B20 (DATA)      │ Yellow        │
│ GPIO 16   │ NPK RS485 (RX)      │ Green         │
│ GPIO 17   │ NPK RS485 (TX)      │ Blue          │
│ GPIO 18   │ NPK RS485 (DE/RE)   │ White         │
│ 3.3V      │ Sensor VCC          │ Red           │
│ GND       │ Sensor GND          │ Black         │
─────────────────────────────────────────────────────
```

## Configuration

Before uploading, update these values in `SmartRose_INM.ino`:

```cpp
// WiFi credentials
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// Backend API endpoint
const char* SERVER_URL = "http://YOUR_SERVER_IP:8000/api/v1/inm/sensor-data";

// Device ID — MUST be unique per physical board
const char* DEVICE_ID = "INM-001";
```

### Device ID rules (important)

| Rule | Detail |
|------|--------|
| **Unique per board** | Every physical ESP32 must have a different `DEVICE_ID`. Never flash the same ID onto two boards — their sensor data will mix in the database and ownership will break. |
| **Must match admin registration** | When the admin registers this device in the SmartRose admin dashboard, they must use the exact same string as `device_serial_number`. |
| **Stable** | Never change the `DEVICE_ID` of a device that is already deployed and assigned to a user/greenhouse. If you change it you must re-register it in admin. |
| **Naming convention** | Use the format `INM-XXX` (e.g. `INM-001`, `INM-002`, `INM-003`). Label the physical device/box with the same string. |

### Provisioning a new INM device (step-by-step)

1. Pick the next available ID: `INM-001`, `INM-002`, etc.
2. Set `DEVICE_ID = "INM-00X"` in `SmartRose_INM.ino`.
3. Flash the firmware onto the ESP32.
4. Stick a label `INM-00X` on the physical device and/or box.
5. Ask the admin to register this device in the SmartRose admin dashboard:
   - `device_serial_number = INM-00X`
   - `type = INM`
   - assign to the correct user and greenhouse.
6. Power the device — it will start posting data automatically.

## Required Libraries

Install these in Arduino IDE (Sketch → Include Library → Manage Libraries):

1. **ArduinoJson** by Benoit Blanchon
2. **DHT sensor library** by Adafruit
3. **OneWire** by Paul Stoffregen
4. **DallasTemperature** by Miles Burton

## Upload Instructions

1. Open Arduino IDE
2. Go to **File → Preferences**
3. Add ESP32 board URL: `https://dl.espressif.com/dl/package_esp32_index.json`
4. Go to **Tools → Board → Boards Manager** → Install "ESP32"
5. Select **Tools → Board → ESP32 Dev Module**
6. Select your COM port
7. Click **Upload**

## Files

| File | Description |
|------|-------------|
| `SmartRose_INM.ino` | Main firmware - reads sensors & POSTs to backend |
| `Sensor_Test.ino` | Test sketch for debugging individual sensors |
| `dummy_post.py` | Python simulator for testing without hardware |

## Sensor Calibration

### Soil Moisture
```cpp
#define SOIL_DRY_VALUE     4095  // ADC reading in dry air
#define SOIL_WET_VALUE     1500  // ADC reading in water
```
Adjust these values based on your sensor by testing in dry and wet conditions.

### pH Sensor
```cpp
#define PH_OFFSET          0.0   // Adjust after buffer calibration
#define PH_SLOPE           3.5   // mV per pH unit
```
Calibrate using pH 4.0 and pH 7.0 buffer solutions.

### EC Sensor
```cpp
#define EC_COEFFICIENT     1.0   // Adjust based on calibration solution
```

## Data Format (JSON Payload)

```json
{
  "device_id": "esp32_001",
  "soil_moisture": 45.2,
  "soil_temp": 22.5,
  "ec": 520,
  "ph": 6.8,
  "N": 120,
  "P": 85,
  "K": 200,
  "air_temp": 28.3,
  "air_hum": 65.0
}
```

## Troubleshooting

| Issue | Solution |
|-------|----------|
| WiFi won't connect | Check SSID/password, ensure 2.4GHz network |
| NPK reads -1 | Check RS485 wiring, verify baud rate (9600) |
| pH reads wrong | Recalibrate with buffer solutions |
| Soil temp -999 | Check DS18B20 wiring, add 4.7kΩ pull-up resistor |
| DHT22 fails | Add 10kΩ pull-up on DATA pin |

## Testing Without Hardware

Use the Python simulator:
```bash
cd iot
pip install requests
python dummy_post.py
```
