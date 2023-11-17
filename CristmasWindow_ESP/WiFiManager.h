#ifndef __WIFI_MANAGER__
#define __WIFI_MANAGER__
#include <Arduino.h>
#include <ArduinoOTA.h>

#ifndef ESP8266
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

//Читать ли SSID и PASS из EEPROM
#define EEPROM_WIFI

#ifdef EEPROM_WIFI
  #include <EEPROM.h>
  #ifndef ESP8266
    #include <WebServer.h>
  #else
    #include <ESP8266WebServer.h>
  #endif
  #define WIFI_SSID_OFFSET      0     //Где в EEPROM хранится SSID
  #define WIFI_SSID_SIZE        32    //Сколько байт занимает SSID
  #define WIFI_PASS_OFFSET      WIFI_SSID_OFFSET+WIFI_SSID_SIZE //Где в EEPROM хранится пароль от WiFi
  #define WIFI_PASS_SIZE        16                              //Сколько байт он занимает
  #define EEPROM_SIZE           512

  #define AP_SSID               "ESP%u"     //SSID точки доступа, если SSID из EEPROM не доступен
  #define AP_PASS               "00000000"  //Пароль от точки доступа

  #define CONNECT_DELAY         20000       //Сколько пытаться соединиться с точкой доступа, указанной пользователем
  
  #define WFM_AP_LIVE_TIME      600000      //Время жизни точки доступа. После этого периода времени ESP будет перезагружена. По умолчанию 1000 мс * 60 с * 10мин = 10 минут
#else
  #define WIFI_SSID             "Your_SSID" //Для намертво приколоченного WiFi
  #define WIFI_PASS             "Your_PASS"
#endif

#define WF_DELAY_COUNT        600   

enum  wifiInitResult_t {
  WiFi_NotConnected = 0,
  WiFi_Connected = 1
};

class WiFiManagerClass {
  public:
    WiFiManagerClass();
    ~WiFiManagerClass();
  
    //инициализация менеджера
    wifiInitResult_t  init(bool useLedPin, uint8_t led_pin, bool useOTA, const char * OTAhostname);
    //вызывать в loop каждую итерацию. Обработка OTA и WebServer
    wifiInitResult_t  update();

    #ifdef ESP32
      uint32_t          getId();
    #endif
  private:
    wifiInitResult_t  _initResult;
    bool              _useOTA;

    uint32_t          _ap_started;

    void              WiFiBegin(bool useLedPin, uint8_t led_pin);
    #ifndef ESP32
      uint32_t          getId();
    #endif    

  #ifdef EEPROM_WIFI
    char      WIFI_SSID[WIFI_SSID_SIZE];
    char      WIFI_PASS[WIFI_PASS_SIZE];
    int       _scanResult;
    bool      _useLedPin;
    uint8_t   _led_pin;

    #ifndef ESP8266
      WebServer         server;
    #else
      ESP8266WebServer  server;
    #endif
  #endif
};

extern WiFiManagerClass   WiFiManager;

#endif //__WIFI_MANAGER__