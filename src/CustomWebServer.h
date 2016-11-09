/**************************************************************
Zabaleni funkcionality pro WebServer
**************************************************************/

#include <ESP8266WebServer.h>

class CustomWebServer {
public:
   CustomWebServer(const int port);
   void init(String pbKey1, int pbKeyActive1, String pbKey2, int pbKeyActive2);
   void handleClient();
   void saveCallback(void (*callback)());
   void ringCallback(void (*callback)());
private:
   void  (*_saveCallback)();
   void  (*_ringCallback)();
   ESP8266WebServer _server;
   String           _pbKey1;
   int              _pbKeyActive1;
   String           _pbKey2;
   int              _pbKeyActive2;
   void             handleIndex();
   void             save(String pbKey1, int pbKeyActive1, String pbKey2, int pbKeyActive2);
   void             handleRedirectHome();
};
