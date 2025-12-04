# Architecture Diagram - SmartRose INM

## System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                     SmartRose INM System                        │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────────────┐ │
│  │   Sensors   │───▶│   Arduino   │───▶│   Data Collection   │ │
│  │  (Nutrient) │    │  (IoT Hub)  │    │     (Python)        │ │
│  └─────────────┘    └──────┬──────┘    └──────────┬──────────┘ │
│                            │                      │             │
│                            │                      ▼             │
│                            │           ┌─────────────────────┐ │
│                            │           │   Data Processing   │ │
│                            │           │   & Exploration     │ │
│                            │           └──────────┬──────────┘ │
│                            │                      │             │
│                            │                      ▼             │
│                            │           ┌─────────────────────┐ │
│                            │           │    ML Model         │ │
│                            │           │  (Random Forest)    │ │
│                            │           └──────────┬──────────┘ │
│                            │                      │             │
│                            ▼                      ▼             │
│                   ┌─────────────┐      ┌─────────────────────┐ │
│                   │  Actuators  │◀─────│    Predictions &    │ │
│                   │   (Pumps)   │      │   Recommendations   │ │
│                   └─────────────┘      └─────────────────────┘ │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

## Component Interaction

```
┌──────────┐     Serial/WiFi     ┌──────────┐
│   IoT    │ ◀───────────────▶  │    ML    │
│  Layer   │                     │  Layer   │
└──────────┘                     └──────────┘
     │                                │
     │                                │
     ▼                                ▼
┌──────────┐                    ┌──────────┐
│ Physical │                    │  Model   │
│ Sensors  │                    │  Store   │
└──────────┘                    └──────────┘
```

## Data Pipeline

```
Raw Data ──▶ Preprocessing ──▶ Feature Engineering ──▶ Model Training
                                                              │
                                                              ▼
Real-time Data ──▶ Inference ◀───────────────────── Trained Model
                       │
                       ▼
              Nutrient Recommendations
```

