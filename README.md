# SmartRose INM - Intelligent Nutrition Management

An IoT-powered intelligent nutrition management system for precision agriculture and plant care.

## Overview

SmartRose INM combines IoT sensors with machine learning to monitor and optimize plant nutrition automatically. The system collects real-time nutrient data, predicts deficiencies, and provides actionable recommendations.

## Project Structure

```
smartrose-inm/
├── iot/                          # Arduino firmware
│   ├── SmartRose_INM.ino         # Main firmware
│   ├── Sensor_Test.ino           # Sensor testing
│   └── README.md
├── ml/                           # Machine learning
│   ├── data/
│   │   ├── raw/                  # Raw sensor data
│   │   ├── processed/            # Cleaned data
│   │   └── labels/               # Training labels
│   ├── notebooks/                # Jupyter notebooks
│   │   ├── 01_data_collection.ipynb
│   │   ├── 02_data_exploration.ipynb
│   │   ├── 03_train_model.ipynb
│   │   └── 04_evaluate_model.ipynb
│   ├── models/                   # Trained models
│   │   └── nutrient_model.pkl
│   ├── scripts/                  # Training & inference
│   │   ├── train.py
│   │   └── predict.py
│   └── README.md
├── docs/                         # Documentation
│   ├── component_overview.md
│   ├── api_contract.md
│   └── architecture_diagram.md
├── .gitignore
└── README.md
```

## Getting Started

### Prerequisites
- Arduino IDE (for IoT components)
- Python 3.9+
- Required Python packages: `pandas`, `numpy`, `scikit-learn`, `matplotlib`, `jupyter`

### Installation

1. Clone the repository:
```bash
git clone https://github.com/your-username/smartrose-inm.git
cd smartrose-inm
```

2. Set up Python environment:
```bash
python -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate
pip install pandas numpy scikit-learn matplotlib seaborn joblib jupyter
```

3. Upload Arduino firmware:
   - Open `iot/SmartRose_INM.ino` in Arduino IDE
   - Select your board and port
   - Upload the sketch

## Usage

### Data Collection
Run the data collection notebook or script to gather sensor readings.

### Model Training
```bash
cd ml/scripts
python train.py
```

### Making Predictions
```bash
python predict.py --input sensor_data.csv --output predictions.csv
```

## Documentation
- [Component Overview](docs/component_overview.md)
- [API Contract](docs/api_contract.md)
- [Architecture Diagram](docs/architecture_diagram.md)

## License
MIT License

## Contributing
Contributions are welcome! Please read our contributing guidelines before submitting PRs.
