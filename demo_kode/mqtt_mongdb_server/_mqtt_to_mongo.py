import paho.mqtt.client as mqtt
from pymongo import MongoClient
import json
from datetime import datetime

# MongoDB setup
mongo_client = MongoClient("mongodb://localhost:27017/")
db = mongo_client["iot"]
collection = db["messages"]

# Topics to subscribe to
# topics = [("sensor/temperature", 0), ("sensor/humidity", 0), ("sensor/status", 0)]
topics = [("sensor/#", 0)]

# Callback når forbindelse oprettes
def on_connect(client, userdata, flags, rc, properties=None):
    print("Connected to MQTT Broker with result code " + str(rc))
    client.subscribe(topics)

# Callback når der modtages besked
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

# MQTT client opsætning
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message

client.connect("localhost", 1883, 60)
client.loop_forever()
