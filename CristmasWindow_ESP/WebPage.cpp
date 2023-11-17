#include "WebPage.h"
#ifdef HALL_LEDS
#include "WebText.h"
#else
#ifdef TV_LEDS
#include "WebText_tv.h"
#else
#include "WebText_red.h"
#endif
#endif
#include <Arduino.h>

WebPage* webPage;

float Temperature = 0.0f;
float Humidity = 0.0f;
float Pressure = 0.0f;

uint8_t up_s;
uint8_t up_m;
uint32_t up_time;

char thp_line[512];

WebPage::WebPage()
  : server(80) {
  server.on("/", HTTP_GET, [this]() {
    String oStr = FPSTR(wpi_html);
    for (uint8_t i = 0; i < MODES_COUNT; i++) {
      if (ledEffector.getMode() == i) oStr += "<option selected>";
      else oStr += "<option>";
      oStr += ledEffector.getModeName(i);
      oStr += "</option>";
    }
    oStr += FPSTR(wpi_html_mid);

    up_time = millis() / 1000;
    up_s = up_time % 60;
    up_time /= 60;
    up_m = up_time % 60;
    up_time /= 60;

#ifdef NO_THP
    sprintf(thp_line, no_THP_html, ledEffector.getCurrent(), ledEffector.getBrightness(), up_time, up_m, up_s);
#else
      sprintf(thp_line, THP_html, Temperature, Humidity, Pressure * 7.501f, (Pressure * 7.501f > 746.0f) ? "&uarr;" : "&darr;", ledEffector.getCurrent(), ledEffector.getBrightness(), up_time, up_m, up_s);
#endif
    oStr += thp_line;
    oStr += FPSTR(wpi_html_end);
    server.send(200, "text/html", oStr);
  });

  server.on("/refT", [this]() {
    up_time = millis() / 1000;
    up_s = up_time % 60;
    up_time /= 60;
    up_m = up_time % 60;
    up_time /= 60;

#ifdef NO_THP
    sprintf(thp_line, no_THP_html, ledEffector.getCurrent(), ledEffector.getBrightness(), up_time, up_m, up_s);
#else
      sprintf(thp_line, THP_html, Temperature, Humidity, Pressure * 7.501f, (Pressure * 7.501f > 746.0f) ? "&uarr;" : "&darr;", ledEffector.getCurrent(), ledEffector.getBrightness(), up_time, up_m, up_s);
#endif
    server.send(200, "text/html", thp_line);
  });

  server.on("/setMode", [this]() {
    String modeName = server.arg("mode");
    ledEffector.setMode(ledEffector.getModeByName(modeName.c_str()));
    server.send(404, "text/html", "");
  });

  server.on("/refMode", [this]() {
    if (ledEffector.getModeChanged()) {
      String oStr = "";
      for (uint8_t i = 0; i < MODES_COUNT; i++) {
        if (ledEffector.getMode() == i) {
          oStr += "<option selected>";
        } else {
          oStr += "<option>";
        }
        oStr += ledEffector.getModeName(i);
        oStr += "</option>\n\r";
      }
      server.send(200, "text/html", oStr);
    } else
      server.send(404, "text/html", "");
  });

  lastRestart = millis();
  server.begin();
}

void WebPage::update() {
  server.handleClient();
}