#include <LoRa.h>
#include <SPI.h>

const int ACPin = 15; // Assuming the AC current sensor is connected to analog pin A0

#define ss 5
#define rst 14
#define dio0 2

#define VREF 3.3
#define AC_THRESHOLD 5.0  // Threshold for low AC current (adjust as needed)

void setup() {
  Serial.begin(115200);
  while (!Serial);

  LoRa.setPins(ss, rst, dio0);

  while (!LoRa.begin(433E6)) {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xA5);
  Serial.println("LoRa Initializing OK!");

  pinMode(13, OUTPUT);
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    int senderID = LoRa.parseInt(); // Read the sender ID
    Serial.print("Sender ID: ");
    Serial.println(senderID);

    // Read LoRa data
    String receivedData = "";
    while (LoRa.available()) {
      receivedData += (char)LoRa.read();
    }
    Serial.println("Received data: " + receivedData);

    // Read AC current value
    float ACCurrentValue = readACCurrentValue();
    Serial.print("AC Current: ");
    Serial.print(ACCurrentValue);
    Serial.println(" A");

    if (ACCurrentValue < AC_THRESHOLD) {
      // If AC current is below threshold, turn on LED
      digitalWrite(13, HIGH);
    } else {
      digitalWrite(13, LOW);
    }

    delay(500);
  }
}

float readACCurrentValue() {
  float ACCurrtntValue = 0;
  float peakVoltage = 0;
  float voltageVirtualValue = 0;   // Vrms

  for (int i = 0; i < 5; i++) {
    peakVoltage += analogRead(ACPin);   // read peak voltage
    delay(1);
  }

  peakVoltage = peakVoltage / 5;
  voltageVirtualValue = peakVoltage * 0.707;    // change the peak voltage to the Virtual Value of voltage

  /* The circuit is amplified by 2 times, so it is divided by 2. */
  voltageVirtualValue = (voltageVirtualValue / 1024 * VREF) / 2;

  ACCurrtntValue = voltageVirtualValue * 20; // Assuming the detection range is 20A

  return ACCurrtntValue;
}
