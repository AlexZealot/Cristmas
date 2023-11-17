#ifndef __LED_EFFECTOR__
#define __LED_EFFECTOR__

#include <FastLED.h>

#define PIN_LED               18 //пин ленты!
#define FPS                   50

#define LEDS_COUNT            126

#define MAX_CURRENT_MA        4500.0f

#define MODES_COUNT           28

#define MODE_OFF          0
#define MODE_R            1
#define MODE_G            2
#define MODE_B            3
#define MODE_WHITE        4
#define MODE_HSV_H        5
#define MODE_HSV_S        6
#define MODE_HSV_V        7
#define MODE_FLAG_1       8
#define MODE_FIRE         9
#define MODE_RAINBOW      10
#define MODE_HSV          11
#define MODE_WAVE         12
#define MODE_RAINBOW_ST   13
#define MODE_RAIN_1       14
#define MODE_RAIN_2       15
#define MODE_RAIN_3       16
#define MODE_RAIN_4       17
#define MODE_TREE         18
#define MODE_WAVE_R       19
#define MODE_WAVE_G       20
#define MODE_WAVE_B       21
#define MODE_FL_DOTS      22
#define MODE_FLAG_DYN     23
#define MODE_HSV_DIF      24
#define MODE_FIRE_1       25
#define MODE_FIRE_2       26
#define MODE_FIRE_3       27

const char mode_names[MODES_COUNT][64] = {
  "Выключено", 
  "Красный", 
  "Зелёный", 
  "Синий", 
  "Белый", 
  "HSV радуга H", 
  "HSV радуга S", 
  "HSV радуга V", 
  "Статичный флаг", 
  "Огонь", 
  "Радуга", 
  "HSV", 
  "Водопад",
  "Статичная радуга",
  "Дождь 1",
  "Дождь 2",
  "Дождь 3 (разноцветный)",
  "Дождь 4 (разноцветный)",
  "Ёлка (не очень)",
  "Водопад (красный)",
  "Водопад (зелёный)",
  "Водопад (синий)",
  "Летающие точки",
  "Динамический флаг",
  "HSV радуга в 2 цвета",
  "Огонь (зелёный)",
  "Огонь (синий)",
  "Огонь (розовый)"
};

#define FLYING_DOTS_COUNT   6
struct  FlyingDot{
  uint8_t start_point;
  uint8_t end_point;
  CRGB    col;
  uint8_t spdTick;
  uint8_t spd;
};

class LEDEffector {
  private:
    FlyingDot     dots[FLYING_DOTS_COUNT];
    CRGB          leds[LEDS_COUNT];  
    unsigned long lastUpd;
    unsigned long dT;
    uint8_t       mode;
    uint8_t       br;
    bool          modeChanged;

    void  zeroLEDS();

    void  runOFF();
    void  runR();
    void  runG();
    void  runB();
    void  runLights(unsigned long T);
    void  runLightsHSV2(unsigned long T);
    void  runLightsValue(unsigned long T);
    void  runLightsSaturation(unsigned long T);
    void  runFlag_v1();
    void  runFlag_v2(unsigned long T);
    void  runFire(unsigned long T);
    void  runFire1(unsigned long T);
    void  runFire2(unsigned long T);
    void  runFire3(unsigned long T);
    void  runRainbow(unsigned long T);
    void  runWhite();
    void  runHSV(unsigned long T);
    void  runWave(unsigned long T);
    void  runWaveR(unsigned long T);
    void  runWaveG(unsigned long T);
    void  runWaveB(unsigned long T);
    void  runStaticRainbow();
    void  runRainV1(unsigned long T);
    void  runRainV2(unsigned long T);
    void  runRainV3_mc(unsigned long T);
    void  runRainV4_mc(unsigned long T);
    void  runTree(unsigned long T);
    void  runFlyingDots(unsigned long T);

    CRGB  getColor(int colIdx);

    void  addPixel(uint8_t idx, CRGB col);
    CRGB  getTableColor(int colIdx);
  public:
    LEDEffector();

    void    init();
    void    setBrightness(byte B);
    void    update(unsigned long T);
    uint8_t getMode();
    uint8_t setMode(uint8_t newMode);
    const char * getModeName(uint8_t _m);
    uint8_t getModeByName(const char * mname);

    uint8_t incMode();
    uint8_t decMode();
    bool    getModeChanged();    

    float   getCurrent();    
    uint8_t getBrightness();
};

extern LEDEffector ledEffector;

#endif //__LED_EFFECTOR__
