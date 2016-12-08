# iot-doorbell
Domestic doorbell automation.

Registers doorbell status changes (ringing/not ringing) and send status to the Cloud (Google sheet / Pushbullet push notification). Also monitors door state (opened / closed) and send this info as well.

Uses **ESP8266 (NodeMcu)** as microcontroller unit, utilitates its WiFi connection for simple HTTP requests.

To make it running, you need to supply API KEYs for the HTTP edpoints. It is done in **Constants.h** (not in GitHub repository):

```
class Constants {
public:
  static const char* BELL_MACRO_ID() { return "xxx"; };
  static const char* DOOR_MACRO_ID() { return "yyy"; };
};
```
