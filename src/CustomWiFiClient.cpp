/**************************************************************
  Zabaleni funkcionality pro WiFiClient (odesilani dat pres HTTPS)
 **************************************************************/
#include "Arduino.h"

#include <WiFiClientSecure.h>
#include <CustomWiFiClient.h>
#include <Constants.h>

CustomWiFiClient::CustomWiFiClient() {
   _hostGoogle = "script.google.com";
   _httpPortGoogle = 443;
   _googleScriptBellMacroId = Constants::BELL_MACRO_ID();
   _googleScriptDoorMacroId = Constants::DOOR_MACRO_ID();

   _hostPushbullet = "api.pushbullet.com";
   _httpPortPushbullet = 443;
   _apiKeyPushbullet = Constants::PUSHBULLET_API_KEY();

   _timeout = 0;
   _requestRunning = false;
}

void CustomWiFiClient::sendBellDataGoogle(int duration) {
  if (isnan(duration)) {
    Serial.println("Wrong duration value!");
    if (_callback) {
      _callback(parseHttpResult("FAIL"));
    }
    return;
  }

  if (!_client) {
    WiFiClientSecure _client;
  }

  // volame zabezpecene - WiFiClientSecure, pro obycejne HTTP by stacil WiFiClient
  if (!_client.connect(_hostGoogle, _httpPortGoogle)) {
    Serial.println("connection failed");
    if (_callback) {
      _callback(parseHttpResult("FAIL"));
    }
    return;
  }

  // knstrukce url
  // priklad: https://script.google.com/macros/s/XxxX/exec?delka=2400
  String url = "/macros/s/";
  url += _googleScriptBellMacroId;
  url += "/exec";
  url += "?delka=";
  url += duration;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // posli request
  _client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                "Host: " + _hostGoogle + "\r\n" +
                "Connection: close\r\n\r\n");

  _requestRunning = true;
  _timeout = millis();
}

void CustomWiFiClient::sendDoorDataGoogle(const char* state) {
  if (!_client) {
    WiFiClientSecure _client;
  }

  // volame zabezpecene - WiFiClientSecure, pro obycejne HTTP by stacil WiFiClient
  if (!_client.connect(_hostGoogle, _httpPortGoogle)) {
    Serial.println("connection failed");
    if (_callback) {
      _callback(parseHttpResult("FAIL"));
    }
    return;
  }

  // knstrukce url
  // priklad: https://script.google.com/macros/s/XxxX/exec?delka=2400
  String url = "/macros/s/";
  url += _googleScriptDoorMacroId;
  url += "/exec";
  url += "?state=";
  url += state;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // posli request
  _client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                "Host: " + _hostGoogle + "\r\n" +
                "Connection: close\r\n\r\n");

  _requestRunning = true;
  _timeout = millis();
}

void CustomWiFiClient::sendDataPushbullet(String title, String message) {
  if (!_client) {
    WiFiClientSecure _client;
  }

   // volame zabezpecene - WiFiClientSecure, pro obycejne HTTP by stacil WiFiClient
   if (!_client.connect(_hostPushbullet, _httpPortPushbullet)) {
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
                "Host: " + _hostPushbullet + "\r\n" +
                "Authorization: Bearer " + _apiKeyPushbullet + "\r\n" +
                "Connection: close\r\n" +
                "Content-Type: application/x-www-form-urlencoded;\r\n" +
                "Content-Length: " + postData.length() + "\r\n" +
                "\r\n" +
                postData + "\n");

    Serial.println("Pusbullet request sent... not waiting for response (do not block other operations)");
}

void CustomWiFiClient::monitorState() {
  if (!_requestRunning || !_client) {
    // nebezi pozadavek
    return;
  }

  // precteni vystupu volani skriptu
  if (_client.available()) {
    String firstLine = _client.readStringUntil('\r');
    Serial.println("closing connection");
    _client.stop();
    if (_callback) {
      _callback(parseHttpResult(firstLine));
    }
    _requestRunning = false;
  } else {
    if (millis() - _timeout > 8000) {
      Serial.println(">>> Client Timeout !");
      _client.stop();
      _requestRunning = false;
    }
  }
}

int CustomWiFiClient::parseHttpResult(String httpResultString) {
   // HTTP/1.1 200 - OK
   if (httpResultString.length() > 12) {
      String numericHttpResult = httpResultString.substring(9,12);
      int httpResult = atoi(numericHttpResult.c_str());
      return httpResult == 0 ? -1 : httpResult;
   }
   return -1; // FAILURE
}

void CustomWiFiClient::sentCallback(void (*callback)(int)) {
   _callback = callback;
}
