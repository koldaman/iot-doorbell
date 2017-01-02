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
   _apiKeyPushbullet1 = "NA";
   _activeRingPushbullet1 = true;
   _activeDoorPushbullet1 = true;
   _apiKeyPushbullet2 = "NA";
   _activeRingPushbullet2 = true;
   _activeDoorPushbullet2 = true;

   _timeout = 0;
   _requestRunning = false;
}

void CustomWiFiClient::setApiKeyPushbullet1(String apiKey) {
  _apiKeyPushbullet1 = apiKey;
}

String CustomWiFiClient::getApiKeyPushbullet1() {
  return _apiKeyPushbullet1;
}

void CustomWiFiClient::setActiveRingPushbullet1(bool active) {
  _activeRingPushbullet1 = active;
}

bool CustomWiFiClient::isActiveRingPushbullet1() {
  return _activeRingPushbullet1;
}

void CustomWiFiClient::setActiveDoorPushbullet1(bool active) {
  _activeDoorPushbullet1 = active;
}

bool CustomWiFiClient::isActiveDoorPushbullet1() {
  return _activeDoorPushbullet1;
}

void CustomWiFiClient::setApiKeyPushbullet2(String apiKey) {
  _apiKeyPushbullet2 = apiKey;
}

String CustomWiFiClient::getApiKeyPushbullet2() {
  return _apiKeyPushbullet2;
}

void CustomWiFiClient::setActiveRingPushbullet2(bool active) {
  _activeRingPushbullet2 = active;
}

bool CustomWiFiClient::isActiveRingPushbullet2() {
  return _activeRingPushbullet2;
}

void CustomWiFiClient::setActiveDoorPushbullet2(bool active) {
  _activeDoorPushbullet2 = active;
}

bool CustomWiFiClient::isActiveDoorPushbullet2() {
  return _activeDoorPushbullet2;
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
    Serial.println("google connection failed");
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
    Serial.println("google connection failed");
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
     Serial.println("pushbullet connection failed");
     return;
   }

   // konstrukce url
   String url = "/v2/pushes";

   Serial.print("Requesting URL: ");
   Serial.println(url);

   String postData = String("type=note&") + "title=" + title + "&" + "body=" + message;

	// spolehame na konkretni title pro urceni druhu notifikace
	bool activePushbullet1 = (title == "Door" && _activeDoorPushbullet1) || (title == "Doorbell" && _activeRingPushbullet1);
	bool activePushbullet2 = (title == "Door" && _activeDoorPushbullet2) || (title == "Doorbell" && _activeRingPushbullet2);

   // posli request(y)
   if (activePushbullet1) {
     Serial.println("Sending pushbullet1 request...");
     _client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                  "Host: " + _hostPushbullet + "\r\n" +
                  "Authorization: Bearer " + _apiKeyPushbullet1 + "\r\n" +
                  "Connection: close\r\n" +
                  "Content-Type: application/x-www-form-urlencoded;\r\n" +
                  "Content-Length: " + postData.length() + "\r\n" +
                  "\r\n" +
                  postData + "\n");
   }
   if (activePushbullet2) {
     Serial.println("Sending pushbullet2 request...");
     _client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                  "Host: " + _hostPushbullet + "\r\n" +
                  "Authorization: Bearer " + _apiKeyPushbullet2 + "\r\n" +
                  "Connection: close\r\n" +
                  "Content-Type: application/x-www-form-urlencoded;\r\n" +
                  "Content-Length: " + postData.length() + "\r\n" +
                  "\r\n" +
                  postData + "\n");
   }

   if (activePushbullet1 || activePushbullet2) {
     Serial.println("Pushbullet request sent... not waiting for response (do not block other operations)");
   }
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
