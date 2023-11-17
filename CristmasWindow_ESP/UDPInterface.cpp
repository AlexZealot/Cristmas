#include "IPAddress.h"
#include "UDPInterface.h"
#include "LEDEffector.h"

static uint8_t    dataBuf[UDP_TX_PACKET_MAX_SIZE];
static IPAddress  serverIP = {0,0,0,0};
static IPAddress  displayIP = {0,0,0,0};

uint8_t getCRC(DataPack * dataPack){
  uint8_t res = dataPack->header;
  res += dataPack->payload & 0xFF;
  res += (dataPack->payload >> 8) & 0xFF;
  res += (dataPack->payload >> 16) & 0xFF;
  res += (dataPack->payload >> 24) & 0xFF;
  return res;
}

void        UDPInterface::init(){
  inSync = false;
  udp.begin(UDP_PORT);
  lastRecieveHello = 0;
  displayFound = false;
  lastDisplaySend = millis();
}

void        UDPInterface::update(){
  int packSize = udp.parsePacket();

  if (packSize == 1){
    packSize = udp.read(dataBuf, UDP_TX_PACKET_MAX_SIZE);
    if ((packSize == 1) && (dataBuf[0] == 0x80)){
      lastRecieveHello = millis();
      serverIP = udp.remoteIP();
    }

    udp.beginPacket(serverIP, UDP_PORT);
    udp.write(UDP_ID);
    udp.endPacket();
  } else
  if (packSize == sizeof(DataPack)){
    DataPack rcv_dataPack;
    packSize = udp.read((uint8_t*)&rcv_dataPack, sizeof(DataPack));
    if (packSize == sizeof(DataPack)) {
      if (rcv_dataPack.CRC == getCRC(&rcv_dataPack)){
        switch (rcv_dataPack.header){
          case UDP_DP_SYNC:
            inSync = (rcv_dataPack.payload == 1);
          break;
          case UDP_DP_MODE:
            if (inSync)
              ledEffector.setMode(rcv_dataPack.payload & 0xFF);
          break;
          case UDP_DISPLAY:
            ledEffector.setMode(rcv_dataPack.payload & 0xFF);
            _display_recieve = millis();
          break;
        }
      }
    }
  } else
  if (packSize == 2){
    packSize = udp.read(dataBuf, UDP_TX_PACKET_MAX_SIZE);
    if (packSize == 2){
      if (dataBuf[0] == 0xAF && dataBuf[1] == 0xE8){
        //Пришёл привет от дисплея.
        displayIP = udp.remoteIP();
        displayFound = true;
        sendDisplayData();
        _display_recieve = millis();
      }
    }
  }

  if (packSize != 0){
    udp.flush();
  }

  if (millis() - lastRecieveHello > 10000){
    inSync = false;
  }

  if (millis() - _display_recieve > DESPLAY_RECIEVE_INTERVAL)
    displayFound = false;

  if ((millis() - lastDisplaySend > DISPLAY_RESEND_INTERVAL) && (displayFound)){
    sendDisplayData();
    lastDisplaySend = millis();
  }
}

static    DataPack  displayPack;
void      UDPInterface::sendDisplayData(){
  displayPack.header = UDP_DISPLAY;
  displayPack.payload = 
    (ledEffector.getMode()) | 
    (inSync?(0x01 << 8):(0x00 << 8)) |
    (UDP_ID << 16);
  displayPack.CRC = getCRC(&displayPack);
  udp.beginPacket(displayIP, UDP_PORT);
  udp.write((uint8_t*)&displayPack, sizeof(DataPack));
  udp.endPacket();
  lastDisplaySend = millis();
}