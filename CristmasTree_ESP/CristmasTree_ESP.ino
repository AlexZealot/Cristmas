//#include <WiFi.h>
//#include "mOTA.h"
#include "Button.h"
#include "WebPage.h"
#include "UDPInterface.h"
#include "LEDEffector.h"
#include "WiFiManager.h"

#define LED_PIN 15

//гирлянда - пин 18
#define BTN_UP      12
#define BTN_DOWN    11

Button btn_up(BTN_UP);
Button btn_down(BTN_DOWN);

void setup() {
  WiFiManager.init(true, LED_PIN, true, "XmasTree");


  ledEffector.init();

  if (WiFiManager.init(true, LED_PIN, true, "XmasTree") == WiFi_Connected){
    webPage = new WebPage();
    webPage->init();
  }

  udpInterface.init();
}

void loop() {
  btn_up.update(millis());
  btn_down.update(millis());

  if (btn_up.isPressed()){
    ledEffector.incMode();
  }
  if (btn_up.isDown() && (btn_up.holdInterval(millis()) > 2000)){
    ledEffector.setMode(MODE_OFF);
  }
  if (btn_up.isDown() && (btn_up.holdInterval(millis()) > 10000)){
    ESP.restart();
  }  

  if (btn_down.isPressed()){
    ledEffector.decMode();
  }
  if (btn_down.isDown() && (btn_down.holdInterval(millis()) > 2000)){
    ledEffector.setMode(MODE_OFF);
  }
  if (btn_down.isDown() && (btn_down.holdInterval(millis()) > 10000)){
    ESP.restart();
  }

  ledEffector.update(millis());

  if (WiFiManager.update() == WiFi_Connected){
    webPage->update();
    udpInterface.update(millis());
  }
}
