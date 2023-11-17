#include "Configuration.h"

#ifdef HALL_LEDS
  #define ONE_BTN
  #ifndef TV_LEDS
    #define USE_BME
  #endif
#endif

#ifdef TV_LEDS
  #define ONE_BTN
#endif

#include <Wire.h>

#ifdef USE_BME
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#endif

#include "WiFiManager.h"
#include "LEDEffector.h"
#include "Button.h"
#include "WebPage.h"
#include "UDPInterface.h"

#ifndef ESP32
  #define LED_PIN   2
#else
  #define LED_PIN   15
#endif

#define WF_DELAY_COUNT      600

#ifdef USE_BME
#define BME_UPD_INTERVAL    5000
Adafruit_BME280 bme;

unsigned long lastUpdBME;
#endif

char ota_name[64];

#ifdef ONE_BTN
  ButtonCB      btn(13);
#else
  #ifndef ESP32
    ButtonCB      btn(13);
    ButtonCB      btn2(14);
  #else
    ButtonCB      btn(7);
    ButtonCB      btn2(11);
  #endif
#endif

UDPInterface      udpInterface;

void setup() {
  ledEffector.init();    
  #ifndef ESP32
    sprintf(ota_name, OTA_NAME, ESP.getChipId());
  #else
    sprintf(ota_name, OTA_NAME, WiFiManager.getId());
  #endif

  #ifdef TV_LEDS
    ledEffector.setMode(MODE_HSV_DIF);
  #else
    ledEffector.setMode(MODE_OFF);
  #endif
  
  btn.setCallback(BTN_CB_PRESS, [](){
    ledEffector.incMode();
  });

  btn.setCallback([](uint32_t ms){
    if (ms > 2000)
      ledEffector.setMode(MODE_OFF);
    
    if (ms > 10000)
      ESP.restart();
  });

#ifndef ONE_BTN
  btn2.setCallback(BTN_CB_PRESS, [](){
    ledEffector.decMode();
  });

  btn2.setCallback([](uint32_t ms){
    if (ms > 2000)
      ledEffector.setMode(MODE_OFF);
    
    if (ms > 10000)
      ESP.restart();
  });
#endif

#ifdef USE_BME
  bme.begin(0x76);

  Temperature = bme.readTemperature();
  Humidity = bme.readHumidity();
  Pressure = bme.readPressure() / 1000.0f;
  lastUpdBME = millis();
#endif

  if (WiFiManager.init(true, LED_PIN, true, ota_name) == WiFi_Connected){
    webPage = new WebPage();
    udpInterface.init();
  }
}

void loop() {
#ifdef USE_BME  
  if (millis() - lastUpdBME >= BME_UPD_INTERVAL){
    Temperature = bme.readTemperature();
    Humidity = bme.readHumidity();
    Pressure = bme.readPressure() / 1000.0f;
    lastUpdBME = millis();
  }
#endif

  ledEffector.update(millis());
  btn.update();
#ifndef ONE_BTN
  btn2.update();
#endif

  if (WiFiManager.update() == WiFi_Connected){
    webPage->update();
    udpInterface.update();
  }

  //yield();
}