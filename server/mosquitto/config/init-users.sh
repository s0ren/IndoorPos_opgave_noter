#!/bin/sh

PASSFILE="/mosquitto/config/passwords.txt"
ACLFILE="/mosquitto/config/acl.txt"

# Opret eller ryd eksisterende filer
> "$PASSFILE"
> "$ACLFILE"

# Tilføj subscriber
mosquitto_passwd -b -c "$PASSFILE" subscriber subscriber-password
echo "user subscriber" >> "$ACLFILE"
echo "topic read sensor/#" >> "$ACLFILE"
echo "" >> "$ACLFILE"

# Tilføj devices
for i in $(seq -w 1 20); do
    user="device$i"
    pass="${user}-password"
    topic="sensor/${user}/#"

    mosquitto_passwd -b "$PASSFILE" "$user" "$pass"

    echo "user $user" >> "$ACLFILE"
    echo "topic write $topic" >> "$ACLFILE"
    echo "" >> "$ACLFILE"
done

echo "Brugere og ACL genereret."
