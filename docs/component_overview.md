# Component Overview - SmartRose INM

## System Components

### 1. IoT Layer
- **Hardware**: Arduino-based microcontroller
- **Sensors**: Nutrient sensors for monitoring plant nutrition levels
- **Actuators**: Pumps and dosing systems for nutrient delivery

### 2. Machine Learning Layer
- **Data Pipeline**: Collection, processing, and labeling of sensor data
- **Model**: Nutrient prediction model using Random Forest
- **Inference**: Real-time predictions for nutrient management

### 3. Communication
- **Protocol**: Serial communication / WiFi (ESP32)
- **Data Format**: JSON for sensor readings and predictions

## Data Flow
```
Sensors → Arduino → Data Collection → ML Model → Predictions → Actuators
```

## Dependencies
- Arduino IDE for firmware development
- Python 3.9+ for ML components
- scikit-learn for model training







