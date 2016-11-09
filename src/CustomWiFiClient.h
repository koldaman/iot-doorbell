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
  void setApiKeyPushbullet1(String apiKey);
  String getApiKeyPushbullet1();
  void setActivePushbullet1(bool active);
  bool isActivePushbullet1();
  void setApiKeyPushbullet2(String apiKey);
  String getApiKeyPushbullet2();
  void setActivePushbullet2(bool active);
  bool isActivePushbullet2();
private:
  // Pushbullet
  const char* _hostPushbullet;
  int         _httpPortPushbullet;
  String      _apiKeyPushbullet1;
  bool        _activePushbullet1;
  String      _apiKeyPushbullet2;
  bool        _activePushbullet2;

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
