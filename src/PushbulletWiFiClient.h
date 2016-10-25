/**************************************************************
Zabaleni funkcionality pro WiFiClient (odesilani dat pres HTTPS)
**************************************************************/

#include <WiFiClientSecure.h>

class PushbulletWiFiClient {
public:
   PushbulletWiFiClient();
   void init();
   void sendData(String title, String message);
private:
   char* _host;
   int   _httpPort;
   char* _apiKey;
   WiFiClientSecure _client;
   double getVbat();
};
