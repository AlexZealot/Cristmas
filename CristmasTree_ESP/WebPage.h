#ifndef __WEB_PAGE__
#define __WEB_PAGE__

#include <WiFi.h>
#include <WebServer.h>

class WebPage {
  public:
    WebServer    server;
    WebPage(): server(80){};
    ~WebPage(){};

    void  init();
    void  update();
};

extern WebPage   *webPage;

#endif 
