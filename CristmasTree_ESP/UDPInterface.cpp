#include "IPAddress.h"
#include "UDPInterface.h"
#include "ledEffector.h"

UDPInterface    udpInterface;

static IPAddress    UDP_HALL_LIGHTS_IP{192, 168, 1, 58};
static IPAddress    UDP_CHILDREN_IP{192, 168, 1, 145};
static IPAddress    UDP_TV_IP{192, 168, 1, 237};
static IPAddress    UDP_DISPLAY{0,0,0,0};

uint8_t getCRC(DataPack * dataPack){
  uint8_t res = dataPack->header;
  res += dataPack->payload & 0xFF;
  res += (dataPack->payload >> 8) & 0xFF;
  res += (dataPack->payload >> 16) & 0xFF;
  res += (dataPack->payload >> 24) & 0xFF;
  return res;
}

void    UDPInterface::init(){
  udp.begin(UDP_PORT);
  hallSync = false;
  childrenSync = false;
  tvSync = false;
  lastUPD = millis();
  lastSendAlive = millis();
}

void    UDPInterface::update(uint32_t T){
  if ((T - lastSendAlive) >= 1000){
    lastSendAlive = T;
    udp.beginPacket("255.255.255.255", UDP_PORT);
    udp.write(0x80);
    udp.endPacket();
  }

  int packSize = udp.parsePacket();
  if (packSize == 1){
    uint8_t udp_id;
    udp.read(&udp_id, 1);

    switch (udp_id){
      case UDP_HALL_WINDOW:
        UDP_HALL_LIGHTS_IP = udp.remoteIP();
        break;

      case UDP_HALL_TV:
        UDP_TV_IP = udp.remoteIP();
        break;

      case UDP_CHILDREN:
        UDP_CHILDREN_IP = udp.remoteIP();
        break;
    }
  }

  uint8_t dataBuf[2];
  if (packSize == 2){
    packSize = udp.read(dataBuf, 2);
    if (packSize == 2){
      if ((dataBuf[0] == 0xAF) && (dataBuf[1] == 0xE8)){
        UDP_DISPLAY = udp.remoteIP();
        displayFound = true;
        sendDisplayData();
        _display_recieve = millis();
      }
    }
  }

  if (packSize == sizeof(DataPack)){
    DataPack rcv_dataPack;
    packSize = udp.read((uint8_t*)&rcv_dataPack, sizeof(DataPack));
    if (packSize == sizeof(DataPack)){
      if ((rcv_dataPack.CRC == getCRC(&rcv_dataPack)) && (rcv_dataPack.header == 0x03)){
        ledEffector.setMode(rcv_dataPack.payload & 0xFF);
        tvSync        = (rcv_dataPack.payload >> 24) & 0x01;
        hallSync      = (rcv_dataPack.payload >> 24) & 0x02;
        childrenSync  = (rcv_dataPack.payload >> 24) & 0x04;
        _display_recieve = millis();
      }
    }
  }

  if ((T - lastUPD) > 500){
    lastUPD = T;
    if (tvSync){
      DataPack  dataPack;
      dataPack.header = UDP_DP_SYNC;
      dataPack.payload = 1;
      dataPack.CRC = getCRC(&dataPack);
      udp.beginPacket(UDP_TV_IP, UDP_PORT);
      udp.write((uint8_t*)&dataPack, sizeof(DataPack));
      udp.endPacket();

      dataPack.header = UDP_DP_MODE;
      dataPack.payload = ledEffector.getMode();
      dataPack.CRC = getCRC(&dataPack);
      udp.beginPacket(UDP_TV_IP, UDP_PORT);
      udp.write((uint8_t*)&dataPack, sizeof(DataPack));
      udp.endPacket();      
    } else {
      DataPack  dataPack;
      dataPack.header = UDP_DP_SYNC;
      dataPack.payload = 0;
      dataPack.CRC = getCRC(&dataPack);
      udp.beginPacket(UDP_TV_IP, UDP_PORT);
      udp.write((uint8_t*)&dataPack, sizeof(DataPack));
      udp.endPacket();  
    }

    if (hallSync){
      DataPack  dataPack;
      dataPack.header = UDP_DP_SYNC;
      dataPack.payload = 1;
      dataPack.CRC = getCRC(&dataPack);
      udp.beginPacket(UDP_HALL_LIGHTS_IP, UDP_PORT);
      udp.write((uint8_t*)&dataPack, sizeof(DataPack));
      udp.endPacket();

      dataPack.header = UDP_DP_MODE;
      dataPack.payload = ledEffector.getMode();
      dataPack.CRC = getCRC(&dataPack);
      udp.beginPacket(UDP_HALL_LIGHTS_IP, UDP_PORT);
      udp.write((uint8_t*)&dataPack, sizeof(DataPack));
      udp.endPacket();      
    } else {
      DataPack  dataPack;
      dataPack.header = UDP_DP_SYNC;
      dataPack.payload = 0;
      dataPack.CRC = getCRC(&dataPack);
      udp.beginPacket(UDP_HALL_LIGHTS_IP, UDP_PORT);
      udp.write((uint8_t*)&dataPack, sizeof(DataPack));
      udp.endPacket();      
    }

    if (childrenSync){
      DataPack  dataPack;
      dataPack.header = UDP_DP_SYNC;
      dataPack.payload = 1;
      dataPack.CRC = getCRC(&dataPack);
      udp.beginPacket(UDP_CHILDREN_IP, UDP_PORT);
      udp.write((uint8_t*)&dataPack, sizeof(DataPack));
      udp.endPacket();

      dataPack.header = UDP_DP_MODE;
      dataPack.payload = ledEffector.getMode();
      dataPack.CRC = getCRC(&dataPack);
      udp.beginPacket(UDP_CHILDREN_IP, UDP_PORT);
      udp.write((uint8_t*)&dataPack, sizeof(DataPack));
      udp.endPacket();      
    } else {
      DataPack  dataPack;
      dataPack.header = UDP_DP_SYNC;
      dataPack.payload = 0;
      dataPack.CRC = getCRC(&dataPack);
      udp.beginPacket(UDP_CHILDREN_IP, UDP_PORT);
      udp.write((uint8_t*)&dataPack, sizeof(DataPack));
      udp.endPacket();      
    }
  }

  if ((millis() - lastDisplaySend > DISPLAY_RESEND_INTERVAL) && (displayFound)){
    sendDisplayData();
    lastDisplaySend = millis();
  }  

  if (millis() - _display_recieve > DISPLAY_RESEND_INTERVAL){
    displayFound = false;
  }
}

static    DataPack  displayPack;
void      UDPInterface::sendDisplayData(){
  displayPack.header = 0x03;
  uint8_t sf = 0;
  sf |= tvSync      ?0x01:0x00;
  sf |= hallSync    ?0x02:0x00;
  sf |= childrenSync?0x04:0x00;
  displayPack.payload = 
    ((ledEffector.getMode()) | 
    (0x0B << 16) | 
    (sf << 24));
  displayPack.CRC = getCRC(&displayPack);
  udp.beginPacket(UDP_DISPLAY, UDP_PORT);
  udp.write((uint8_t*)&displayPack, sizeof(DataPack));
  udp.endPacket();
}