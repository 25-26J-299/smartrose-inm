# API Contract - SmartRose INM

## Sensor Data Format

### Input (from IoT)
```json
{
  "timestamp": "2024-01-01T12:00:00Z",
  "device_id": "smartrose-001",
  "sensors": {
    "nutrient_level": 0.75,
    "ph": 6.5,
    "temperature": 25.0,
    "humidity": 60.0
  }
}
```

### Prediction Response
```json
{
  "timestamp": "2024-01-01T12:00:01Z",
  "device_id": "smartrose-001",
  "predictions": {
    "nitrogen": 0.82,
    "phosphorus": 0.65,
    "potassium": 0.73
  },
  "recommendations": {
    "action": "increase_nitrogen",
    "dosage_ml": 15
  }
}
```

## Serial Protocol

### Commands
| Command | Description | Response |
|---------|-------------|----------|
| `READ`  | Read sensor values | JSON sensor data |
| `DOSE`  | Trigger nutrient dosing | `OK` or `ERROR` |
| `STATUS`| Get system status | JSON status object |

## Error Codes
| Code | Description |
|------|-------------|
| E001 | Sensor read failure |
| E002 | Communication timeout |
| E003 | Invalid command |







