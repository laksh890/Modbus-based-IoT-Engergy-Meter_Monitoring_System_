#include <HardwareSerial.h>
#include <ModbusMaster.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP32Firebase.h>
#include <ArduinoJson.h>


#define REFERENCE_URL "https://building-management-syst-7627d-default-rtdb.firebaseio.com"
const String FIREBASE_AUTH = "AIzaSyCX12o22sNmekmRaj8pCH7CZNFEdjbayTU";
const String FIREBASE_PATH = "/energyMeters/meter1/data";
Firebase firebase(REFERENCE_URL);

const char* ssid     = "CIL";        // Replace with your network credentials
const char* password = "123456789";    // Replace with your network credentials

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);  // 19800 seconds = UTC+5:30

#define MAX485_DE_0 33
#define MAX485_RE_0 32
#define MAX485_DE_1 25
#define MAX485_RE_1 26
#define MAX485_DE_2 4
#define MAX485_RE_2 2
#define MODBUS_RX_PIN_2 16  // Rx pin
#define MODBUS_TX_PIN_2 17  // Tx pin
#define MODBUS_RX_PIN_1 21  // Rx pin
#define MODBUS_TX_PIN_1 19  // Tx pin
#define MODBUS_RX_PIN_0 3  // Rx pin
#define MODBUS_TX_PIN_0 1  // Tx pin
// HardwareSerial s(2);
ModbusMaster node;

float energybtu;
float energy_total;
float frequency;
float power;
float vlnavg;
float powerfactor;
float irms;
float flow_rate;
float temp_hot;
float temp_cold;
float temp_dif;
float poweren;
uint16_t Temperature;
uint16_t Humidity;
float powerbtu;


float get_address_value(uint16_t address, uint16_t number_of_register) {
  // Serial.println("Fetching the data.....");
  node.clearResponseBuffer();
  bool dgError = true;
  float data = node.readHoldingRegisters(address, number_of_register);

  if (data == node.ku8MBResponseTimedOut) {
    Serial.println("Response TimeOut");
  } else if (data == node.ku8MBIllegalDataAddress) {
    Serial.println("Illegal Data Address");
  } else if (data == node.ku8MBIllegalDataValue) {
    Serial.println("Illegal Data value");
  } else if (data == node.ku8MBIllegalFunction) {
    Serial.println("Illegal Function");
  } else if (data == node.ku8MBInvalidCRC) {
    Serial.println("Invalid CRC");
  } else if (data == node.ku8MBInvalidFunction) {
    Serial.println("Invalid Function");
  } else if (data == node.ku8MBInvalidSlaveID) {
    Serial.println("Invalid Slave Id");
  } else if (data == node.ku8MBSlaveDeviceFailure) {
    Serial.println("Slave Device Failure");
  } else if (data == node.ku8MBSlaveDeviceFailure) {
    Serial.println("Slave Device Failure");
  } else if (data == node.ku8MBSuccess && number_of_register == 2) {
    dgError = false;
    // Serial.println("data Received");
    uint16_t data_1, data_2;
    data_1 = node.getResponseBuffer(0x00);
    data_2 = node.getResponseBuffer(0x01);
    // Serial.print("data_1=");
    // Serial.println(data_1);
    // Serial.print("data_2=");
    // Serial.println(data_2);
    // Serial.print("final_voltage= ");
    float ans = calculateMantissa(data_1, data_2);
    // Serial.println(ans);
    // get_two_bit_address(address);
    return ans;

  } else if (data == node.ku8MBSuccess) {
    float data_3 = node.getResponseBuffer(0x00);
    // Serial.println("Data Feteched");
    // Serial.println(data_3);
    return data_3;
  } else {
    Serial.print("Modbus error: ");
    Serial.println(data, HEX);
  }
  return dgError ? 0 : data;
}

