"""
MongoDB Database Connection
"""
from motor.motor_asyncio import AsyncIOMotorClient
from pymongo.server_api import ServerApi
import os

# MongoDB Configuration
MONGO_URI = os.getenv("MONGO_URI", "mongodb://localhost:27017")
DATABASE_NAME = os.getenv("DATABASE_NAME", "smartrose_inm")

# Global database client
client: AsyncIOMotorClient = None
db = None


async def connect_to_mongo():
    """Connect to MongoDB"""
    global client, db
    
    print(f"Connecting to MongoDB: {MONGO_URI}")
    client = AsyncIOMotorClient(MONGO_URI, serverSelectionTimeoutMS=5000)
    db = client[DATABASE_NAME]
    
    # Test connection
    try:
        await client.admin.command('ping')
        print(f"✅ Connected to MongoDB database: {DATABASE_NAME}")
    except Exception as e:
        print(f"❌ MongoDB connection failed: {e}")
        raise e


async def close_mongo_connection():
    """Close MongoDB connection"""
    global client
    if client:
        client.close()
        print("MongoDB connection closed")


def get_database():
    """Get database instance"""
    return db


def get_collection(collection_name: str):
    """Get a specific collection"""
    return db[collection_name]

