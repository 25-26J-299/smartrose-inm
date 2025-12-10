# File: smartrose-inm/iot/dummy_post.py
# Purpose: Simulate INM sensor readings and POST to common backend API

import requests
import random
from datetime import datetime
import time

# 1️⃣ Set the URL of your common backend INM sensor-data endpoint
API_URL = "http://localhost:8000/api/v1/inm/sensor-data"

# 2️⃣ Function to generate a dummy sensor reading
def generate_dummy_reading():
    return {
        "sensor_id": "sensor_001",
        "temperature": round(random.uniform(20.0, 30.0), 1),
        "humidity": round(random.uniform(40.0, 70.0), 1),
        "soil_moisture": round(random.uniform(30.0, 60.0), 1),
        "timestamp": datetime.utcnow().isoformat()
    }

# 3️⃣ Main loop to send readings continuously
if __name__ == "__main__":
    while True:
        reading = generate_dummy_reading()
        try:
            response = requests.post(API_URL, json=reading)
            print(f"Sent: {reading} → Status: {response.status_code}, Response: {response.json()}")
        except Exception as e:
            print(f"Error sending data: {e}")
        time.sleep(5)  # send every 5 seconds



