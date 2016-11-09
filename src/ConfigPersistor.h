/**************************************************************
Nacitani a ukladani konfiguracnich parametru
**************************************************************/
#include "Arduino.h"

class ConfigPersistor {
public:
  ConfigPersistor();

  void   init();
  String getPbApiKey1();
  void   setPbApiKey1(String apiKey);
  int    getPbActive1();
  void   setPbActive1(int active);
  String getPbApiKey2();
  void   setPbApiKey2(String apiKey);
  int    getPbActive2();
  void   setPbActive2(int active);
  void   persist();
private:
  void   write(int position, int length, String data);
  void   write(int position, int data);
  String read(int position, int length);
  int    read(int position);
};
