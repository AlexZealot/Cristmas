#pragma once
#include "devData.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <FS.h>
#ifndef ESP8266
	#include <WiFi.h>
#else
	#include <ESP8266WiFi.h>
#endif

extern int p_c;

#define DEV_COUNT   4
struct device_t {
    IPAddress   ip;
    uint8_t     id;
    uint8_t     mode;
    uint8_t     lastMode;
    uint8_t     inSync;
    uint8_t     lastInSync;
    bool        connected;
    bool        lastConnected;
    uint32_t    whenConnected;
    uint8_t     syncFlag;
    uint8_t     lastSyncFlag;
};

#define UDP_HALL_WINDOW     0x08
#define UDP_HALL_TV         0x09
#define UDP_CHILDREN        0x0A
#define UDP_XMAS_TREE       0x0B  //!!!!!!

#define CONNECTED_DELAY     15000
#define TOUCH_DELAY         100
#define ADC_DELAY           5000

#define FPS     25
#define DUDP    1000/FPS

#define CALIBRATION_FILE "/TouchCalDataFile"

#define REPEAT_CAL false
//#define MANUAL_CAL_DATA

enum display_state_t {
    ds_dev_list,
    ds_modes_list
};

enum bat_state_t {
    bs_full,
    bs_mid_h,
    bs_mid_l,
    bs_low,
    bs_unknown
};

#define PIN_BACKLIGHT   32
#define PIN_ADC         34
class GUIClass {
    public:
        GUIClass():
            _display_state(ds_dev_list),
            _last_display_state(ds_dev_list),
            _selected_dev(0),
            _lastTouch(0),
            _repaintDisplay(true),
            _bat_state(bs_full),
            _last_bat_state(bs_unknown),
            _last_adc(0)
        {
            ledcSetup(0, 5000, 8);
            ledcAttachPin(PIN_BACKLIGHT, 0);
        }
        ~GUIClass(){}

        // @brief Инициализация дисплея
        void            init();
        // @brief показать логотип WiFi
        void            showWiFiLogo();
        // @brief управление подсветкой дисплея
        // @param duty ШИМ подсветки
        void            setBacklight(uint8_t duty){/*digitalWrite(PIN_BACKLIGHT, duty==0?0:1);*/ledcWrite(0, duty);}
        // @brief получить указатель на структуру устройства по его ID
        // @param ID идентефикатор устройства
        device_t*       getDevice(uint8_t ID);
        // @brief стереть экран
        void            blackScreen();
        // @brief тикер для обновления дисплея
        void            update();
        // @brief получить текущий режим дисплея
        display_state_t getState(){return _display_state;}
    private:
        bat_state_t         _bat_state;
        bat_state_t         _last_bat_state;
        uint32_t            _lastTouch;
        uint16_t            _tx;
        uint16_t            _ty;
        display_state_t     _display_state;
        display_state_t     _last_display_state;
        uint8_t             _selected_dev;
        bool                _repaintDisplay;
        uint16_t            _adc_val;
        uint32_t            _last_adc;
        TFT_eSPI            tft{};

        device_t    _devices[DEV_COUNT];

        uint32_t    _lastDraw;

        // @brief получить имя устройства по его ID
        const char * getDeviceName(uint8_t id);

        // @brief построить 16-bit RGB цвет из полноценных 24 бит
        uint16_t    makeRGB(uint8_t R, uint8_t G, uint8_t B);

        // @brief нарисовать интерфейс для устройства device со смещением по оси y
        // @param d устройство
        // @param y_offs смещение по оси y
        void        drawDevice(device_t &d, uint16_t y_offs);

        void        touchCalibrate();
};

extern GUIClass GUI;