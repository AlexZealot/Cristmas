#pragma once
#include <Arduino.h>
#include <AsyncUDP.h>
#include "../gui/GUI.h"

#define UDP_PORT		1234
#define UDP_DISPLAY     0x03
#define HELLO_DELAY		5000	//как часто отправляем "привет"

struct DataPack {
  uint8_t   header;
  uint32_t  payload;
  uint8_t   CRC;
};
uint8_t getCRC(DataPack * dataPack);

// @brief обёртка над асинхронным UDP для связи с гирляндами
class UDPConnectClass {
	public:
		UDPConnectClass(){}
		~UDPConnectClass(){}

		// @brief инициализация ядра для обработки udp пакетов
		void	init();

		// @brief тикер для отправки пакета hello
		void	update();

		// @brief отправить данные на устройство
		// @param d устройство, на которое нужно отправить данные
		void	sendData(const device_t& d);
	private:
		AsyncUDP	udp;
		uint32_t	lastSendHello;
		static const uint8_t	hello_data[2];		
};

extern UDPConnectClass		UDPConnect;