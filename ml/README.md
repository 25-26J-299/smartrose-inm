# ML - SmartRose INM

## Overview
Machine learning components for nutrient prediction and analysis.

## Directory Structure
```
ml/
├── data/
│   ├── raw/          # Raw sensor data
│   ├── processed/    # Cleaned and processed data
│   └── labels/       # Labeled training data
├── notebooks/
│   ├── 01_data_collection.ipynb
│   ├── 02_data_exploration.ipynb
│   ├── 03_train_model.ipynb
│   └── 04_evaluate_model.ipynb
├── models/
│   └── nutrient_model.pkl
└── scripts/
    ├── train.py
    └── predict.py
```

## Setup
```bash
pip install pandas numpy scikit-learn matplotlib seaborn joblib jupyter
```

## Usage

### Training
```bash
cd scripts
python train.py
```

### Prediction
```bash
cd scripts
python predict.py --input data.csv --output predictions.csv
```

## Notebooks
Run notebooks in order for the complete ML pipeline:
1. Data Collection
2. Data Exploration
3. Model Training
4. Model Evaluation





