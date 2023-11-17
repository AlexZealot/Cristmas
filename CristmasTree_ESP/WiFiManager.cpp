#include "WiFiManager.h"

WiFiManagerClass   WiFiManager;

#ifdef EEPROM_WIFI
const char html_page [] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Init page</title>
    <link rel="icon" href="data:,">
    <style>
      html {font-family: Arial; display: inline-block; text-align: center; color: #FFFFFF}
      body {max-width: 400px; margin:0px auto; padding-bottom: 25px; background-color: #000000}
      select {height: 30px; border: solid; border-width: 1px; border-radius: 3px; width: 300px}
      div {border: solid; border-color: #ffffff; border-width: 1px; border-radius: 10px; padding-left: 10px; padding-bottom: 10px; margin: 10px}
    </style>
    <script>
      function okFunc(element){
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/UPD?ssid="+document.getElementById("ssid").value+"&pass="+document.getElementById("pass").value, true);
        xhr.send();
      }
    </script>
  </head>
  <body>
    <h1>Network params</h1>
    <div align=right>
      <label><p>SSID:   <select id="ssid">
)rawliteral";

const char html_page_end [] PROGMEM = R"rawliteral(
      </select></label>
      <label><p>Pasword:  <input type="text" id="pass"></label>
    </div>      
    <label><p><button onclick="okFunc(this)">  Ok  </button></label>
  </body>
</html>  
)rawliteral";
#endif

WiFiManagerClass::WiFiManagerClass(): 
  _initResult(WiFi_NotConnected),
  _useOTA(false)
#ifdef EEPROM_WIFI
  ,server(80)
#endif
{}

WiFiManagerClass::~WiFiManagerClass(){}

uint32_t          WiFiManagerClass::getId(){
#ifndef ESP8266
  uint32_t chipID = 0;
	for(int i=0; i<17; i=i+8) {
	  chipID |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
	}
  return chipID;
#else
  return ESP.getChipId();
#endif
}

void              WiFiManagerClass::WiFiBegin(bool useLedPin, uint8_t led_pin){
  WiFi.mode(WIFI_STA);
#ifdef ESP8266
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
#else
  WiFi.setSleep(WIFI_PS_NONE);
#endif  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  uint16_t wfDelay = 0;
  while(WiFi.status() != WL_CONNECTED){
    if (useLedPin){
      delay(100);
      digitalWrite(led_pin, 0);
      delay(100);
      digitalWrite(led_pin, 1);
    } else
      delay(200);
    wfDelay++;

    if (wfDelay > WF_DELAY_COUNT)//минуту не могу подключиться. Скажем своё фи
      ESP.restart();
  }

  #ifdef ESP32
  if (useLedPin)
    digitalWrite(led_pin, 0);
  #endif
}

wifiInitResult_t  WiFiManagerClass::init(bool useLedPin, uint8_t led_pin, bool useOTA, const char * OTAhostname){
  _useLedPin = useLedPin;
  _led_pin = led_pin;
  if (useLedPin){
    pinMode(led_pin, OUTPUT);
  }
  _useOTA = useOTA;
#ifdef EEPROM_WIFI
  //Читаем из EEPROM
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(WIFI_SSID_OFFSET, WIFI_SSID);
  EEPROM.get(WIFI_PASS_OFFSET, WIFI_PASS);
  EEPROM.end();

  _scanResult = WiFi.scanNetworks(false, true);
  String ssid;
  int32_t rssi;
  uint8_t encryptionType;
  uint8_t *bssid;
  int32_t channel;
  #ifdef ESP8266
  bool hidden;
  #endif

  if (strlen(WIFI_SSID) > 1){
    for (int i = 0; i < _scanResult; i++) {
      WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel
      #ifdef ESP8266
        , hidden
      #endif
      );
      if (ssid == WIFI_SSID) {
        _initResult = WiFi_Connected;
        break;
      }
    }
  }

  switch (_initResult){
    case WiFi_Connected:
      WiFiBegin(useLedPin, led_pin);
    break;

    case WiFi_NotConnected:
      char AP_NAME[64];
      sprintf(AP_NAME, AP_SSID, getId());
      WiFi.softAP(AP_NAME, AP_PASS);

      IPAddress myIP = WiFi.softAPIP();
//      wifiWebPage = new WifiWebPage(scanResult);

      server.on("/", [this](){
        String tmp = FPSTR(html_page);

        String ssid;
        int32_t rssi;
        uint8_t encryptionType;
        uint8_t *bssid;
        int32_t channel;
        #ifdef ESP8266
        bool hidden;
        #endif
    
        for (int8_t i = 0; i < _scanResult; i++){
          WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel
          #ifdef ESP8266
            , hidden
          #endif
          );
          if (ssid != ""){
            tmp += "\t\t<option>";
            tmp += ssid;
            tmp += "</option>\n";
          }
        }
    
        tmp += FPSTR(html_page_end);
        server.send(200, "text/html", tmp);
      });

      server.on("/UPD", [this](){
        String  ssid_str = server.arg("ssid");
        String  pass_str = server.arg("pass");
      
        //пробуем подключиться
        uint32_t tick = millis();
        WiFi.begin(ssid_str.c_str(), pass_str.c_str());

        //мигаем
        while( WiFi.status() != WL_CONNECTED ){
          if (_useLedPin){
            delay(100);
            digitalWrite(_led_pin, 0);
            delay(100);
            digitalWrite(_led_pin, 1);
          } else
            delay(200);

          if (millis() - tick > CONNECT_DELAY){
            ESP.restart();
          }
        }

        memset(WIFI_SSID, 0, WIFI_SSID_SIZE);
        memset(WIFI_PASS, 0, WIFI_PASS_SIZE);

        for (uint32_t i = 0; i < ssid_str.length(); i++){
          WIFI_SSID[i] = ssid_str.c_str()[i];
        }

        for (uint32_t i = 0; i < pass_str.length(); i++){
          WIFI_PASS[i] = pass_str.c_str()[i];
        }

        EEPROM.begin(EEPROM_SIZE);
        EEPROM.put(WIFI_SSID_OFFSET, WIFI_SSID);
        EEPROM.commit();
        EEPROM.put(WIFI_PASS_OFFSET, WIFI_PASS);
        EEPROM.commit();
        EEPROM.end();

        ESP.restart();        
      });

      server.begin();
      _ap_started = millis();
    break;
  }
#else
  WiFiBegin(useLedPin, led_pin);

  _initResult = WiFi_Connected;
#endif

  if (useOTA && (_initResult == WiFi_Connected)){
    ArduinoOTA.setHostname(OTAhostname);
    ArduinoOTA.begin();     
  }  

  return _initResult;
}

wifiInitResult_t  WiFiManagerClass::update(){
#ifdef EEPROM_WIFI
  if ((_initResult == WiFi_Connected) && _useOTA){
    ArduinoOTA.handle();
  }

  if (_initResult == WiFi_NotConnected){
    server.handleClient();

    if (millis() - _ap_started > WFM_AP_LIVE_TIME)
      ESP.restart();
  }
#else
  if ((_initResult == WiFi_Connected) && _useOTA){
    ArduinoOTA.handle();
  }
#endif

  return _initResult;
}