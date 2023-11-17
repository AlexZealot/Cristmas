#include "UDPConnect.h"
#include "../gui/devData.h"

UDPConnectClass		UDPConnect;

uint8_t getCRC(DataPack * dataPack){
    uint8_t res = 0;
    res = dataPack->header;
    res += dataPack->payload & 0xFF;
    res += (dataPack->payload >> 8) & 0xFF;
    res += (dataPack->payload >> 16) & 0xFF;
    res += (dataPack->payload >> 24) & 0xFF;
    return res;
}

const uint8_t	UDPConnectClass::hello_data[2] = {0xAF, 0xE8};

void    UDPConnectClass::init(){
    lastSendHello = millis();
    udp.onPacket([](AsyncUDPPacket& packet){
        if (packet.length() == sizeof(DataPack)){
            DataPack * dp = (DataPack*)packet.data();
            if ((dp->CRC == getCRC(dp)) && (dp->header == UDP_DISPLAY)){
                uint8_t m  = (dp->payload      ) & 0xFF;//(dp->payload & 0xFF);
                uint8_t s  = (dp->payload >>  8) & 0xFF;//((dp->payload & 0xFF00) >> 8);
                uint8_t id = (dp->payload >> 16) & 0xFF;//((dp->payload & 0xFF0000) >> 16);
                uint8_t sf = (dp->payload >> 24) & 0xFF;//((dp->payload & 0xFF000000) >> 24);
                device_t *dt = GUI.getDevice(id);
                if (dt != NULL){
                    dt->ip = packet.remoteIP();
                    dt->mode = m;
                    dt->inSync = s;
                    dt->connected = true;
                    dt->syncFlag = sf;
                    dt->whenConnected = millis();
                }
            }
        }
    });
    udp.listen(UDP_PORT);
}

void	UDPConnectClass::update(){
    if (GUI.getState() == ds_dev_list)
        if (millis() - lastSendHello > HELLO_DELAY){
            udp.broadcast((uint8_t*)hello_data, 2);
            lastSendHello = millis();
        }
}

void	UDPConnectClass::sendData(const device_t& d){
    DataPack dp;
    dp.header = UDP_DISPLAY;
    dp.payload = (d.mode & 0x000000FF) | ((d.syncFlag << 24) & 0xFF000000);
    dp.CRC = getCRC(&dp);
    udp.writeTo((uint8_t*)&dp, sizeof(DataPack), d.ip, UDP_PORT);
    udp.writeTo((uint8_t*)hello_data, 2, d.ip, UDP_PORT);
}