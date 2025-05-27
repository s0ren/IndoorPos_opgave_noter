import os
import json
from datetime import datetime
import paho.mqtt.client as mqtt
from pymongo import MongoClient

# Environment variables
mqtt_host = os.environ.get("MQTT_HOST", "localhost")
mqtt_port = int(os.environ.get("MQTT_PORT", 1883))
mqtt_topic = os.environ.get("MQTT_TOPIC", "sensor/#")
mqtt_username = os.environ.get("MQTT_USERNAME", 'subscriber')
mqtt_password = os.environ.get("MQTT_PASSWORD", 'subscriber-password')


mongo_uri = os.environ.get("MONGO_URI", "mongodb://localhost:27017/")
mongo_db = os.environ.get("MONGO_DB", "iot")
mongo_collection = os.environ.get("MONGO_COLLECTION", "messages")

# MongoDB setup
mongo_client = MongoClient(mongo_uri)
db = mongo_client[mongo_db]
collection = db[mongo_collection]

# Topics to subscribe
topics = [(mqtt_topic, 0)]

def on_connect(client, userdata, flags, rc, properties=None):
    print("Connected to MQTT Broker with result code " + str(rc))
    client.subscribe(topics)

def on_message(client, userdata, msg):
    try:
        payload = msg.payload.decode('utf-8')
        data = json.loads(payload)
    except json.JSONDecodeError:
        data = {"raw": payload}

    document = {
        "topic": msg.topic,
        "message": data,
        "timestamp": datetime.utcnow()
    }
    collection.insert_one(document)
    print(f"Inserted into MongoDB: {document}")

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
if mqtt_username and mqtt_password:
    client.username_pw_set(mqtt_username, mqtt_password)
client.on_connect = on_connect
client.on_message = on_message

client.connect(mqtt_host, mqtt_port, 60)
client.loop_forever()
