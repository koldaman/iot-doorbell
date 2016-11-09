#include "Arduino.h"

#include <CustomWiFiManager.h>
#include <CustomWiFiClient.h>
#include <CustomWebServer.h>
#include <ConfigPersistor.h>
#include <PinReader.h>
#include <Blink.h>

// send HTTP requests to cloud
CustomWiFiClient httpsClient;

// HTTP server
CustomWebServer server(80);

const int BELL_PIN = 4;   // D2
PinReader pinReaderBell(BELL_PIN);

const int DOOR_PIN = 5;   // D1
PinReader pinReaderDoor(DOOR_PIN);

const int RELAY_PIN = 0;  // D3

const int LED_PIN = 14;   // D5
Blink blinker(LED_PIN);

int state = LOW;

long lastMillisHIGH = 0;
long lastMillisEvent = 0;

int ringCollector[] = {0,0,0,0,0,0,0,0,0,0};

// maximum doorbell duration
const int maxInterval = 10*1000; // 10 seconds

// delay of sending statistic data
const int sendDelay = 2*1000; // 2 seconds

bool printRings() {
   Serial.print("{");
   for (int i = 0; i < 10; i++) {
      Serial.print(ringCollector[i]);
      Serial.print(",");
   }
   Serial.println("}");
}

int countRings() {
  int count = 0;
   for (int i = 0; i < 10; i++) {
      count += ringCollector[i] > 0 ? 1 : 0;
   }
   return count;
}

void addRing(int duration) {
   if (duration > maxInterval) { // ignoring long ringing - suppose we'ha already sent it
     return;
   }
   for (int i = 0; i < 10; i++) {
      if (ringCollector[i] == 0) {
         ringCollector[i] = duration;
         break;
      }
   }
}

void clearRings() {
   for (int i = 0; i < 10; i++) {
      ringCollector[i] = 0;
   }
}

void bellCallback(int oldValue, int newValue) {
   Serial.print("Doorbell state changed: ");
   Serial.println(newValue ? "Ringing" : "Stopped");

   state = newValue;

   digitalWrite(RELAY_PIN, newValue);

   unsigned long currentMillis = millis();

   if (newValue == HIGH) {
      // HIGH - registr time
      lastMillisHIGH = currentMillis;
   } else {
      // LOW - register statistic data
      int duration = currentMillis - lastMillisHIGH;
      String message = "Doorbell duration: ";
      message += duration;
      Serial.println(message);
      addRing(duration);
      printRings();
   }
   lastMillisEvent = currentMillis;
}

void doorCallback(int oldValue, int newValue) {
  Serial.print("Door state changed: ");
  const char* state = newValue ? "Closed" : "Opened";
  Serial.println(state);

  // send push notification to Pushbullet
  httpsClient.sendDataPushbullet("Door", state);

  // send statistic data to Google
  httpsClient.sendDoorDataGoogle(state);
}

void handleDataSent(int httpResult) {
   Serial.print("Google HttpResult: ");
   Serial.print(httpResult);
   if (httpResult >= 200 && httpResult < 300) {
      Serial.println(" - OK");
      blinker.init({10}, 1);
   } else {
      Serial.println(" - FAILED");
      blinker.init({10,50,10,50,10}, 1);
   }

   blinker.start();
}

// when config get saved to EEPROM
void configChanged() {
  Serial.println("Loading config data from EEPROM...");
  ConfigPersistor configPersistor;
  configPersistor.init();
  httpsClient.setApiKeyPushbullet1(configPersistor.getPbApiKey1());
  httpsClient.setActivePushbullet1(configPersistor.getPbActive1());
  httpsClient.setApiKeyPushbullet2(configPersistor.getPbApiKey2());
  httpsClient.setActivePushbullet2(configPersistor.getPbActive2());
}

// manual request for ring a bell
void ringRequest() {
  Serial.println("Doorbell ring request from web server...");
  bellCallback(LOW, HIGH);
  delay(300);
  bellCallback(HIGH, LOW);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  pinReaderBell.init();
  pinReaderBell.setCallback(bellCallback);

  pinReaderDoor.init();
  pinReaderDoor.setCallback(doorCallback);

  blinker.stop();

  httpsClient.sentCallback(handleDataSent);

  CustomWiFiManager::start(&blinker);

  // load EEPROM configuration data
  configChanged();

  server.init(httpsClient.getApiKeyPushbullet1(), httpsClient.isActivePushbullet1(), httpsClient.getApiKeyPushbullet2(), httpsClient.isActivePushbullet2());
  server.saveCallback(configChanged);
  server.ringCallback(ringRequest);
}

void loop() {

  server.handleClient();

  pinReaderBell.monitorChanges();
  pinReaderDoor.monitorChanges();

  httpsClient.monitorState();

  unsigned long currentMillis = millis();

  // prevent long door bell active state
  if (state == HIGH && currentMillis - lastMillisHIGH >= maxInterval) {
    String message = "Max duration reached: ";
    message += currentMillis - lastMillisHIGH;
    Serial.println(message);
    bellCallback(state, LOW);
  }

  if (countRings() > 0 && state == LOW &&  currentMillis - lastMillisEvent >= sendDelay) {
    printRings();

    // send push notification to Pushbullet
    String message = "Ringing ";
    message += countRings();
    message += "x...";
    httpsClient.sendDataPushbullet("Doorbell", message);

    lastMillisEvent = 0;
    for (int i = 0; i < 10; ++i) {
      if (ringCollector[i] == 0) {
        break;
      }
      // send statistic data to Google
      httpsClient.sendBellDataGoogle(ringCollector[i]);
      if (ringCollector[i] == maxInterval) {
        Serial.println("Max ringing interval reached");
      }
    }
    clearRings();
  }

}
