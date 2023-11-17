#ifndef __UDP_INTERFACE__
#define __UDP_INTERFACE__

#include <WiFiUDP.h>
#include "Configuration.h"

#define UDP_PORT        1234

//заголовки
#define UDP_DP_SYNC     0x01
#define UDP_DP_MODE     0x02
#define UDP_DISPLAY     0x03

#define DISPLAY_RESEND_INTERVAL   2000 //как часто отправлять данные о своём режиме на дисплей
#define DESPLAY_RECIEVE_INTERVAL  30000

//Сформировать UDP_ID
#ifdef HALL_LEDS
  #define UDP_ID  UDP_HALL_WINDOW
#else
  #ifdef TV_LEDS
    #define UDP_ID  UDP_HALL_TV
  #else
    #define UDP_ID  UDP_CHILDREN
  #endif
#endif

#ifndef UDP_TX_PACKET_MAX_SIZE
  #define UDP_TX_PACKET_MAX_SIZE  4096
#endif

struct DataPack {
  uint8_t   header;
  uint32_t  payload;
  uint8_t   CRC;
};

uint8_t getCRC(DataPack * dataPack);

class UDPInterface {
  public:
    UDPInterface(){};
    ~UDPInterface(){};

    void        init();
    void        update();
  private:
    WiFiUDP     udp;
    uint32_t    lastRecieveHello;
    uint32_t    lastDisplaySend;
    bool        inSync;
    bool        displayFound;
    uint32_t    _display_recieve;

    void      sendDisplayData();
};

#endif //__UDP_INTERFACE__
