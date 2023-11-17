#ifndef __WEB_PAGE__
#define __WEB_PAGE__

#include "Configuration.h"

#ifndef HALL_LEDS
#define NO_THP
#endif

#ifndef ESP32
  #include <ESP8266WebServer.h>
  #include <ESP8266WiFi.h>
#else
  #include <WebServer.h>
  #include <WiFi.h>
#endif
#include <Arduino.h>
#include "LEDEffector.h"

#define   RESTART_INTERVAL    600000

class WebPage {
  public:
    WebPage();

    #ifndef ESP32
      ESP8266WebServer  server; 
    #else
      WebServer server;
    #endif

    void    update();
  private:
    uint32_t  lastRestart;  
};

extern WebPage * webPage;
extern float   Temperature;
extern float   Humidity;
extern float   Pressure;

#endif 