// float get_two_bit_address(uint16_t address) {
//   float data, data_1, data_2;
//   get_address_value(address, 2);
//   data_1 = node.getResponseBuffer(0x00);
//   data_2 = node.getResponseBuffer(0x01);
//   data = ((data_1 * 10) + (data_2));
//   Serial.println(data);
//   return data;
// }

void get_generator_value_energy() {
  // uint16_t nominal_freq = get_address_value(2016, 1);
  // Serial.print("Nominal Frequency: ");
  // Serial.println(nominal_freq);

  float total_energy = get_address_value(158, 2);
  Serial.println("Total Energy " + String(total_energy) + " Wh");
  energy_total = total_energy;
  delay(2000);

  float total_watt = get_address_value(100, 2);
  Serial.println("Total Power " + String(total_watt) + " W");
  poweren = total_watt;
  delay(2000);

  float total_current = get_address_value(148, 2);
  Serial.println("Total Current " + String(total_current) + " A");
  irms = total_current;
  delay(2000);

  float running_hours = get_address_value(156, 2);
  Serial.println("Frequency " + String(frequency) + " Hz");
  frequency = running_hours;
  delay(2000);

  float power_factor = get_address_value(116, 2);  
  Serial.println("Power Factor " + String(power_factor) + " pf");
  powerfactor = power_factor;
  delay(2000);

  float vln = get_address_value(140, 2);
  Serial.println("VLN Average " + String(vln) + " V");
  vlnavg = vln;
}

void get_generator_value() {
  // uint16_t nominal_freq = get_address_value(2016, 1);
  // Serial.print("Nominal Frequency: ");
  // Serial.println(nominal_freq);
  int16_t power_btu = get_address_value(8, 1);
  Serial.println( "Power BTU" + String(power_btu) + " L/m");
  powerbtu = power_btu/100.00;
  delay(2000);

  uint16_t flow = get_address_value(5, 1);
  Serial.println("flow_rate " + String(flow) + " L/m");
  flow_rate = flow/1000.00;
  delay(2000);

  float total_energy_btu = get_address_value(15, 1);  
  Serial.println("total energy " + String(total_energy_btu) + " Wh");
  energybtu = total_energy_btu;
  delay(2000);


  uint16_t hot = get_address_value(1, 1);
  Serial.println("Temp_hot " + String(hot) + " Deg C");
  temp_hot = hot/100.00;
  delay(2000);

  uint16_t cold = get_address_value(2, 1);
  Serial.println("temp_cold " + String(cold) + " Deg C");
  temp_cold = cold/100.00;
  delay(2000);

  int16_t dif = get_address_value(3, 1);  
  Serial.println("temp_dif " + String(dif) + " Deg C");
  temp_dif = dif/100.00;
  delay(2000);

  // float watts = get_address_value(6, 2);
  // Serial.println("power " + String(watts) + " kW");
  // power = watts;
}

void get_generator_value_TH() {
  // uint16_t nominal_freq = get_address_value(2016, 1);
  // Serial.print("Nominal Frequency: ");
  // Serial.println(nominal_freq);

  float tem = get_address_value(7, 2);
  Serial.println("Temperature " + String(tem) + " Deg C");
  Temperature = tem;
  delay(2000);

  float hum = get_address_value(5, 2);
  Serial.println("Humidity " + String(hum) + " %RH");
  Humidity = hum;
  delay(2000);

}


void preTransmission_0() {
  digitalWrite(MAX485_DE_0, HIGH);
  digitalWrite(MAX485_RE_0, HIGH);
}

void postTransmission_0() {
  digitalWrite(MAX485_DE_0, LOW);
  digitalWrite(MAX485_RE_0, LOW);
}

void preTransmission_1() {
  digitalWrite(MAX485_DE_1, HIGH);
  digitalWrite(MAX485_RE_1, HIGH);
}

void postTransmission_1() {
  digitalWrite(MAX485_DE_1, LOW);
  digitalWrite(MAX485_RE_1, LOW);
}

void preTransmission_2() {
  digitalWrite(MAX485_DE_2, HIGH);
  digitalWrite(MAX485_RE_2, HIGH);
}

