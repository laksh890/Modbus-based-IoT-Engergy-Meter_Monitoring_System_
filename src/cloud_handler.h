#ifndef CLOUD_HANDLER_H
#define CLOUD_HANDLER_H

#include <WiFi.h>
#include <ESP32Firebase.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

class CloudHandler {
public:
    CloudHandler(const char* ssid, const char* password, const char* firebaseHost);
    void begin();
    void updateTime();
    String getISOTime();
    
    Firebase& getFirebase() { return _firebase; }

private:
    const char* _ssid;
    const char* _password;
    const char* _firebaseHost;
    
    WiFiUDP _ntpUDP;
    NTPClient _timeClient;
    Firebase _firebase;

    String formatTimeToISO(time_t epochTime);
};

#endif // CLOUD_HANDLER_H
