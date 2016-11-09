/**************************************************************
  Nacitani a ukladani konfiguracnich parametru
 **************************************************************/
#include "Arduino.h"

#include <EEPROM.h>
#include <ConfigPersistor.h>

ConfigPersistor::ConfigPersistor() {
}

void ConfigPersistor::init() {
  EEPROM.begin(512);
}

void ConfigPersistor::persist() {
  EEPROM.commit();
}

String ConfigPersistor::getPbApiKey1() {
  return read(80, 60);
}

void ConfigPersistor::setPbApiKey1(String apiKey) {
  return write(80, 60, apiKey);
}

int ConfigPersistor::getPbActive1() {
  return read(140);
}

void ConfigPersistor::setPbActive1(int active) {
  return write(140, active);
}

String ConfigPersistor::getPbApiKey2() {
  return read(150, 60);
}

void ConfigPersistor::setPbApiKey2(String apiKey) {
  return write(150, 60, apiKey);
}

int ConfigPersistor::getPbActive2() {
  return read(210);
}

void ConfigPersistor::setPbActive2(int active) {
  return write(210, active);
}

String ConfigPersistor::read(int position, int length) {
  String result;
  for (int i = position; i < position+length; ++i) {
    int tempData = EEPROM.read(i);
    if (tempData != 0) {
      result += char(tempData);
    }
  }
  return result;
}

int ConfigPersistor::read(int position) {
  int result = EEPROM.read(position);
  return result;
}

void ConfigPersistor::write(int position, int length, String data) {
  if (data.length() > length) {
    Serial.println("Unable to write String data to EEPROM, too long!");
    return;
  }

  // clear data
  for (int i = 0; i < length; ++i) {
    EEPROM.write(position + i, 0);
  }
  // write new one
  for (int i = 0; i < data.length(); ++i) {
    EEPROM.write(position + i, data[i]);
  }
}

void ConfigPersistor::write(int position, int data) {
  EEPROM.write(position, data);
}
