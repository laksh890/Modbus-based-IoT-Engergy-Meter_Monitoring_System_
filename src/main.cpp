#include <Arduino.h>
#include "config.h"
#include "modbus_handler.h"
#include "cloud_handler.h"

// Initialize Handlers
ModbusHandler modbusCh1(Serial1, MAX485_DE_1, MAX485_RE_1); // Energy Meters
ModbusHandler modbusCh2(Serial2, MAX485_DE_2, MAX485_RE_2); // TH & BTU Meters
CloudHandler cloud(WIFI_SSID, WIFI_PASSWORD, FIREBASE_REFERENCE_URL);

void setup() {
    Serial.begin(9600);
    cloud.begin();

    // Channel 1: Energy Meters (Slave 101, 102)
    modbusCh1.begin(9600, SERIAL_8N1, MODBUS_RX_PIN_1, MODBUS_TX_PIN_1);
    
    // Channel 2: Temp/Humidity (Slave 1-5) and BTU (Slave 104-105)
    modbusCh2.begin(9600, SERIAL_8N1, MODBUS_RX_PIN_2, MODBUS_TX_PIN_2);

    Serial.println("System Initialized");
}

void readAndUploadEnergyMeters(String isoTime) {
    String paths[] = {"/energyMeters/meterH11/data", "/energyMeters/meterH12/data"};
    int pathIdx = 0;

    for (int slaveId = ENERGY_SLAVE_START; slaveId <= ENERGY_SLAVE_END; slaveId++) {
        Serial.printf("Reading Energy Meter Slave ID: %d\n", slaveId);
        
        float energy = modbusCh1.getAddressValue(slaveId, 158, 2);
        delay(1000); // Small delay between register reads as per original code
        float power = modbusCh1.getAddressValue(slaveId, 100, 2);
        delay(1000);
        float current = modbusCh1.getAddressValue(slaveId, 148, 2);
        delay(1000);
        float freq = modbusCh1.getAddressValue(slaveId, 156, 2); // frequency in code
        delay(1000);
        float pf = modbusCh1.getAddressValue(slaveId, 116, 2);
        delay(1000);
        float vln = modbusCh1.getAddressValue(slaveId, 140, 2);

        if (energy != -1.0f) {
            String path = paths[pathIdx++];
            Firebase& firebase = cloud.getFirebase();
            firebase.setInt(path + "/slaveID", slaveId);
            firebase.setFloat(path + "/energy", energy);
            firebase.setFloat(path + "/watts", power);
            firebase.setFloat(path + "/irms", current);
            firebase.setFloat(path + "/frequency", freq);
            firebase.setFloat(path + "/powerfactor", pf);
            firebase.setFloat(path + "/vlnavg", vln);
            firebase.setString(path + "/timestamp", isoTime);
            Serial.println("Energy data uploaded to " + path);
        } else {
            Serial.println("Device is Offline or Communication Error");
        }
    }
}

void readAndUploadTHSensors(String isoTime) {
    for (int slaveId = TH_SLAVE_START; slaveId <= TH_SLAVE_END; slaveId++) {
        Serial.printf("Reading TH Sensor Slave ID: %d\n", slaveId);
        
        float temp = modbusCh2.getAddressValue(slaveId, 7, 2);
        delay(1000);
        float hum = modbusCh2.getAddressValue(slaveId, 5, 2);

        if (temp != -1.0f) {
            String path = "/temperatureHumidity/meter" + String(slaveId) + "/data";
            Firebase& firebase = cloud.getFirebase();
            firebase.setFloat(path + "/Temperature", temp);
            firebase.setFloat(path + "/Humidity", hum);
            firebase.setString(path + "/timestamp", isoTime);
            Serial.println("TH data uploaded to " + path);
        } else {
            Serial.println("Device is Offline");
        }
    }
}

void loop() {
    cloud.updateTime();
    String isoTime = cloud.getISOTime();

    readAndUploadEnergyMeters(isoTime);
    delay(2000);
    
    readAndUploadTHSensors(isoTime);
    delay(5000);
}
