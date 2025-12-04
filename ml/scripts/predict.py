"""
SmartRose INM - Prediction Script
Makes nutrient predictions using the trained model.
"""

import pandas as pd
import numpy as np
import joblib
import argparse

# Paths
MODEL_PATH = "../models/nutrient_model.pkl"


def load_model(path: str):
    """Load trained model from disk."""
    return joblib.load(path)


def predict(model, features: np.ndarray) -> np.ndarray:
    """Make predictions using the loaded model."""
    return model.predict(features)


def main():
    parser = argparse.ArgumentParser(
        description="SmartRose INM Nutrient Prediction"
    )
    parser.add_argument(
        "--input",
        type=str,
        help="Path to input data file"
    )
    parser.add_argument(
        "--output",
        type=str,
        help="Path to save predictions"
    )
    args = parser.parse_args()

    # Load model
    model = load_model(MODEL_PATH)
    
    # TODO: Implement prediction pipeline
    print("Prediction complete")


if __name__ == "__main__":
    main()

