# iot-doorbell
Domestic doorbell automation.

Registers doorbell status changes (ringing/not ringing) and send status to the Cloud (Google sheet / Pushbullet push notification). Also monitors door state (opened / closed) and send this info as well.

Uses ESP8266 (NodeMcu) as mircocontroller unit, utilitates its WiFi connection for simple HTTP requests.
