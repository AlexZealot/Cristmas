/*
	Простой менеджер WiFi
	Читает из EEPROM SSID и PASS сети. Если не может найти такую сеть - поднимает точку доступа
		AP SSID: ESP{id}
		AP PASS: 00000000
	Так же, поднимает WebServer. Доступ через 192.168.4.1. На страничке можно выбрать SSID сети, и ввести её пароль. 
	Если контроллер сумеет подключиться к сети - устройство сохранит SSID и PASS и будет перезагружено.
*/
#ifndef __WIFI_MANAGER__
#define __WIFI_MANAGER__
#include <Arduino.h>
#include <ArduinoOTA.h>

#ifndef ESP8266
	#include <WiFi.h>
#else
	#include <ESP8266WiFi.h>
#endif

//состояние светодиода, при котором он не светится
#define LED_STATE_OFF		 1

//Читать ли SSID и PASS из EEPROM
#define EEPROM_WIFI

#ifdef EEPROM_WIFI
	#include <EEPROM.h>
	#ifndef ESP8266
		#include <WebServer.h>
	#else
		#include <ESP8266WebServer.h>
	#endif
	#define WIFI_SSID_OFFSET			0		 						//Где в EEPROM хранится SSID
	#define WIFI_SSID_SIZE				32								//Сколько байт занимает SSID
	#define WIFI_PASS_OFFSET			WIFI_SSID_OFFSET+WIFI_SSID_SIZE //Где в EEPROM хранится пароль от WiFi
	#define WIFI_PASS_SIZE				16								//Сколько байт он занимает
	#define EEPROM_SIZE					512

	#define AP_SSID						"ESP%u"		 					//SSID точки доступа, если SSID из EEPROM не доступен
	#define AP_PASS						"00000000"						//Пароль от точки доступа

	#define CONNECT_DELAY				20000			 				//Сколько пытаться соединиться с точкой доступа, указанной пользователем
	
	#define WFM_AP_LIVE_TIME			600000							//Время жизни точки доступа. После этого периода времени ESP будет перезагружена. По умолчанию 1000 мс * 60 с * 10мин = 10 минут
#else
	#define WIFI_SSID					"Your_SSID" 					//Для намертво приколоченного WiFi
	#define WIFI_PASS					"Your_PASS"
#endif

#define WF_DELAY_COUNT				600	 

enum	wifiInitResult_t {
	WiFi_NotConnected = 0,
	WiFi_Connected = 1
};

class WiFiManagerClass {
	public:
		WiFiManagerClass();
		~WiFiManagerClass();
	
		// @brief инициализация менеджера
		// @param useLedPin использовать ли встроенный (или внешний) светодиод на плате
		// @param led_pin пин, к которому подключен светодиод. Игнорируется, если useLedPin = false
		// @param useOTA использовать ли OTA (стандартная библиотека Arduino IDE)
		// @param OTAhostname имя хоста при использовании OTA. Игнорируется, если useOTA = false
		// @result состояние системы после инициализации.
		//  WiFi_NotConnected - WiFi не подключен, поднята точка доступа
		//  WiFi_Connected - Подключение успешно
		wifiInitResult_t				init(bool useLedPin, uint8_t led_pin, bool useOTA, const char * OTAhostname);
		// @brief вызывать в loop каждую итерацию. Обработка OTA и WebServer
		// @result состояние системы
		//  WiFi_NotConnected - WiFi не подключен, поднята точка доступа
		//  WiFi_Connected - Подключение успешно
		wifiInitResult_t				update();

		#ifdef ESP32
			// @brief Получение ID устройства. В ESP32 нет ESP.getChipID()
			uint32_t					getId();
		#endif

		#ifdef	EEPROM_WIFI
			//Очистить данные для подключения к WiFi
			void	cleanEEPROM();
		#endif

		typedef		std::function<void(bool on)>	pWFCallback;
		// @brief утановить функцию для индикаии подключения к WiFi. Аргумент в cb - состояние условного светодиода 
		// @param cb функция обратной связи
		void		setCallback(pWFCallback	cb){_callback = cb;}

		// @brief Получить состояние WiFi;
		//  WiFi_NotConnected - WiFi не подключен, поднята точка доступа
		//  WiFi_Connected - Подключение успешно
		const wifiInitResult_t	getStatus() {
			return _initResult;
		}
	private:
		pWFCallback						_callback;

		wifiInitResult_t				_initResult;
		bool							_useOTA;

		uint32_t						_ap_started;

		void							WiFiBegin(bool useLedPin, uint8_t led_pin);
	#ifndef ESP32
		uint32_t					getId();
	#endif		

	#ifdef EEPROM_WIFI
		char			WIFI_SSID[WIFI_SSID_SIZE];
		char			WIFI_PASS[WIFI_PASS_SIZE];
		int			 	_scanResult;
		bool			_useLedPin;
		uint8_t	 		_led_pin;

		#ifndef ESP8266
			WebServer			server;
		#else
			ESP8266WebServer	server;
		#endif
	#endif
};

extern WiFiManagerClass	 WiFiManager;

#endif //__WIFI_MANAGER__