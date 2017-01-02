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
  void setActiveRingPushbullet1(bool active);
  bool isActiveRingPushbullet1();
  void setActiveDoorPushbullet1(bool active);
  bool isActiveDoorPushbullet1();
  void setApiKeyPushbullet2(String apiKey);
  String getApiKeyPushbullet2();
  void setActiveRingPushbullet2(bool active);
  bool isActiveRingPushbullet2();
  void setActiveDoorPushbullet2(bool active);
  bool isActiveDoorPushbullet2();
private:
  // Pushbullet
  const char* _hostPushbullet;
  int         _httpPortPushbullet;
  String      _apiKeyPushbullet1;
  bool        _activeRingPushbullet1;
  bool        _activeDoorPushbullet1;
  String      _apiKeyPushbullet2;
  bool        _activeRingPushbullet2;
  bool        _activeDoorPushbullet2;

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
