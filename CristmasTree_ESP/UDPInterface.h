#include <stdint.h>
#ifndef __UDP_INTERFACE__
#define __UDP_INTERFACE__

#include <WiFiUDP.h>

#define UDP_PORT      1234

/*
Передаваемые команды:
1) синхронизировать
2) не синхронизаровать
3) включить режим
*/

#define UDP_DP_SYNC     0x01
#define UDP_DP_MODE     0x02

#define UDP_HALL_WINDOW   0x08
#define UDP_HALL_TV       0x09
#define UDP_CHILDREN      0x0A

#define DISPLAY_RESEND_INTERVAL 2000

struct DataPack {
  uint8_t   header;
  uint32_t  payload;
  uint8_t   CRC;
};

uint8_t getCRC(DataPack * dataPack);

class UDPInterface {
  public:
    UDPInterface():
    displayFound(false)
    {};
    ~UDPInterface(){};

    void    init();
    void    update(uint32_t T);

    bool    hallSync;
    bool    childrenSync;
    bool    tvSync;
  private:
    bool      displayFound;
    WiFiUDP   udp;

    uint32_t    lastUPD;
    uint32_t    lastSendAlive;
    uint32_t    lastDisplaySend;
    uint32_t    _display_recieve;

    void sendDisplayData();
};

extern UDPInterface    udpInterface;

#endif //__UDP_INTERFACE__
