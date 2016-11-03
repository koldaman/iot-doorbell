/**************************************************************
Zabaleni funkcionality pro WiFiClient (odesilani dat pres HTTPS)
**************************************************************/

#include <WiFiClientSecure.h>

class CustomWiFiClient {
public:
  CustomWiFiClient();

  void sendDataPushbullet(String title, String message);
  void sendBellDataGoogle(int duration);
  void sendDoorDataGoogle(const char* state);
  void sentCallback(void (*callback)(int httpStatus));
  void monitorState();
private:
  // Pushbullet
  const char* _hostPushbullet;
  int   _httpPortPushbullet;
  const char* _apiKeyPushbullet;

  // Google
  const char* _hostGoogle;
  int   _httpPortGoogle;
  const char* _googleScriptBellMacroId;
  const char* _googleScriptDoorMacroId;

  void  (*_callback)(int httpResult);
  WiFiClientSecure _client;
  unsigned long _timeout;
  bool _requestRunning;
  int parseHttpResult(String httpResultString);
};
