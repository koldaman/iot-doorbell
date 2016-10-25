/**************************************************************
Zabaleni funkcionality pro WiFiClient (odesilani dat pres HTTPS)
**************************************************************/

#include <WiFiClientSecure.h>

class GoogleScriptWiFiClient {
public:
   GoogleScriptWiFiClient();

   void sendData(int duration);
   void sentCallback(void (*callback)(int httpStatus));
private:
   char* _host;
   int   _httpPort;
   char* _googleScriptMacroId;
   WiFiClientSecure _client;
   void  (*_callback)(int httpResult);
   int parseHttpResult(String httpResultString);
};