void postTransmission_2() {
  digitalWrite(MAX485_DE_2, LOW);
  digitalWrite(MAX485_RE_2, LOW);
}


void setup() {
  Serial.begin(9600);
    WiFi.begin(ssid, password);
  
  // Wait for the WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  timeClient.begin();
  timeClient.update();

  firebase.json(true);

  // Modbus protocol
  pinMode(MAX485_DE_0, OUTPUT);
  pinMode(MAX485_RE_0, OUTPUT);
  digitalWrite(MAX485_RE_0, LOW);
  digitalWrite(MAX485_DE_0, LOW);
  pinMode(MAX485_DE_1, OUTPUT);
  pinMode(MAX485_RE_1, OUTPUT);
  digitalWrite(MAX485_RE_1, LOW);
  digitalWrite(MAX485_DE_1, LOW);
  pinMode(MAX485_DE_2, OUTPUT);
  pinMode(MAX485_RE_2, OUTPUT);
  digitalWrite(MAX485_RE_2, LOW);
  digitalWrite(MAX485_DE_2, LOW);
  // s.begin(9600, SERIAL_8N1, MODBUS_RX_PIN, MODBUS_TX_PIN);


}

void loop() {
  timeClient.update();  // Get the time from the NTP server
  unsigned long epochTime = timeClient.getEpochTime();  // Get epoch time
  String isoTime = formatTimeToISO(epochTime);  // Convert to ISO 8601 format

  int j=0;
  int k=0;
  int p=0;

  for (int n = 101; n<103; n++){
  HardwareSerial s(1);
  // ModbusMaster node;
  s.begin(9600, SERIAL_8N1, MODBUS_RX_PIN_1, MODBUS_TX_PIN_1);


  String paths[] = {"/energyMeters/meterH11/data", "/energyMeters/meterH12/data"};
    if ( j < 2) {
    Serial.print("Node Slave ID: ");
    Serial.println(n);
    node.begin(n, s);
    node.preTransmission(preTransmission_1);
    node.postTransmission(postTransmission_1);

    get_generator_value_energy();
    if(frequency ==0 && poweren ==0 && irms ==0 && powerfactor == 0 && vlnavg == 0 ){
    Serial.println("Device is Offline");
    }
    else{
    timeClient.update();  // Get the time from the NTP server

    unsigned long epochTime = timeClient.getEpochTime();  // Get epoch time
    String isoTime = formatTimeToISO(epochTime);  // Convert to ISO 8601 format

    String path = paths[j];
    firebase.setInt(path + "/slaveID", n);
    firebase.setFloat(path + "/energy", energy_total);
    firebase.setFloat(path + "/frequency", frequency);
    firebase.setFloat(path + "/watts", poweren);
    firebase.setFloat(path + "/irms", irms);
    firebase.setFloat(path + "/powerfactor", powerfactor);
    firebase.setFloat(path + "/vlnavg", vlnavg);
    firebase.setString(path + "/timestamp", isoTime);

    Serial.println(isoTime);  // Print time in ISO format
    delay(1000);
   }
  }
  j++;
 }
 delay(2000);
//   for(int i = 104; i<106; i++){
//   HardwareSerial s(2);
//   // ModbusMaster node;
//   s.begin(9600, SERIAL_8N1, MODBUS_RX_PIN_2, MODBUS_TX_PIN_2);

//     String pathsbt[] = {"/btuMeters/meter3/data", "/btuMeters/meter4/data"};
//     if (k<2){
//     Serial.print("Node Slave ID: ");
//     Serial.println(i);
//     node.begin(i, s);
//     node.preTransmission(preTransmission_0);
//     node.postTransmission(postTransmission_0);
//     get_generator_value();
//     if(flow_rate ==0 && temp_hot ==0 && temp_cold ==0 && temp_dif == 0 && power == 0 ){
//     Serial.println("Device is Offline");
//     }
//     else{
//     String path_1 = pathsbt[k];
//     firebase.setFloat(path_1 + "/power", powerbtu);
//     firebase.setFloat(path_1 + "/energy", energybtu);
//     firebase.setFloat(path_1 + "/flow_rate", flow_rate);
//     firebase.setFloat(path_1 + "/temp_hot", temp_hot);
//     firebase.setFloat(path_1 + "/temp_cold", temp_cold);
//     firebase.setFloat(path_1 + "/temp_dif", temp_dif);
//     firebase.setFloat(path_1 + "/power", power);
//     firebase.setString(path_1 + "/timestamp", isoTime);

//     Serial.println(isoTime);  // Print time in ISO format
//     delay(1000);

//     }
//   }
//   k++;
//  }

for (int t=1; t<6; t++){
  HardwareSerial s(2);
  // ModbusMaster node;
  s.begin(9600, SERIAL_8N1, MODBUS_RX_PIN_2, MODBUS_TX_PIN_2);

  String pathsth[] = {"/temperatureHumidity/meter1/data", "/temperatureHumidity/meter2/data", "/temperatureHumidity/meter3/data", "/temperatureHumidity/meter4/data", "/temperatureHumidity/meter5/data"};
    if ( p < 5) {
    Serial.print("Node Slave ID: ");
    Serial.println(t);
    node.begin(t, s);
    node.preTransmission(preTransmission_2);
    node.postTransmission(postTransmission_2);

    get_generator_value_TH();
    if(Temperature ==0 && Humidity ==0 ){
    Serial.println("Device is Offline");
    }
    else{
    String path_2 = pathsth[p];
    firebase.setFloat(path_2 + "/Temperature", Temperature);
    firebase.setFloat(path_2 + "/Humidity", Humidity);
    firebase.setString(path_2 + "/timestamp", isoTime);

    Serial.println(isoTime);  // Print time in ISO format
    delay(5000);
    }
    }
    
    p++;
  }

}

