# ⚡ IoT Energy & BTU Monitoring System

A real-time industrial-grade monitoring system built using ESP32, designed to collect electrical and thermal parameters via Modbus RTU and push them to the cloud using Firebase Realtime Database.

This system is ideal for Building Management Systems (BMS), HVAC monitoring, and industrial energy analytics.

🚀 Features
📡 Modbus RTU Communication
Reads data from energy meters and BTU meters
⚡ Electrical Parameter Monitoring
Voltage (VLN)
Current (Irms)
Power (W)
Energy (Wh)
Power Factor
Frequency


🌡 Thermal Monitoring (BTU Meter)
Flow rate
Hot & Cold temperatures
Temperature difference
Energy in BTU

☁️ Cloud Integration
Sends real-time data to Firebase

🕒 Time Synchronization
Uses NTP for accurate timestamping

🔌 Multi-Device Support
Multiple RS485 channels for scalability

## 🧠 System Architecture
Energy Meter / BTU Meter
        │
   RS485 (Modbus RTU)
        │
      ESP32
        │
   WiFi Network
        │
   Firebase Realtime DB
        │
   Dashboard / BMS

## 🛠 Hardware Used
ESP32
RS485 to TTL Converter (MAX485)
Energy Meter (Modbus compatible)
BTU Meter
Temperature Sensors (Integrated with BTU meter)


## 📦 Libraries Used
#include <HardwareSerial.h>
#include <ModbusMaster.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP32Firebase.h>
#include <ArduinoJson.h>


## 🔌 Pin Configuration
Function	Pin
MAX485 DE (0)	33
MAX485 RE (0)	32
MAX485 DE (1)	25
MAX485 RE (1)	26
MAX485 DE (2)	4
MAX485 RE (2)	2
RX (Channel 2)	16
TX (Channel 2)	17
RX (Channel 1)	21
TX (Channel 1)	19
RX (Channel 0)	3
TX (Channel 0)	1


## 📡 Data Parameters Collected
Electrical Data
Total Energy (Wh)
Power (W)
Current (A)
Voltage (V)
Frequency (Hz)
Power Factor
Thermal / BTU Data
Flow Rate
Temperature (Hot & Cold)
Temperature Difference
BTU Energy
Thermal Power

## ☁️ Firebase Configuration
#define REFERENCE_URL "https://your-project.firebaseio.com"
const String FIREBASE_AUTH = "YOUR_API_KEY";
const String FIREBASE_PATH = "/energyMeters/meter1/data";


## 🌐 WiFi Configuration
const char* ssid     = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";

## 🕒 Time Sync
Uses NTP (pool.ntp.org)
Configurable offset for timezone
Ensures accurate logging

## 🔍 Key Functions Explained
get_address_value()
Reads Modbus registers
Handles communication errors
Converts 16-bit / 32-bit values
get_generator_value_energy()
Reads electrical parameters
Updates global variables
get_generator_value()
Reads BTU-related parameters

## ⚠️ Error Handling

The system handles:

Modbus timeout
CRC errors
Invalid slave response
Illegal register access


## 🧪 Use Cases
🏢 Building Management Systems (BMS)
❄️ HVAC Monitoring
⚡ Energy Auditing
🏭 Industrial Automation
📊 Smart Energy Analytics

## 📈 Future Improvements
Web dashboard for visualization
MQTT support
Edge AI for anomaly detection
OTA firmware updates
Multi-node distributed system
🔐 Security Notes
Do not expose Firebase API keys publicly
Use authentication rules in Firebase
Consider HTTPS + token-based auth
📄 License

MIT License

## 👨‍💻 Author

Lakshay Gandotra

IoT | Embedded Systems | Smart Infrastructure
Focused on real-world scalable engineering solutions