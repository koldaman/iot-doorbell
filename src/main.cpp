#include "Arduino.h"

#include <CustomWiFiManager.h>
#include <CustomWiFiClient.h>
#include <PinReader.h>
#include <Blink.h>

// zasilani informaci do cloudu
CustomWiFiClient client;

const int BELL_PIN = 4;   // D2
PinReader pinReader(BELL_PIN);

const int LED_PIN = 14;   // D5
Blink blinker(LED_PIN);

const int RELAY_PIN = 0;  // D3

int state = LOW;

// send report only once
bool reportSent = false;

long lastMillis = 0;

// maximum doorbell duration
const int maxInterval = 10*1000; // 10 seconds

void pinCallback(int oldValue, int newValue) {
   Serial.print("State changed: ");
   Serial.println(newValue);

   state = newValue;

   digitalWrite(RELAY_PIN, newValue);

   unsigned long currentMillis = millis();

   if (newValue == HIGH) {
      // HIGH - registr time
      lastMillis = currentMillis;
      reportSent = false;
   } else {
      // LOW - send statistic data
      int duration = currentMillis - lastMillis;
      String message = "Doorbell duration: ";
      message += duration;
      Serial.println(message);
      if (!reportSent) {
         client.sendData(duration);
         reportSent = true;
      } else {
         Serial.println("Ignoring long duration report, already reported");
      }
   }
}

void handleDataSent(int httpResult) {
   Serial.print("HttpResult: ");
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  pinReader.init();
  pinReader.setCallback(pinCallback);

  blinker.stop();

  client.sentCallback(handleDataSent);

  CustomWiFiManager::start(&blinker);
}

void loop() {
   pinReader.monitorChanges();

   unsigned long currentMillis = millis();

   // prevent long door bell active state
   if (state == HIGH && currentMillis - lastMillis >= maxInterval) {
      String message = "Max duration reached: ";
      message += currentMillis - lastMillis;
      Serial.println(message);
      pinCallback(state, LOW);
   }
}
