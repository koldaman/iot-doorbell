/**************************************************************
Zabaleni funkcionality pro WiFiManager (zapamatovani
posledniho nastaveni pripojeni k WiFi vcetne prenastaveni)
**************************************************************/

#include <DNSServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <Blink.h>

class CustomWiFiManager {
public:
   static void start(Blink* blinker);
private:
  static WiFiManager _wifiManager;
  static Blink* _blinker;
  static void configModeCallback(WiFiManager *myWiFiManager);
};
