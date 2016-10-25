/**************************************************************
  Zabaleni funkcionality pro WiFiClient (odesilani dat pres HTTPS)
 **************************************************************/
#include "Arduino.h"

#include <WiFiClientSecure.h>
#include <PushbulletWiFiClient.h>
#include <Ticker.h>
#include <Constants.h>

PushbulletWiFiClient::PushbulletWiFiClient() {
   WiFiClientSecure _client;
   _host = "api.pushbullet.com";
   _httpPort = 443;
   // FIXME zmenit na cilove API_KEY
   _apiKey = Constants::PUSHBULLET_API_KEY();
}

void PushbulletWiFiClient::init() {
}

void PushbulletWiFiClient::sendData(String title, String message) {
   // volame zabezpecene - WiFiClientSecure, pro obycejne HTTP by stacil WiFiClient
   if (!_client.connect(_host, _httpPort)) {
     Serial.println("connection failed");
     return;
   }

   // konstrukce url
   String url = "/v2/pushes";

   Serial.print("Requesting URL: ");
   Serial.println(url);

   String postData = String("type=note&") + "title=" + title + "&" + "body=" + message;

   // posli request
   _client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                "Host: " + _host + "\r\n" +
                "Authorization: Bearer " + _apiKey + "\r\n" +
                "Connection: close\r\n" +
                "Content-Type: application/x-www-form-urlencoded;\r\n" +
                "Content-Length: " + postData.length() + "\r\n" +
                "\r\n" +
                postData + "\n");

    unsigned long timeout = millis();
    while (_client.available() == 0) {
      if (millis() - timeout > 8000) {
        Serial.println(">>> Client Timeout !");
        _client.stop();
        return;
      }
    }

    // precteni vystupu volani skriptu
    while (_client.available()){
      String line = _client.readStringUntil('\r');
      Serial.print(line);
    }

    Serial.println();
    Serial.println("closing connection");
}
