#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- WiFi Configuration ---
const char* WIFI_SSID     = "CIL";
const char* WIFI_PASSWORD = "123456789";

// --- Firebase Configuration ---
#define FIREBASE_REFERENCE_URL "https://building-management-syst-7627d-default-rtdb.firebaseio.com"
const String FIREBASE_AUTH = "AIzaSyCX12o22sNmekmRaj8pCH7CZNFEdjbayTU";

// --- Pin Configuration ---

// Channel 0 (used for BTU in original code, though commented out)
#define MAX485_DE_0 33
#define MAX485_RE_0 32
#define MODBUS_RX_PIN_0 3
#define MODBUS_TX_PIN_0 1

// Channel 1 (Energy Meters)
#define MAX485_DE_1 25
#define MAX485_RE_1 26
#define MODBUS_RX_PIN_1 21
#define MODBUS_TX_PIN_1 19

// Channel 2 (Temp/Humidity Sensors)
#define MAX485_DE_2 4
#define MAX485_RE_2 2
#define MODBUS_RX_PIN_2 16
#define MODBUS_TX_PIN_2 17

// --- Modbus Slave Configuration ---
#define ENERGY_SLAVE_START 101
#define ENERGY_SLAVE_END   102

#define TH_SLAVE_START     1
#define TH_SLAVE_END       5

#define BTU_SLAVE_START    104
#define BTU_SLAVE_END      105

// --- NTP Configuration ---
#define NTP_SERVER "pool.ntp.org"
#define NTP_OFFSET 0
#define NTP_INTERVAL 60000

#endif // CONFIG_H
