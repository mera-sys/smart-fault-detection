#include <LoRa.h>
#include <SPI.h>

const int ACPin = 15;
#define ACTectionRange 20;  // set Non-invasive AC Current Sensor detection range (5A,10A,20A)

#define ss 5
#define rst 14
#define dio0 2

#define VREF 3.3
#define AC_THRESHOLD 5.0  // Threshold for low AC current (adjust as needed)

int slaveID = 1; // Unique ID for each slave node

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
  float ACCurrentValue = readACCurrentValue(); // read AC Current Value

  Serial.print("Engineering Department ");
  Serial.print(slaveID);
  Serial.print(" - AC Current: ");
  Serial.print(ACCurrentValue);
  Serial.println(" A");

  if (ACCurrentValue < AC_THRESHOLD) {
    // If AC current is below threshold, turn on LED
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(13, LOW);
  }

  LoRa.beginPacket();
  LoRa.print(slaveID); // Send slave ID
  LoRa.print(": AC Current: ");
  LoRa.print(ACCurrentValue);
  LoRa.print(" A");
  LoRa.endPacket();

  delay(500);
  delay(9500); // 10 seconds minus the time spent on other operations
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

  ACCurrtntValue = voltageVirtualValue * ACTectionRange;

  return ACCurrtntValue;
}
