from fastapi import APIRouter, HTTPException
from datetime import datetime
from app.models.inm_models import INMSensorData
from app.database import get_collection

router = APIRouter(prefix="/api/v1/inm", tags=["INM"])


@router.post("/sensor-data", status_code=201)
async def ingest_sensor_data(payload: INMSensorData):
    """
    Ingest sensor data from ESP32 device.
    Accepts full NPK + pH + EC + soil + air data payload.
    Stores data in MongoDB.
    """
    data = payload.dict()
    
    # Force backend-controlled timestamp
    data["timestamp"] = datetime.utcnow()
    data["created_at"] = datetime.utcnow()
    
    # Store in MongoDB
    try:
        collection = get_collection("sensor_readings")
        result = await collection.insert_one(data)
        data["_id"] = str(result.inserted_id)
        
        print(f"✅ Saved to MongoDB: {data['device_id']} at {data['timestamp']}")
        
    except Exception as e:
        print(f"❌ MongoDB Error: {e}")
        raise HTTPException(status_code=500, detail=f"Database error: {str(e)}")
    
    return {"status": "ok", "inserted_id": data["_id"], "data": data}


@router.get("/sensor-data")
async def get_sensor_data(device_id: str = None, limit: int = 100):
    """
    Retrieve sensor data from MongoDB.
    Optional filter by device_id.
    """
    try:
        collection = get_collection("sensor_readings")
        
        query = {}
        if device_id:
            query["device_id"] = device_id
        
        cursor = collection.find(query).sort("timestamp", -1).limit(limit)
        readings = []
        
        async for doc in cursor:
            doc["_id"] = str(doc["_id"])
            readings.append(doc)
        
        return {"count": len(readings), "data": readings}
        
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"Database error: {str(e)}")


@router.get("/sensor-data/latest")
async def get_latest_reading(device_id: str = None):
    """
    Get the latest sensor reading.
    """
    try:
        collection = get_collection("sensor_readings")
        
        query = {}
        if device_id:
            query["device_id"] = device_id
        
        doc = await collection.find_one(query, sort=[("timestamp", -1)])
        
        if doc:
            doc["_id"] = str(doc["_id"])
            return {"status": "ok", "data": doc}
        else:
            return {"status": "ok", "data": None, "message": "No readings found"}
        
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"Database error: {str(e)}")
