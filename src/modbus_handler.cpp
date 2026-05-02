#include "modbus_handler.h"
#include <Arduino.h>

uint8_t ModbusHandler::_staticDePin = 0;
uint8_t ModbusHandler::_staticRePin = 0;

ModbusHandler::ModbusHandler(HardwareSerial& serial, uint8_t dePin, uint8_t rePin) 
    : _serial(serial), _dePin(dePin), _rePin(rePin) {
}

void ModbusHandler::begin(uint32_t baud, uint32_t config, uint8_t rxPin, uint8_t txPin) {
    _serial.begin(baud, config, rxPin, txPin);
    
    pinMode(_dePin, OUTPUT);
    pinMode(_rePin, OUTPUT);
    digitalWrite(_dePin, LOW);
    digitalWrite(_rePin, LOW);

    _staticDePin = _dePin;
    _staticRePin = _rePin;
}

void ModbusHandler::preTransmission() {
    digitalWrite(_staticDePin, HIGH);
    digitalWrite(_staticRePin, HIGH);
}

void ModbusHandler::postTransmission() {
    digitalWrite(_staticDePin, LOW);
    digitalWrite(_staticRePin, LOW);
}

float ModbusHandler::getAddressValue(uint8_t slaveId, uint16_t address, uint16_t numRegisters) {
    _node.begin(slaveId, _serial);
    
    // Set static pins for callbacks before calling node methods
    _staticDePin = _dePin;
    _staticRePin = _rePin;
    
    _node.preTransmission(preTransmission);
    _node.postTransmission(postTransmission);

    _node.clearResponseBuffer();
    uint8_t result = _node.readHoldingRegisters(address, numRegisters);

    if (result == _node.ku8MBSuccess) {
        if (numRegisters == 2) {
            uint16_t data_1 = _node.getResponseBuffer(0x00);
            uint16_t data_2 = _node.getResponseBuffer(0x01);
            return calculateMantissa(data_1, data_2);
        } else {
            return _node.getResponseBuffer(0x00);
        }
    } else {
        Serial.printf("Modbus Error (Slave %d, Addr %d): 0x%02X\n", slaveId, address, result);
        return -1.0f; // Error indicator
    }
}

String ModbusHandler::decimalToBinary16(int decimalValue) {
    String binaryString = String(decimalValue, BIN);
    while (binaryString.length() < 16) {
        binaryString = "0" + binaryString;
    }
    return binaryString;
}

float ModbusHandler::calculateMantissa(int data1, int data2) {
    String binData1 = decimalToBinary16(data1);
    String binData2 = decimalToBinary16(data2);
    String combinedBinary = binData2 + binData1;

    int sign = combinedBinary.substring(0, 1).toInt();
    int exponent = strtol(combinedBinary.substring(1, 9).c_str(), NULL, 2);
    String mantissaBits = combinedBinary.substring(9);

    float mantissa = 1.0;
    for (int i = 0; i < mantissaBits.length(); i++) {
        if (mantissaBits.charAt(i) == '1') {
            mantissa += pow(2, -(i + 1));
        }
    }

    float finalValue = mantissa * pow(2, exponent - 127);
    if (sign == 1) finalValue = -finalValue;

    return finalValue;
}
