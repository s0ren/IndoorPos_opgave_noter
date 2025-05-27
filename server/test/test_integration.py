import os
import time
import json
import unittest
from dotenv import load_dotenv
from pymongo import MongoClient
import paho.mqtt.publish as publish

class TestMQTTToMongoDBIntegration(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        # Load environment variables from .env
        # load_dotenv('.env')
        load_dotenv('../.env')

        # cls.mqtt_host = os.getenv("MQTT_HOST", "localhost")
        cls.mqtt_host = "localhost"
        cls.mqtt_host = "192.168.0.130"
        cls.mqtt_port = int(os.getenv("MQTT_PORT", 1883))
        cls.mqtt_username = os.getenv("MQTT_USERNAME")
        cls.mqtt_password = os.getenv("MQTT_PASSWORD")
        cls.mqtt_topic = os.getenv("MQTT_TOPIC", "sensor/test")

        # cls.mongo_uri = os.getenv("MONGO_URI", "mongodb://localhost:27017/")
        cls.mongo_uri = "mongodb://192.168.0.130:27017/"
        cls.mongo_db = os.getenv("MONGO_DB", "iot")
        cls.mongo_collection = os.getenv("MONGO_COLLECTION", "messages")

    def _test_publish_and_store_in_mongo(self):
        test_value = 99999
        test_payload = {"value": test_value}
        # auth = {"username": self.mqtt_username, "password": self.mqtt_password} if self.mqtt_username else None
        auth = {"username": "device20", "password": "device20-password"}

        #DEBUG 
        # print('test_payload:', test_payload)
        # print('auth:', auth)
        # print('self.mqtt_host:', self.mqtt_host)
        # print('self.mqtt_port:', self.mqtt_port)

        # Publish to MQTT
        publish.single(
            topic=self.mqtt_topic.replace("#", "test"),  # Replace wildcard for publishing
            payload=json.dumps(test_payload),
            hostname=self.mqtt_host,
            port=self.mqtt_port,
            auth=auth,
            # qos=2
        )

        # Wait for the subscriber to process
        time.sleep(2)

        # Query MongoDB
        client = MongoClient(self.mongo_uri)
        collection = client[self.mongo_db][self.mongo_collection]

        result = collection.find_one(
            {"topic": self.mqtt_topic.replace("#", "test"), "message.value": test_value},
            sort=[("timestamp", -1)]
        )
        client.close()

        self.assertIsNotNone(result, "No document found in MongoDB matching the test payload.")

    def test_publish_and_store_in_mongo_device01(self):
        test_value = 99999
        test_payload = {"value": test_value}
        # auth = {"username": self.mqtt_username, "password": self.mqtt_password} if self.mqtt_username else None
        auth = {"username": "device01", "password": "device01-password"}

        #DEBUG 
        # print('test_payload:', test_payload)
        # print('auth:', auth)
        # print('self.mqtt_host:', self.mqtt_host)
        # print('self.mqtt_port:', self.mqtt_port)

        # Publish to MQTT
        publish.single(
            topic=self.mqtt_topic.replace("#", "sensor/device01"),  # Replace wildcard for publishing
            payload=json.dumps(test_payload),
            hostname=self.mqtt_host,
            port=self.mqtt_port,
            auth=auth
        )

        # Wait for the subscriber to process
        time.sleep(2)

        # Query MongoDB
        client = MongoClient(self.mongo_uri)
        collection = client[self.mongo_db][self.mongo_collection]

        result = collection.find_one(
            {"topic": self.mqtt_topic.replace("#", "test"), "message.value": test_value},
            sort=[("timestamp", -1)]
        )
        client.close()

        self.assertIsNotNone(result, "No document found in MongoDB matching the test payload.")

if __name__ == "__main__":
    unittest.main()
