/**************************************************************
   Zabaleni funkcionality pro WebServer
 **************************************************************/
#include "Arduino.h"

#include "CustomWebServer.h"
#include "ESP8266WebServer.h"
#include "ConfigPersistor.h"

CustomWebServer::CustomWebServer(const int port) {
   ESP8266WebServer _server(port);
}

void CustomWebServer::saveCallback(void (*callback)()) {
   _saveCallback = callback;
}

void CustomWebServer::ringCallback(void (*callback)()) {
   _ringCallback = callback;
}

void CustomWebServer::handleClient() {
   _server.handleClient();
}

void CustomWebServer::save(String pbKey1, int pbKeyActive1, String pbKey2, int pbKeyActive2) {
   Serial.println("Saving data...");
   ConfigPersistor configPersistor;
   configPersistor.init();
   configPersistor.setPbApiKey1(pbKey1);
   configPersistor.setPbActive1(pbKeyActive1);
   configPersistor.setPbApiKey2(pbKey2);
   configPersistor.setPbActive2(pbKeyActive2);
   configPersistor.persist();
   if (_saveCallback) {
     _saveCallback();
   }
}

void CustomWebServer::handleRedirectHome() {
  String html = "\
    <!doctype html>\
    <html class='no-js' lang=''>\
    <head>\
      <script type='text/javascript'>\
        function redirectHome() {\
          window.location = '.';\
        }\
      </script>\
      <title>Redirection</title>\
    </head>\
    <body onLoad='redirectHome()'>\
      <p>Performing requested action, you will be redirected...</p>\
      <p>Click <a href='#' onclick='redirectHome()'>here</a> to redirect manualy.</p>\
    </body>\
    </html>\
  ";

  _server.send(200, "text/html", html);
}

void CustomWebServer::handleIndex() {
  if (_server.args() > 0) {
    if (_server.arg("pbKey1").length() > 0) {
      String pbKey1 = _server.arg("pbKey1");
      String pbKeyActive1 = _server.arg("pbKeyActive1");
      String pbKey2 = _server.arg("pbKey2");
      String pbKeyActive2 = _server.arg("pbKeyActive2");
      Serial.println("PbKey1: " + pbKey1 + ", PbKeyActive1: " + pbKeyActive1);
      Serial.println("PbKey2: " + pbKey2 + ", PbKeyActive2: " + pbKeyActive2);
      _pbKey1 = pbKey1;
      _pbKeyActive1 = pbKeyActive1.length() == 0 ? 0 : atoi(pbKeyActive1.c_str());
      _pbKey2 = pbKey2;
      _pbKeyActive2 = pbKeyActive2.length() == 0 ? 0 : atoi(pbKeyActive2.c_str());
      save(_pbKey1, _pbKeyActive1, _pbKey2, _pbKeyActive2);
    }
    if (_server.arg("bell").length() > 0) {
      if (_ringCallback) {
        _ringCallback();
      }
    }
    // redirect to home page without actions specified (prevednt double save / ring etc.)
    handleRedirectHome();
    return;
  }

  String html = "\
  <!doctype html>\
  <html class='no-js' lang=''>\
      <head>\
          <meta charset='utf-8'>\
          <meta http-equiv='x-ua-compatible' content='ie=edge'>\
          <title>Doorbell IoT</title>\
          <meta name='description' content=''>\
          <meta name='viewport' content='width=device-width, initial-scale=1'>\
          <link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/normalize/5.0.0/normalize.min.css'>\
          <link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.6.3/css/font-awesome.min.css'>\
          <link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.6.3/fonts/fontawesome-webfont.eot'>\
          <link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.6.3/fonts/fontawesome-webfont.svg'>\
          <link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.6.3/fonts/fontawesome-webfont.ttf'>\
          <link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.6.3/fonts/fontawesome-webfont.woff'>\
          <link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.6.3/fonts/fontawesome-webfont.woff2'>\
          <link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.6.3/fonts/FontAwesome.otf'>\
          <link rel='stylesheet' href='css/main.css'>\
          <script src='https://cdnjs.cloudflare.com/ajax/libs/modernizr/2.8.3/modernizr.min.js'></script>\
  		<style>\
           body {\
              background-color: rgba(0,0,0,.05);\
              font-family: 'Helvetica Neue',Helvetica,Arial,sans-serif;\
           }\
           h1 {\
              color: rgba(255,255,255,.9);\
  	          text-shadow: rgba(0,0,0,.2) 0px -1px 0px;\
              background-color: #2d353f;\
              padding: 5px 0;\
              margin-top: 0;\
  				    font-size: 2em;\
  				    text-align: center;\
  			   }\
           .container {\
              padding: 0 1em;\
           }\
           .action {\
  				    padding: 0.5em;\
              font-size: 1.2em;\
              text-align: center;\
  			   }\
           .action a {\
              color: rgba(0,0,0,.5);\
  			   }\
  		</style>\
      </head>\
      <body>\
          <h1 class='header'>Doorbell IoT</h1>\
          <div class='container'>\
            <form action='' method='get'>\
              <fieldset>\
                <legend>Pushbbullet notification</legend>\
                <p>API key 1:<br>\
                <input type='text' name='pbKey1' value='";
  html += _pbKey1;
  html += "'> &nbsp; <input type='checkbox' name='pbKeyActive1' value='1'";
  html += _pbKeyActive1 == 1 ? " checked" : "";
  html += "\
                > active</p>\
                <p>API key 2:<br>\
                <input type='text' name='pbKey2' value='";
  html += _pbKey2;
  html += "'> &nbsp; <input type='checkbox' name='pbKeyActive2' value='1'";
  html += _pbKeyActive2 == 1 ? " checked" : "";
  html += "\
                > active</p>\
                <input type='submit' value='Save'>\
              </fieldset>\
            </form>\
       </div>\
       <div class='action'><a href='?bell=1'><i class='fa fa-bell' aria-hidden='true'></i> Ring doorbell</a></div>\
      </body>\
  </html>\
  ";

  _server.send(200, "text/html", html);
}

void CustomWebServer::init(String pbKey1, int pbKeyActive1, String pbKey2, int pbKeyActive2) {
  _pbKey1 = pbKey1;
  _pbKeyActive1 = pbKeyActive1;
  _pbKey2 = pbKey2;
  _pbKeyActive2 = pbKeyActive2;

   std::function<void ()> handleIndex = [&]{ CustomWebServer::handleIndex(); };
   _server.on("/", handleIndex);
   _server.begin();
   Serial.println("HTTP server started");
}
