#ifndef __LED_EFFECTOR__
#define __LED_EFFECTOR__

//#define TEST_MODE

#include "Configuration.h"

#include <FastLED.h>

#ifdef HALL_LEDS
  #define ROWS_8
#endif

#ifndef ESP32
  #define PIN_LED               12 //пин ленты!
#else
  #define PIN_LED               9
#endif

#ifdef TEST_MODE
  #define FPS                   1
#else
  #define FPS                   50
#endif

#ifdef ROWS_8
  #define LEDS_COUNT          100
#else
  #ifndef TV_LEDS
    #define LEDS_COUNT          74 
  #else
    #define LEDS_COUNT          88
  #endif
#endif

#define FLY_POINTS_COUNT      6
#define FLY_POINTS_UPD        5

#ifdef ROWS_8
  #define ROWS_COUNT            8
  #define FLAG_LINES_PER_COLOR  4
#else
  #define ROWS_COUNT            9
  #define FLAG_LINES_PER_COLOR  3
#endif

#ifndef TV_LEDS
  #define MAX_CURRENT_MA        4500.0f
#else
  #define MAX_CURRENT_MA        10000.0f
#endif

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
#define MODE_FIRE_2       25
#define MODE_FIRE_3       26
#define MODE_FIRE_4       27

#ifdef ROWS_8
  const uint8_t leds_per_col_count [ROWS_COUNT] = {11, 12, 13, 14, 14, 13, 12, 11};
#else
  #ifndef TV_LEDS
    const uint8_t leds_per_col_count [ROWS_COUNT] = {8, 7, 9, 8, 10, 8, 9, 7, 8};
  #endif
#endif

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

#define DOT_TICK        2

struct RainDot {
  uint8_t x;
  uint8_t y;
  int8_t  dT;
  uint8_t spd;
  CRGB    dotColor;
};

struct PointDir {
  int8_t  x;
  int8_t  y;
};

struct FlyPoint {
  int8_t  x;
  int8_t  y;
  CRGB    color;
  int8_t  dT;
  int8_t  init_dT;

  PointDir  prevPoint;
  PointDir  prevPrevPoint;
  PointDir  spd;
};

#ifdef TV_LEDS
#define FLYING_DOTS_COUNT   6
struct  FlyingDot{
  uint8_t start_point;
  uint8_t end_point;
  CRGB    col;
  uint8_t spdTick;
  uint8_t spd;
};
#endif

class LEDEffector {
  private:
  #ifdef TV_LEDS
    FlyingDot     dots[FLYING_DOTS_COUNT];
  #else
    RainDot       r1_dots[ROWS_COUNT];
    RainDot       r2_dots[ROWS_COUNT];
    RainDot       r3_dots[ROWS_COUNT];
    RainDot       r4_dots[ROWS_COUNT];
  #endif

    CRGB          leds[LEDS_COUNT];  
    unsigned long lastUpd;
    unsigned long dT;
    uint8_t       mode;
    uint8_t       br;
    bool          modeChanged;
    uint8_t       max_lines;

    FlyPoint      points[FLY_POINTS_COUNT];

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
    void  runFire2(unsigned long T);
    void  runFire3(unsigned long T);
    void  runFire4(unsigned long T);
    void  runRainbow(unsigned long T);//TODO
    void  runWhite();
    void  runHSV(unsigned long T);
    void  runWave(unsigned long T);
    void  runWaveR(unsigned long T);
    void  runWaveG(unsigned long T);
    void  runWaveB(unsigned long T);
    void  runStaticRainbow();//TODO
    void  runRainV1(unsigned long T);
    void  runRainV2(unsigned long T);
    void  runRainV3_mc(unsigned long T);
    void  runRainV4_mc(unsigned long T);
    void  runTree(unsigned long T);
    void  runFlyingDots();//TODO

  #ifndef TV_LEDS
    void  setRowColor(uint8_t lineNum, CRGB color, bool zeroOther = true, bool fadeToZero = false);
    void  setLineColor(uint8_t lineNum, CRGB color);
    PointDir  tryPoint(int8_t x, int8_t y); 
  #endif

    CRGB  getColor(uint8_t colIdx);
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

  #ifndef TV_LEDS
    void    putPixel(uint8_t x, uint8_t y, CRGB col);
    void    putPixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);

    void    addPixel(uint8_t x, uint8_t y, CRGB col);
    void    addPixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);

    uint8_t getPixelIdx(int8_t x, int8_t y);
  #else
    void    addPixel(uint8_t idx, CRGB col);
    void    addPixel(uint8_t idx, uint8_t r, uint8_t g, uint8_t b);
  #endif
};

extern LEDEffector ledEffector;

#endif //__LED_EFFECTOR__
