# kommandoer 

## start med at builde docker compose

    docker-compose build

## Start docker compose

med den lokale `docker-compose.yml"

    docker compose up -d   

## Abonner på alle topics i Mosquitto

    docker run --rm eclipse-mosquitto mosquitto_sub -h host.docker.internal -t "#" -v

## Send demo message til Mosquitto mqtt

    <!-- docker run --rm eclipse-mosquitto mosquitto_pub -h host.docker.internal -t sensor/test -m '{"value": 42}' -->

    docker run --rm eclipse-mosquitto mosquitto_pub -h host.docker.internal -t sensor/test -m '{"value": 42.10, "timestamp": 2024-05-20T21:37:49.414Z }'

## Opret virtuelt env til python

    py -3.12 -m venv .venv   

### aktiver .venv

    py -3.12 -m venv .venv 

### installer python pakker i .venv
 
    pip install paho-mqtt pymongo      

## aktiver Pytons virtuelle env

    .\.venv\Scripts\Activate.ps1

## Kør python subscriber, som migrerer til MongoDB

    python mqtt_to_mongo.py

## Se data i mongodb

    docker exec mongodb mongosh "mongodb://localhost:27017/iot" --quiet --eval "db.messages.find().sort({ timestamp: -1 }).limit(10).forEach(doc => printjson(doc));"

# Integrationstest

    pip install -r test/requirements-test.txt

    python .\test\test_integration.py      

# Brugere, password og ACL

## Opret brugere

<!-- ~~    docker run --rm -v "C:\Users\smag\Docs\journal\1205hf25044p\ -\ IoT2\server\mosquitto\config:/mosquitto/config" eclipse-mosquitto mosquitto_passwd -c /mosquitto/config/passwords.txt device01 ~~ 
~~docker run --rm eclipse-mosquitto chmod +x mosquitto/config/init-users.sh~~
~~docker exec -it mosquitto sh /mosquitto/config/init-users.sh~~ -->

    docker-compose exec mosquitto chmod +x /mosquitto/config/init-users.sh

    docker-compose exec mosquitto chmod 0700 /mosquitto/config/passwords.txt
    
    docker-compose exec mosquitto /mosquitto/config/init-users.sh

## test med bruger

### publish

    docker run --rm eclipse-mosquitto mosquitto_pub -h host.docker.internal -p 1883 -t sensor/test -m '{"value": 42.10, "timestamp": "2024-05-20T21:37:49.414Z"}' -u device01 -P device01-password

### subscribe

docker run --rm eclipse-mosquitto mosquitto_sub -h host.docker.internal -p 1883 -t sensor/# -u subscriber -P subscriber-password

...