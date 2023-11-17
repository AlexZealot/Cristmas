#include "pgmspace.h"
#include "WebPage.h"
#include "WebText.h"

#include "LEDEffector.h"
#include "UDPInterface.h"

WebPage   *webPage;
uint8_t up_s;
uint8_t up_m;
uint32_t up_time;

char add_line[512];

void  WebPage::init(){
  server.on("/", HTTP_GET, [this]() {
    String oStr = FPSTR(wpi_html);
    for (uint8_t i = 0; i < MODES_COUNT; i++){
      if (ledEffector.getMode() == i) oStr += "<option selected>";
      else                            oStr += "<option>";
      oStr += ledEffector.getModeName(i);
      oStr += "</option>";
    }
    oStr += FPSTR(wpi_html_mid);

    if (udpInterface.hallSync) oStr += "<input type=\"checkbox\" id=\"hall\" value=\"В зале\" onChange=\"changeSync(this)\" checked />В зале<br><br>";
    else                       oStr += "<input type=\"checkbox\" id=\"hall\" value=\"В зале\" onChange=\"changeSync(this)\" />В зале<br><br>";

    if (udpInterface.childrenSync)  oStr += "<input type=\"checkbox\" id=\"children\" value=\"В детской\" onChange=\"changeSync(this)\" checked />В детской<br><br>";
    else                            oStr += "<input type=\"checkbox\" id=\"children\" value=\"В детской\" onChange=\"changeSync(this)\" />В детской<br><br>";

    if (udpInterface.tvSync)        oStr += "<input type=\"checkbox\" id=\"tv\" value=\"Подсветка ТВ\" onChange=\"changeSync(this)\" checked />Подсветка ТВ";
    else                            oStr += "<input type=\"checkbox\" id=\"tv\" value=\"Подсветка ТВ\" onChange=\"changeSync(this)\" />Подсветка ТВ";    

    oStr += FPSTR(wpi_html_mid2);

    up_time = millis()/1000;
    up_s = up_time % 60;
    up_time /= 60;
    up_m = up_time % 60;
    up_time /= 60;

    sprintf_P(add_line, no_THP_html, ledEffector.getCurrent(), ledEffector.getBrightness(), up_time, up_m, up_s);
    oStr += add_line;
    oStr += FPSTR(wpi_html_end);
    server.send(200, "text/html", oStr);
  });

  server.on("/refT", [this](){
    up_time = millis()/1000;
    up_s = up_time % 60;
    up_time /= 60;
    up_m = up_time % 60;
    up_time /= 60;

    sprintf_P(add_line, no_THP_html, ledEffector.getCurrent(), ledEffector.getBrightness(), up_time, up_m, up_s);
    server.send(200, "text/html", add_line);
  });

  server.on("/refMode", [this](){
    String oStr = "";
    for (uint8_t i = 0; i < MODES_COUNT; i++){
      if (ledEffector.getMode() == i){
        oStr += "<option selected>";
      } else {
        oStr += "<option>";
      }
      oStr += ledEffector.getModeName(i);
      oStr += "</option>\n\r";
    }
    if (ledEffector.getModeChanged())
      server.send(200, "text/html", oStr);
    else
      server.send(404, "text/html", "");
  });

  server.on("/setMode",[this](){
    String modeName = server.arg("mode");
    ledEffector.setMode(ledEffector.getModeByName(modeName.c_str()));
    server.send(404, "text/html", "");
  });  

  server.on("/sync", [this](){
    String  strID = server.arg("id");
    String  strVal = server.arg("sync");

    if (strID == "hall"){
      if (strVal == "0") udpInterface.hallSync = false;
      if (strVal == "1") udpInterface.hallSync = true;
    }

    if (strID == "children"){
      if (strVal == "0") udpInterface.childrenSync = false;
      if (strVal == "1") udpInterface.childrenSync = true;
    }    

    if (strID == "tv"){
      if (strVal == "0") udpInterface.tvSync = false;
      if (strVal == "1") udpInterface.tvSync = true;
    }    
  });

  server.begin();
}

void  WebPage::update(){
  server.handleClient();
}