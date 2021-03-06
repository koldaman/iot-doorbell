/**************************************************************
Zabaleni funkcionality pro WebServer
**************************************************************/

#include <ESP8266WebServer.h>

class CustomWebServer {
public:
   CustomWebServer(const int port);
   void init(String pbKey1, int pbKeyActiveRing1, int pbKeyActiveDoor1, String pbKey2, int pbKeyActiveRing2, int pbKeyActiveDoor2);
   void handleClient();
   void saveCallback(void (*callback)());
   void ringCallback(void (*callback)());
private:
   void  (*_saveCallback)();
   void  (*_ringCallback)();
   ESP8266WebServer _server;
   String           _pbKey1;
   int              _pbKeyActiveRing1;
   int              _pbKeyActiveDoor1;
   String           _pbKey2;
   int              _pbKeyActiveRing2;
   int              _pbKeyActiveDoor2;
   void             handleIndex();
   void             save(String pbKey1, int pbKeyActiveRing1, int pbKeyActiveDoor1, String pbKey2, int pbKeyActiveRing2, int pbKeyActiveDoor2);
   void             handleRedirectHome();
};
