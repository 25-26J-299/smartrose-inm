from pydantic import BaseModel
from datetime import datetime
from typing import Optional


class INMSensorData(BaseModel):
    device_id: str

    soil_moisture: float
    soil_temp: float
    ec: int
    ph: float
    N: int
    P: int
    K: int

    air_temp: float
    air_hum: float

    timestamp: Optional[datetime] = None

