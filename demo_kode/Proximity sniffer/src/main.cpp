#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>

struct ProximityDevice {
  uint8_t mac[6];
  int8_t rssi;
  uint32_t lastSeen;
  int8_t minRssi;
  int8_t maxRssi;
  uint32_t firstSeen;
  String proximity;
};

const int MAX_DEVICES = 300;
ProximityDevice devices[MAX_DEVICES];
int deviceCount = 0;

// Proximity thresholds
const int8_t VERY_CLOSE = -40;  // < 1 meter
const int8_t CLOSE = -55;       // 1-5 meters  
const int8_t NEAR = -70;        // 5-15 meters
const int8_t FAR = -80;         // 15-30 meters


String macToString(uint8_t* mac) {
  char macStr[18];
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", 
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(macStr);
}

String getProximityString(int8_t rssi) {
  if (rssi >= VERY_CLOSE) return "VERY CLOSE";
  else if (rssi >= CLOSE) return "CLOSE";
  else if (rssi >= NEAR) return "NEAR";
  else if (rssi >= FAR) return "FAR";
  else return "VERY FAR";
}

String getProximityEmoji(int8_t rssi) {
  if (rssi >= VERY_CLOSE) return "🔥";
  else if (rssi >= CLOSE) return "📱";
  else if (rssi >= NEAR) return "📡";
  else return "🌐";
}

void updateDevice(uint8_t* mac, int8_t rssi) {
  // Find existing device
  for (int i = 0; i < deviceCount; i++) {
    if (memcmp(devices[i].mac, mac, 6) == 0) {
      devices[i].rssi = rssi;
      devices[i].lastSeen = millis();
      
      // Update min/max RSSI
      if (rssi < devices[i].minRssi) devices[i].minRssi = rssi;
      if (rssi > devices[i].maxRssi) devices[i].maxRssi = rssi;
      
      // Update proximity
      devices[i].proximity = getProximityString(rssi);
      return;
    }
  }
  
  // Add new device
  if (deviceCount < MAX_DEVICES) {
    memcpy(devices[deviceCount].mac, mac, 6);
    devices[deviceCount].rssi = rssi;
    devices[deviceCount].minRssi = rssi;
    devices[deviceCount].maxRssi = rssi;
    devices[deviceCount].lastSeen = millis();
    devices[deviceCount].firstSeen = millis();
    devices[deviceCount].proximity = getProximityString(rssi);
    
    Serial.printf("🔍 New device detected: %s - %s (%d dBm)\n", 
                  macToString(devices[deviceCount].mac).c_str(),
                  devices[deviceCount].proximity.c_str(),
                  rssi);
    
    deviceCount++;
  }
}

void snifferCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
  if (type != WIFI_PKT_MGMT) return;
  
  wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
  wifi_pkt_rx_ctrl_t ctrl = pkt->rx_ctrl;
  
  
  uint8_t* payload = pkt->payload;
  uint8_t frameSubType = (payload[0] & 0xF0) >> 4;
  
  // Probe requests from client devices
  if (frameSubType == 4) {
    uint8_t* srcMac = &payload[10];
    
    
    // Skip random MAC addresses
    // if (srcMac[0] & 0x02) return;
    // Serial.println(srcMac[0]);
    if (srcMac[0] == 0xB8) return; // der er bare en irreterende ting her som starter med B8
    
    // Only track devices within reasonable range
    // if (ctrl.rssi < FAR) return;
    if (ctrl.rssi < VERY_CLOSE) return;
    
    updateDevice(srcMac, ctrl.rssi);

    Serial.printf("🔍 Package detected: %s - %d (%d dBm)\n", 
                  macToString(srcMac).c_str(),
                  ctrl.timestamp,
                  // devices[deviceCount].proximity.c_str(),
                  ctrl.rssi);

  //   uint32_t ageSeconds = (millis() - devices[i].lastSeen) / 1000;
  //   uint32_t durationSeconds = (millis() - devices[i].firstSeen) / 1000;
  //   Serial.printf("%s %s | %4d dBm | %s | Age: %ds | Duration: %ds\n",
  //                 getProximityEmoji(devices[i].rssi).c_str(),
  //                 macToString(devices[i].mac).c_str(),
  //                 devices[i].rssi,
  //                 devices[i].proximity.c_str(),
  //                 ageSeconds,
  //                 durationSeconds);
  }
}

void sortDevicesByRSSI() {
  // Simple bubble sort by RSSI (descending)
  for (int i = 0; i < deviceCount - 1; i++) {
    for (int j = 0; j < deviceCount - i - 1; j++) {
      if (devices[j].rssi < devices[j + 1].rssi) {
        ProximityDevice temp = devices[j];
        devices[j] = devices[j + 1];
        devices[j + 1] = temp;
      }
    }
  }
}

void sortDevicesByAge() {
  // Simple bubble sort by Age (descending)
  for (int i = 0; i < deviceCount - 1; i++) {
    for (int j = 0; j < deviceCount - i - 1; j++) {
      if (devices[j].lastSeen < devices[j + 1].lastSeen) {
        ProximityDevice temp = devices[j];
        devices[j] = devices[j + 1];
        devices[j + 1] = temp;
      }
    }
  }
}

void updateProximityDisplay() {
  if (deviceCount == 0) {
    Serial.println("👀 No devices detected nearby...");
    return;
  }
  
  Serial.println("\n📍 NEARBY DEVICES");
  Serial.println("================");
  
  // Sort devices by RSSI (closest first)
  sortDevicesByRSSI();
  
  for (int i = 0; i < deviceCount; i++) {
    uint32_t ageSeconds = (millis() - devices[i].lastSeen) / 1000;
    uint32_t durationSeconds = (millis() - devices[i].firstSeen) / 1000;
    
    Serial.printf("%s %s | %4d dBm | %s | Age: %ds | Duration: %ds\n",
                  getProximityEmoji(devices[i].rssi).c_str(),
                  macToString(devices[i].mac).c_str(),
                  devices[i].rssi,
                  devices[i].proximity.c_str(),
                  ageSeconds,
                  durationSeconds);
  }
  
  Serial.printf("\nTotal devices: %d\n", deviceCount);
  Serial.println("────────────────────────────────────────────────────");
}

void cleanupDevices() {
  uint32_t now = millis();
  int removed = 0;
  
  for (int i = 0; i < deviceCount; i++) {
    // if (now - devices[i].lastSeen > 20000) {  // 20 second timeout
    if (deviceCount > MAX_DEVICES - 10) { // 5 away from limit
      Serial.printf("👋 Device left: %s\n", macToString(devices[i].mac).c_str());
      
      // Shift array to remove device
      for (int j = i; j < deviceCount - 1; j++) {
        devices[j] = devices[j + 1];
      }
      deviceCount--;
      i--;
      removed++;
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("ESP32 Device Proximity Tracker");
  Serial.println("==============================");
  Serial.println("Tracking nearby WiFi-enabled devices by RSSI");
  Serial.println();
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&snifferCallback);
  esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE);  // Channel 6 is commonly used
  
  Serial.println("🎯 Proximity tracking active...\n");
}

void loop() {
  static uint32_t lastUpdate = 0;
  if (millis() - lastUpdate > 3000) {
    // updateProximityDisplay();
    lastUpdate = millis();
  }
  
  // Cleanup old devices
  static uint32_t lastCleanup = 0;
  if (millis() - lastCleanup > 15000) {
    cleanupDevices();
    lastCleanup = millis();
  }
  
  delay(100);
}
