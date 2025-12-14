# File: smartrose-inm/iot/dummy_post.py
# Purpose: Simulate ESP32 INM sensor readings and POST to backend API

import requests
import random
from datetime import datetime
import time

# Set the URL of your INM sensor-data endpoint
API_URL = "http://localhost:8000/api/v1/inm/sensor-data"


def generate_dummy_reading():
    """
    Generate dummy ESP32 sensor reading matching the full payload:
    NPK + pH + EC + soil + air data
    """
    return {
        "device_id": "esp32_001",
        
        # Soil sensors
        "soil_moisture": round(random.uniform(30.0, 70.0), 1),
        "soil_temp": round(random.uniform(18.0, 28.0), 1),
        "ec": random.randint(200, 800),  # Electrical conductivity (µS/cm)
        "ph": round(random.uniform(5.5, 7.5), 2),
        
        # NPK sensors (mg/kg)
        "N": random.randint(50, 200),   # Nitrogen
        "P": random.randint(30, 150),   # Phosphorus
        "K": random.randint(100, 300),  # Potassium
        
        # Air sensors
        "air_temp": round(random.uniform(20.0, 35.0), 1),
        "air_hum": round(random.uniform(40.0, 80.0), 1),
    }


if __name__ == "__main__":
    print("SmartRose INM - Sensor Simulator Started")
    print(f"Sending data to: {API_URL}")
    print("-" * 50)
    
    while True:
        reading = generate_dummy_reading()
        try:
            response = requests.post(API_URL, json=reading)
            print(f"[{datetime.now().strftime('%H:%M:%S')}] Sent: {reading}")
            print(f"  → Status: {response.status_code}, Response: {response.json()}")
        except requests.exceptions.ConnectionError:
            print(f"[{datetime.now().strftime('%H:%M:%S')}] Error: Cannot connect to {API_URL}")
        except Exception as e:
            print(f"[{datetime.now().strftime('%H:%M:%S')}] Error: {e}")
        
        time.sleep(5)  # Send every 5 seconds
