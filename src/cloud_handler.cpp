#include "cloud_handler.h"
#include <Arduino.h>

CloudHandler::CloudHandler(const char* ssid, const char* password, const char* firebaseHost)
    : _ssid(ssid), _password(password), _firebaseHost(firebaseHost), 
      _timeClient(_ntpUDP, "pool.ntp.org", 0, 60000), _firebase(firebaseHost) {
}

void CloudHandler::begin() {
    Serial.print("Connecting to WiFi: ");
    Serial.println(_ssid);
    WiFi.begin(_ssid, _password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");

    _timeClient.begin();
    _timeClient.update();
    _firebase.json(true);
}

void CloudHandler::updateTime() {
    _timeClient.update();
}

String CloudHandler::getISOTime() {
    return formatTimeToISO(_timeClient.getEpochTime());
}

String CloudHandler::formatTimeToISO(time_t epochTime) {
    struct tm timeinfo;
    gmtime_r(&epochTime, &timeinfo);

    char isoStr[30];
    snprintf(isoStr, sizeof(isoStr), "%04d-%02d-%02dT%02d:%02d:%02dZ",
             timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    return String(isoStr);
}
