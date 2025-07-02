# Smart Fault Detection for Secondary Distribution Lines

An ESP32-based smart system designed to monitor and detect faults in secondary power distribution lines. It uses **analog AC current sensors** and communicates via **SX1278 LoRa** for remote monitoring and early detection.

---

## ⚙️ Key Features

- 📡 **Wireless Communication** using SX1278 LoRa module  
- 🔌 **AC Current Sensing** using analog current sensor  
- 🧠 **Microcontroller**: ESP32 (WiFi + Bluetooth)  
- 📈 Real-time data transmission to receiver node or dashboard  
- 🔔 Alerts for abnormal or faulty readings

---

## 🧪 Hardware Components

| Component        | Model              |
|------------------|--------------------|
| Microcontroller  | ESP32              |
| Current Sensor   | Analog AC Sensor   |
| LoRa Module      | SX1278             |
| Power Supply     | 5V or Battery      |

---

## 📁 File Structure

```bash
smart-fault-detection/
├── fault_detection.ino        # Arduino sketch for ESP32
├── wiring_diagram.png         # Optional: schematic or Fritzing file
├── README.md                  # Project description