// Function to convert a decimal number to a 16-bit binary string
String decimalToBinary16(int decimalValue) {
  String binaryString = String(decimalValue, BIN);
  // Pad the binary string to make it 16 bits long
  while (binaryString.length() < 16) {
    binaryString = "0" + binaryString;
  }
  return binaryString;
}

// Function to calculate the mantissa using the two binary values
float calculateMantissa(int data1, int data2) {
  // Convert decimal values to binary strings
  String binData1 = decimalToBinary16(data1);
  String binData2 = decimalToBinary16(data2);

  // Combine the two 16-bit binary numbers into one 32-bit string
  String combinedBinary = binData2 + binData1;

  // Extract the sign, exponent, and mantissa as per IEEE-754 format
  int sign = combinedBinary.substring(0, 1).toInt();
  int exponent = strtol(combinedBinary.substring(1, 9).c_str(), NULL, 2);  // 8 bits
  String mantissaBits = combinedBinary.substring(9);                       // Remaining bits for mantissa

  // Calculate the mantissa
  float mantissa = 1.0;  // Start with 1 as implied by IEEE 754
  for (int i = 0; i < mantissaBits.length(); i++) {
    if (mantissaBits.charAt(i) == '1') {
      mantissa += pow(2, -(i + 1));  // Add powers of 2 for each bit
    }
  }

  // Final value calculation using the exponent
  float finalValue = mantissa * pow(2, exponent - 127);  // Subtracting 127 for bias in IEEE-754

  // If the sign is 1, the value is negative
  if (sign == 1) {
    finalValue = -finalValue;
  }

  return finalValue;
}

// Function to format time in ISO 8601 format
String formatTimeToISO(time_t epochTime) {
    struct tm timeinfo;
    gmtime_r(&epochTime, &timeinfo);  // Convert epoch to UTC time structure

    char isoStr[30];
    snprintf(isoStr, sizeof(isoStr), "%04d-%02d-%02dT%02d:%02d:%02dZ",
             timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    return String(isoStr);
}
