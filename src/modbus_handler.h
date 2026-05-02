#ifndef MODBUS_HANDLER_H
#define MODBUS_HANDLER_H

#include <ModbusMaster.h>
#include <HardwareSerial.h>

class ModbusHandler {
public:
    ModbusHandler(HardwareSerial& serial, uint8_t dePin, uint8_t rePin);
    void begin(uint32_t baud, uint32_t config, uint8_t rxPin, uint8_t txPin);
    float getAddressValue(uint8_t slaveId, uint16_t address, uint16_t numRegisters);
    
private:
    HardwareSerial& _serial;
    ModbusMaster _node;
    uint8_t _dePin;
    uint8_t _rePin;

    static uint8_t _staticDePin;
    static uint8_t _staticRePin;
    static void preTransmission();
    static void postTransmission();

    float calculateMantissa(int data1, int data2);
    String decimalToBinary16(int decimalValue);
};

#endif // MODBUS_HANDLER_H
