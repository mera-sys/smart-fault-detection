#include <LoRa.h>
#include <SPI.h>
#include <WiFiMulti.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

WiFiMulti wifiMulti;

#define DEVICE "ESP32"
#define WIFI_SSID "POCO"
#define WIFI_PASSWORD "000000000"
#define INFLUXDB_URL "http://192.168.143.182:8086"
#define INFLUXDB_TOKEN "PuxZinZgWr2rWxqlG7Maqn8JBGPlTmgaF1wf2B0Hq8i9ToI1gZv4PGtA908rjbLDvKd7MfbGzPXdBmlfoK5v3w=="
#define INFLUXDB_ORG "84bb40fa52befcd7"
#define INFLUXDB_BUCKET "ESP32"
#define SERVER_IP "192.168.103.224" // Change this to your receiver ESP32's IP
#define SERVER_PORT 8080           // Change this to the port your receiver is listening on

#define ss 5
#define rst 14
#define dio0 2

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

void setup() {
  Serial.begin(115200);
  
  // Initialize LoRa
  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(433E6)) {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xA5);
  Serial.println("LoRa Initializing OK!");

  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();

  // Check connection to InfluxDB
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    int senderID = LoRa.parseInt(); // Read the sender ID
    Serial.print("Sender ID: ");
    Serial.println(senderID);

    // Read LoRa data
    while (LoRa.available()) {
      String receivedData = LoRa.readString();
      float acValue = parseACValue(receivedData);
      
      // Check sender ID and process data accordingly
      if (senderID == 1) {
        // Handle data for sender ID 1
        Serial.println("Processing data for Sender ID 1");
        // Update tag and field names for sender ID 1
        writeDataToInfluxDB(senderID, acValue, "receivedData1", "senderID1Data");
        // Send data to the receiver ESP32 via WiFi
        sendWiFiData(senderID, receivedData);
      } else if (senderID == 2) {
        // Handle data for sender ID 2
        Serial.println("Processing data for Sender ID 2");
        // Update tag and field names for sender ID 2
        writeDataToInfluxDB(senderID, acValue, "receivedData2", "senderID2Data");
        // Send data to the receiver ESP32 via WiFi
        sendWiFiData(senderID, receivedData);
      }
    }
  }

  // If no WiFi signal, try to reconnect
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi connection lost");
  }

  delay(500); // Delay between LoRa packet checks
}

// Function to parse AC current value from the received LoRa data
float parseACValue(String data) {
  int pos = data.indexOf("AC Current: ");
  if (pos != -1) {
    return data.substring(pos + 12).toFloat();
  }
  return 0.00; // Default value if parsing fails
}

// Function to write data to InfluxDB with custom tag and field names based on sender ID
void writeDataToInfluxDB(int senderID, float acValue, const char* receivedDataTag, const char* senderDataField) {
  Point sensor("sensor");
  sensor.addTag("device", DEVICE);
  sensor.addTag("SSID", WiFi.SSID());
  sensor.addTag("sensortag", receivedDataTag); // Update tag name based on received data
  sensor.addField(senderDataField, acValue); // Update field name based on sender ID

  Serial.print("Writing to InfluxDB: ");
  Serial.println(client.pointToLineProtocol(sensor));
  
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

// Function to send data to the receiver ESP32 via WiFi
void sendWiFiData(int senderID, String message) {
  WiFiClient client;
  if (client.connect(SERVER_IP, SERVER_PORT)) {
    client.print(senderID);
    client.print(",");
    client.println(message);
    client.stop();
    Serial.println("Data sent to receiver via WiFi");
  } else {
    Serial.println("Connection to receiver failed!");
  }
}
