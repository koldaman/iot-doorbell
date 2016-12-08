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

const int BELL_PIN = 13;
PinReader pinReaderBell(BELL_PIN);

const int DOOR_PIN = 14;
PinReader pinReaderDoor(DOOR_PIN);

const int RELAY_PIN = 0;

const int LED_PIN = 2;
Blink blinker(LED_PIN);

const int INTERNAL_LED_PIN = 1;
Blink blinkerInternal(INTERNAL_LED_PIN);

int ringState = LOW;
int doorState = HIGH;

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

void stateChanged() {
   blinker.stop();
   if (ringState == HIGH) {
      // ringing
      blinker.init({1000}, 0); // no blink
   } else if (doorState == LOW) {
      // door opened
      blinker.init({10, 50}, 0); // blink quickly
   } else {
      // normal state
      blinker.init({100, 200, 100, 3000}, 0); // heartbeat
   }

   blinker.start();
}

void bellCallback(int oldValue, int newValue) {
   ringState = !newValue;

   Serial.print("Doorbell state changed: ");
   Serial.println(ringState ? "Ringing" : "Stopped");

   digitalWrite(RELAY_PIN, !ringState);

   unsigned long currentMillis = millis();

   if (ringState == HIGH) {
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
   stateChanged();
}

void doorCallback(int oldValue, int newValue) {
  doorState = newValue;

  Serial.print("Door state changed: ");
  const char* state = doorState ? "Closed" : "Opened";
  Serial.println(state);

  stateChanged();

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
      blinkerInternal.init({10}, 1);
   } else {
      Serial.println(" - FAILED");
      blinkerInternal.init({10,50,10,50,10}, 1);
   }

   blinkerInternal.start();
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
  bellCallback(HIGH, LOW);
  delay(300);
  bellCallback(LOW, HIGH);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);

  pinReaderBell.init();
  pinReaderBell.setCallback(bellCallback);

  pinReaderDoor.init();
  pinReaderDoor.setCallback(doorCallback);

  blinker.inverse(true);
  blinker.stop();

  blinkerInternal.inverse(true);
  blinkerInternal.stop();

  httpsClient.sentCallback(handleDataSent);

  CustomWiFiManager::start(&blinkerInternal);

  // load EEPROM configuration data
  configChanged();

  server.init(httpsClient.getApiKeyPushbullet1(), httpsClient.isActivePushbullet1(), httpsClient.getApiKeyPushbullet2(), httpsClient.isActivePushbullet2());
  server.saveCallback(configChanged);
  server.ringCallback(ringRequest);

  stateChanged();
}

void loop() {

  server.handleClient();

  pinReaderBell.monitorChanges();
  pinReaderDoor.monitorChanges();

  httpsClient.monitorState();

  unsigned long currentMillis = millis();

  // prevent long door bell active state
  if (ringState == HIGH && currentMillis - lastMillisHIGH >= maxInterval) {
    String message = "Max duration reached: ";
    message += currentMillis - lastMillisHIGH;
    Serial.println(message);
    bellCallback(!ringState, HIGH);
  }

  if (countRings() > 0 && ringState == LOW &&  currentMillis - lastMillisEvent >= sendDelay) {
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
