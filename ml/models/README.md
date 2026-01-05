# INM ML Models

This directory contains trained ML artifacts for the Intelligent Nutrition Management (INM) module.

## Files

| File | Description |
|------|-------------|
| `inm_ec_rf_model.pkl` | Random Forest model for EC prediction (24h ahead) |
| `inm_ec_scaler.pkl` | Feature scaler used during training |

## Usage

```python
import joblib

# Load model and scaler
model = joblib.load('inm_ec_rf_model.pkl')
scaler = joblib.load('inm_ec_scaler.pkl')

# Prepare features and predict
features_scaled = scaler.transform(features)
predicted_ec = model.predict(features_scaled)
```

## Note

These models are used by the common backend (`smartrose_backend`) for EC predictions.
The backend loads these artifacts at startup for inference.

