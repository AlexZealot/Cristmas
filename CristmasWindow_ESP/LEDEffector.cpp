#include "LEDEffector.h"

LEDEffector ledEffector;

#define MAX(a, b) (a > b) ? a : b
#define MIN(a, b) (a < b) ? a : b

CRGB col_table[7];

LEDEffector::LEDEffector() {
}

void LEDEffector::init() {
#ifdef TEST_MODE
  test_lineIdx = 0;
#endif

  memset(leds, 0, sizeof(CRGB) * LEDS_COUNT);
  #ifndef HALL_LEDS
    #ifndef TV_LEDS
      FastLED.addLeds<WS2812, PIN_LED, BGR>(leds, LEDS_COUNT).setCorrection(TypicalLEDStrip);    
    #else
      FastLED.addLeds<WS2812B, PIN_LED, GRB>(leds, LEDS_COUNT).setCorrection(TypicalLEDStrip);
    #endif
  #else
    FastLED.addLeds<WS2812B, PIN_LED, GRB>(leds, LEDS_COUNT).setCorrection(TypicalLEDStrip);
  #endif
  setBrightness(255);
  zeroLEDS();
  mode = MODE_OFF;

  for (uint8_t i = 0; i < 7; i++) {
    col_table[i] = CRGB((62 * i) % 255, (62 * (i + 1)) % 255, (62 * (i + 2)) % 255);
  }

#ifndef TV_LEDS
  max_lines = 0;
  for (uint8_t i = 0; i < ROWS_COUNT; i++) {
    if (leds_per_col_count[i] > max_lines)
      max_lines = leds_per_col_count[i];
  }

  for (uint8_t i = 0; i < ROWS_COUNT; i++) {
    r1_dots[i].x = i;
    r1_dots[i].dT = DOT_TICK;

    r3_dots[i].x = i;
    r3_dots[i].dT = DOT_TICK;
    r3_dots[i].dotColor = CRGB(random(0xff), random(0xff), random(0xff));

    r2_dots[i].x = i;
    r2_dots[i].y = random(leds_per_col_count[i]);
    r2_dots[i].dT = random(4) + 1;
    r2_dots[i].spd = r2_dots[i].dT;

    r4_dots[i].x = i;
    r4_dots[i].y = random(leds_per_col_count[i]);
    r4_dots[i].dT = random(4) + 1;
    r4_dots[i].spd = r4_dots[i].dT;
    r4_dots[i].dotColor = CRGB(random(0xff), random(0xff), random(0xff));
  }

  if (ROWS_COUNT == 8) {
    r1_dots[0].y = 0;
    r1_dots[1].y = 1;
    r1_dots[2].y = 2;
    r1_dots[3].y = 3;
    r1_dots[4].y = 3;
    r1_dots[5].y = 2;
    r1_dots[6].y = 1;
    r1_dots[7].y = 0;

    r3_dots[0].y = 0;
    r3_dots[1].y = 1;
    r3_dots[2].y = 2;
    r3_dots[3].y = 3;
    r3_dots[4].y = 3;
    r3_dots[5].y = 2;
    r3_dots[6].y = 1;
    r3_dots[7].y = 0;
  }

  if (ROWS_COUNT == 9) {
    r1_dots[0].y = 1;
    r1_dots[1].y = 0;
    r1_dots[2].y = 2;
    r1_dots[3].y = 1;
    r1_dots[4].y = 3;
    r1_dots[5].y = 1;
    r1_dots[6].y = 2;
    r1_dots[7].y = 0;
    r1_dots[8].y = 1;

    r3_dots[0].y = 1;
    r3_dots[1].y = 0;
    r3_dots[2].y = 2;
    r3_dots[3].y = 1;
    r3_dots[4].y = 3;
    r3_dots[5].y = 1;
    r3_dots[6].y = 2;
    r3_dots[7].y = 0;
    r3_dots[8].y = 1;
  }

  for (uint8_t i = 0; i < FLY_POINTS_COUNT; i++) {
    points[i].x = random(8);
    points[i].y = random(11);
    points[i].prevPoint.x = points[i].x;
    points[i].prevPoint.y = points[i].y;
    points[i].prevPrevPoint.x = points[i].x;
    points[i].prevPrevPoint.y = points[i].y;
    points[i].spd.x = random(2) ? 1 : -1;
    points[i].spd.y = random(2) ? 1 : -1;
    points[i].color = CRGB(62 * i, (62 * (i + 1)) % 255, (62 * (i + 2)) % 255);

    points[i].init_dT = random(5) + FLY_POINTS_UPD;
    points[i].dT = points[i].init_dT;
  }
#else
  for (uint8_t i = 0; i < FLYING_DOTS_COUNT; i++) {
    dots[i].start_point = random(LEDS_COUNT);
    dots[i].end_point = dots[i].start_point + random(10) + 5;
    dots[i].spd = random(5) + 5;
    dots[i].spdTick = dots[i].spd;
    dots[i].col = CRGB((62 * i) % 255, (62 * (i + 1)) % 255, (62 * (i + 2)) % 255);
  }
#endif

  dT = 1000 / FPS;
  lastUpd = millis();
  modeChanged = true;
}

void LEDEffector::runFlyingDots() {
#ifndef TV_LEDS
  zeroLEDS();
  PointDir dirCorrect;
  for (uint8_t i = 0; i < FLY_POINTS_COUNT; i++) {
    points[i].dT--;
    if (points[i].dT <= 0) {
      points[i].dT = points[i].init_dT;
      points[i].prevPrevPoint = points[i].prevPoint;
      points[i].prevPoint.x = points[i].x;
      points[i].prevPoint.y = points[i].y;

      dirCorrect = tryPoint(points[i].x + points[i].spd.x, points[i].y + points[i].spd.y);
      if (dirCorrect.x != 0) {
        points[i].spd.x = -points[i].spd.x;
      }
      if (dirCorrect.y != 0) {
        /*if (dirCorrect.y == 1)
          points[i].spd.y = -abs(points[i].spd.y);
        if (dirCorrect.y == -1)
        points[i].spd.y = abs(points[i].spd.y);*/
        points[i].spd.y = -dirCorrect.y;
      }
      points[i].x += points[i].spd.x;
      points[i].y += points[i].spd.y;
    }

    putPixel(points[i].x, points[i].y, points[i].color);
    putPixel(points[i].prevPoint.x, points[i].prevPoint.y, points[i].color.r * 0.5, points[i].color.g * 0.5, points[i].color.b * 0.5);
    putPixel(points[i].prevPrevPoint.x, points[i].prevPrevPoint.y, points[i].color.r * 0.1, points[i].color.g * 0.1, points[i].color.b * 0.1);
  }
#else
  zeroLEDS();
  for (uint8_t i = 0; i < FLYING_DOTS_COUNT; i++) {
    addPixel(dots[i].start_point, dots[i].col);
    addPixel(dots[i].start_point - 1, CRGB(dots[i].col.r * 0.5, dots[i].col.g * 0.5, dots[i].col.b * 0.5));
    addPixel(dots[i].start_point - 2, CRGB(dots[i].col.r * 0.1, dots[i].col.g * 0.1, dots[i].col.b * 0.1));

    dots[i].spdTick--;
    if (dots[i].spdTick == 0) {
      dots[i].start_point++;
      dots[i].spdTick = dots[i].spd;

      if (dots[i].start_point == dots[i].end_point) {
        dots[i].start_point = random(LEDS_COUNT);
        dots[i].end_point = dots[i].start_point + random(10) + 5;
      }
    }
  }
#endif
}

#ifdef TV_LEDS
void LEDEffector::addPixel(uint8_t idx, CRGB col) {
  if (idx < LEDS_COUNT) {
    leds[idx].r = MIN(leds[idx].r + col.r, 0xFF);
    leds[idx].g = MIN(leds[idx].g + col.g, 0xFF);
    leds[idx].b = MIN(leds[idx].b + col.b, 0xFF);
  }
}

void LEDEffector::addPixel(uint8_t idx, uint8_t r, uint8_t g, uint8_t b) {
  if (idx < LEDS_COUNT) {
    leds[idx].r = MIN(leds[idx].r + r, 0xFF);
    leds[idx].g = MIN(leds[idx].g + g, 0xFF);
    leds[idx].b = MIN(leds[idx].b + b, 0xFF);
  }
}
#endif

void LEDEffector::setBrightness(byte B) {
  FastLED.setBrightness(B);
  br = B;
}

uint8_t LEDEffector::getBrightness() {
  return br;
}

void LEDEffector::update(unsigned long T) {
  //работаем с лентой

  if (T - lastUpd >= dT) {
    switch (mode) {
      case MODE_OFF: runOFF(); break;
      case MODE_R: runR(); break;
      case MODE_G: runG(); break;
      case MODE_B: runB(); break;
      case MODE_HSV_H: runLights(T); break;
      case MODE_HSV_S: runLightsSaturation(T); break;
      case MODE_HSV_V: runLightsValue(T); break;
      case MODE_FLAG_1: runFlag_v1(); break;
      case MODE_FIRE: runFire(T); break;
      case MODE_RAINBOW: runRainbow(T); break;
      case MODE_WHITE: runWhite(); break;
      case MODE_HSV: runHSV(T); break;
      case MODE_WAVE: runWave(T); break;
      case MODE_RAINBOW_ST: runStaticRainbow(); break;
      case MODE_RAIN_1: runRainV1(T); break;
      case MODE_RAIN_2: runRainV2(T); break;
      case MODE_RAIN_3: runRainV3_mc(T); break;
      case MODE_RAIN_4: runRainV4_mc(T); break;
      case MODE_TREE: runTree(T); break;
      case MODE_WAVE_R: runWaveR(T); break;
      case MODE_WAVE_G: runWaveG(T); break;
      case MODE_WAVE_B: runWaveB(T); break;
      case MODE_FL_DOTS: runFlyingDots(); break;
      case MODE_FLAG_DYN: runFlag_v2(T); break;
      case MODE_HSV_DIF: runLightsHSV2(T); break;
      case MODE_FIRE_2: runFire2(T); break;
      case MODE_FIRE_3: runFire3(T); break;
      case MODE_FIRE_4: runFire4(T); break;
    }
    lastUpd = T;

    float cI = getCurrent() * 1000.0f;
    if (cI > MAX_CURRENT_MA)
      setBrightness((MAX_CURRENT_MA)*255 / cI);
    else if (br != 0xFF)
      setBrightness(0xFF);

    FastLED.show();  // записываем цвета в ленту
  }
}

void LEDEffector::runRainV1(unsigned long T) {
#ifndef TV_LEDS
  zeroLEDS();
  for (uint8_t i = 0; i < ROWS_COUNT; i++) {
    r1_dots[i].dT -= 1;
    if (r1_dots[i].dT < 0) {
      r1_dots[i].dT = DOT_TICK;
      r1_dots[i].y++;
      if (r1_dots[i].y > (leds_per_col_count[i] + 1)) {
        r1_dots[i].y = 0;
      }
    }
    putPixel(r1_dots[i].x, r1_dots[i].y, CRGB(255, 255, 255));
    putPixel(r1_dots[i].x, r1_dots[i].y - 1, CRGB(95, 95, 95));
    putPixel(r1_dots[i].x, r1_dots[i].y - 2, CRGB(10, 10, 10));
  }
#else
  runWave(T);
#endif
}

void LEDEffector::runRainV3_mc(unsigned long T) {
#ifndef TV_LEDS
  zeroLEDS();
  for (uint8_t i = 0; i < ROWS_COUNT; i++) {
    r3_dots[i].dT -= 1;
    if (r3_dots[i].dT < 0) {
      r3_dots[i].dT = DOT_TICK;
      r3_dots[i].y++;
      if (r3_dots[i].y > (leds_per_col_count[i] + 1)) {
        r3_dots[i].y = 0;
        r3_dots[i].dotColor = CRGB(random(0xff), random(0xff), random(0xff));
      }
    }
    putPixel(r3_dots[i].x, r3_dots[i].y, r3_dots[i].dotColor);
    putPixel(r3_dots[i].x, r3_dots[i].y - 1, CRGB(r3_dots[i].dotColor.r * 0.37, r3_dots[i].dotColor.g * 0.37, r3_dots[i].dotColor.b * 0.37));
    putPixel(r3_dots[i].x, r3_dots[i].y - 2, CRGB(r3_dots[i].dotColor.r * 0.04, r3_dots[i].dotColor.g * 0.04, r3_dots[i].dotColor.b * 0.04));
  }
#else
  uint8_t K;
  uint8_t W;
  for (uint8_t i = 0; i < LEDS_COUNT; i++) {
    K = 3 - (((LEDS_COUNT - i) + T / 100) % 4);
    W = 37.5 * K * K - 27.5 * K;
    float k = W / 255.0f;
    CRGB col = getTableColor(i / 12);
    leds[i] = CRGB(col.r * k, col.g * k, col.b * k);
  }
#endif
}

CRGB LEDEffector::getTableColor(int colIdx) {
  colIdx = colIdx % 7;
  return col_table[colIdx];
}

void LEDEffector::runRainV2(unsigned long T) {
#ifndef TV_LEDS
  zeroLEDS();
  for (uint8_t i = 0; i < ROWS_COUNT; i++) {
    r2_dots[i].dT -= 1;
    if (r2_dots[i].dT < 0) {
      r2_dots[i].dT = r2_dots[i].spd;
      r2_dots[i].y++;
      if (r2_dots[i].y > (leds_per_col_count[i] + 1)) {
        r2_dots[i].y = 0;
        r2_dots[i].spd = random(4) + 1;
        r2_dots[i].dT = r2_dots[i].spd;
      }
    }
    putPixel(r2_dots[i].x, r2_dots[i].y, CRGB(255, 255, 255));
    putPixel(r2_dots[i].x, r2_dots[i].y - 1, CRGB(95, 95, 95));
    putPixel(r2_dots[i].x, r2_dots[i].y - 2, CRGB(10, 10, 10));
  }
#else
  runWave(T);
#endif
}

void LEDEffector::runRainV4_mc(unsigned long T) {
#ifndef TV_LEDS
  zeroLEDS();
  for (uint8_t i = 0; i < ROWS_COUNT; i++) {
    r4_dots[i].dT -= 1;
    if (r4_dots[i].dT < 0) {
      r4_dots[i].dT = r4_dots[i].spd;
      r4_dots[i].y++;
      if (r4_dots[i].y > (leds_per_col_count[i] + 1)) {
        r4_dots[i].y = 0;
        r4_dots[i].spd = random(4) + 1;
        r4_dots[i].dT = r4_dots[i].spd;
        r4_dots[i].dotColor = CRGB(random(0xff), random(0xff), random(0xff));
      }
    }
    putPixel(r4_dots[i].x, r4_dots[i].y, r4_dots[i].dotColor);
    putPixel(r4_dots[i].x, r4_dots[i].y - 1, CRGB(r4_dots[i].dotColor.r * 0.37, r4_dots[i].dotColor.g * 0.37, r4_dots[i].dotColor.b * 0.37));
    putPixel(r4_dots[i].x, r4_dots[i].y - 2, CRGB(r4_dots[i].dotColor.r * 0.04, r4_dots[i].dotColor.g * 0.04, r4_dots[i].dotColor.b * 0.04));
  }
#else
  uint8_t K;
  uint8_t W;
  for (uint8_t i = 0; i < LEDS_COUNT; i++) {
    K = 3 - (((LEDS_COUNT - i) + T / 100) % 4);
    W = 37.5 * K * K - 27.5 * K;
    float k = W / 255.0f;
    CRGB col = getTableColor(i / 12 + T / 200);
    leds[i] = CRGB(col.r * k, col.g * k, col.b * k);
  }
#endif
}

void LEDEffector::zeroLEDS() {
  for (int i = 0; i < LEDS_COUNT; i++) {
    leds[i].r = 0;
    leds[i].g = 0;
    leds[i].b = 0;
  }
}

uint8_t LEDEffector::getMode() {
  return mode;
}

uint8_t LEDEffector::setMode(uint8_t newMode) {
  modeChanged = ((newMode != mode) || modeChanged);
  mode = newMode;
  if (newMode >= MODES_COUNT) mode = 0;
  return mode;
}

const char* LEDEffector::getModeName(uint8_t _m) {
  if (_m >= MODES_COUNT) return "";
  return mode_names[_m];
}

uint8_t LEDEffector::getModeByName(const char* mname) {
  for (uint8_t i = 0; i < MODES_COUNT; i++) {
    if (strstr(mname, mode_names[i]) && (strlen(mname) == strlen(mode_names[i]))) return i;
  }
  return 0xFF;
}

void LEDEffector::runWave(unsigned long T) {
#ifndef TV_LEDS
  uint8_t K;
  uint8_t W;
  for (uint8_t i = 0; i < max_lines; i++) {
    K = 3 - (((max_lines - i) + T / 100) % 4);
    W = 37.5 * K * K - 27.5 * K;
    setLineColor(i, CRGB(W, W, W));
  }
#else
  uint8_t K;
  uint8_t W;
  for (uint8_t i = 0; i < LEDS_COUNT; i++) {
    K = 3 - (((LEDS_COUNT - i) + T / 100) % 4);
    W = 37.5 * K * K - 27.5 * K;
    leds[i] = CRGB(W, W, W);
  }
#endif
}

void LEDEffector::runWaveR(unsigned long T) {
#ifndef TV_LEDS
  uint8_t K;
  uint8_t W;
  for (uint8_t i = 0; i < max_lines; i++) {
    K = 3 - (((max_lines - i) + T / 100) % 4);
    W = 37.5 * K * K - 27.5 * K;
    setLineColor(i, CRGB(W, 0, 0));
  }
#else
  uint8_t K;
  uint8_t W;
  for (uint8_t i = 0; i < LEDS_COUNT; i++) {
    K = 3 - (((LEDS_COUNT - i) + T / 100) % 4);
    W = 37.5 * K * K - 27.5 * K;
    leds[i] = CRGB(W, 0, 0);
  }
#endif
}

void LEDEffector::runWaveG(unsigned long T) {
#ifndef TV_LEDS
  uint8_t K;
  uint8_t W;
  for (uint8_t i = 0; i < max_lines; i++) {
    K = 3 - (((max_lines - i) + T / 100) % 4);
    W = 37.5 * K * K - 27.5 * K;
    setLineColor(i, CRGB(0, W, 0));
  }
#else
  uint8_t K;
  uint8_t W;
  for (uint8_t i = 0; i < LEDS_COUNT; i++) {
    K = 3 - (((LEDS_COUNT - i) + T / 100) % 4);
    W = 37.5 * K * K - 27.5 * K;
    leds[i] = CRGB(0, W, 0);
  }
#endif
}

void LEDEffector::runWaveB(unsigned long T) {
#ifndef TV_LEDS
  uint8_t K;
  uint8_t W;
  for (uint8_t i = 0; i < max_lines; i++) {
    K = 3 - (((max_lines - i) + T / 100) % 4);
    W = 37.5 * K * K - 27.5 * K;
    setLineColor(i, CRGB(0, 0, W));
  }
#else
  uint8_t K;
  uint8_t W;
  for (uint8_t i = 0; i < LEDS_COUNT; i++) {
    K = 3 - (((LEDS_COUNT - i) + T / 100) % 4);
    W = 37.5 * K * K - 27.5 * K;
    leds[i] = CRGB(0, 0, W);
  }
#endif
}

uint8_t LEDEffector::incMode() {
  modeChanged = true;
  if (++mode == MODES_COUNT)
    mode = 0;
  return mode;
}

uint8_t LEDEffector::decMode() {
  modeChanged = true;
  if (mode == 0xFF)
    mode = MODES_COUNT;
  return --mode;
}

bool LEDEffector::getModeChanged() {
  if (modeChanged) {
    modeChanged = false;
    return true;
  }
  return false;
}

void LEDEffector::runOFF() {
  zeroLEDS();
}

void LEDEffector::runR() {
  for (uint8_t i = 0; i < LEDS_COUNT; i++) {
    leds[i].r = 0xff;
    leds[i].g = 0;
    leds[i].b = 0;
  }
}

void LEDEffector::runG() {
  for (uint8_t i = 0; i < LEDS_COUNT; i++) {
    leds[i].r = 0;
    leds[i].g = 0xff;
    leds[i].b = 0;
  }
}

void LEDEffector::runB() {
  for (uint8_t i = 0; i < LEDS_COUNT; i++) {
    leds[i].r = 0;
    leds[i].g = 0;
    leds[i].b = 0xff;
  }
}

float LEDEffector::getCurrent() {
  float res = 0.0f;
  for (uint8_t i = 0; i < LEDS_COUNT; i++) {
    res += (leds[i].r * 0.02f) / 255.0f;
    res += (leds[i].g * 0.02f) / 255.0f;
    res += (leds[i].b * 0.02f) / 255.0f;
  }
  return res;
}

void LEDEffector::runHSV(unsigned long T) {
  for (int i = 0; i < LEDS_COUNT; i++) {
    unsigned long H = T / 25;
    H = H % 255;
    leds[i] = CHSV(H, 255, 255);
  }
}

void LEDEffector::runLights(unsigned long T) {
  for (int i = 0; i < LEDS_COUNT; i++) {
    unsigned long H = i + T / 25;
    H = H % 255;
    leds[i] = CHSV(H, 255, 255);
  }
}

void LEDEffector::runLightsHSV2(unsigned long T) {
  for (int i = 0; i < LEDS_COUNT; i++) {
    unsigned long H = (i + T / 25) % 0xFF;
    if (i % 2 == 0) H = 255 - ((i + T / 15) % 0xFF);
    leds[i] = CHSV(H, 255, 255);
  }
}

void LEDEffector::runLightsValue(unsigned long T) {
  for (int i = 0; i < LEDS_COUNT; i++) {
    unsigned long H = i + T / 10;
    unsigned long V = (3 * i + T / 20) % 255;
    H = H % 255;
    leds[i] = CHSV(H, 255, V);
  }
}

void LEDEffector::runLightsSaturation(unsigned long T) {
  for (int i = 0; i < LEDS_COUNT; i++) {
    unsigned long H = i + T / 10;
    unsigned long S = (3 * i + T / 20) % 255;
    H = H % 255;
    leds[i] = CHSV(H, S, 255);
  }
}

void LEDEffector::runFlag_v1() {
#ifndef TV_LEDS
  memset(leds, 0, sizeof(CRGB) * LEDS_COUNT);

  for (uint8_t i = 0; i < FLAG_LINES_PER_COLOR; i++) {
    setLineColor(i, CRGB(0xff, 0xff, 0xff));
  }

  for (uint8_t i = FLAG_LINES_PER_COLOR; i < 2 * FLAG_LINES_PER_COLOR; i++) {
    setLineColor(i, CRGB(0x00, 0x00, 0xff));
  }

  for (uint8_t i = FLAG_LINES_PER_COLOR * 2; i < max_lines; i++) {
    setLineColor(i, CRGB(0xff, 0x00, 0x00));
  }
#else
  uint8_t leds_per_col = LEDS_COUNT / 6;
  memset(leds, 0, sizeof(CRGB) * LEDS_COUNT);
  for (uint8_t i = 0; i < leds_per_col; i++) {
    leds[i] = CRGB(0xFF, 0xFF, 0xFF);
    leds[i + leds_per_col] = CRGB(0x00, 0x00, 0xFF);

    leds[i + LEDS_COUNT / 2] = leds[i];
    leds[i + leds_per_col + LEDS_COUNT / 2] = leds[i + leds_per_col];
  }
  for (uint8_t i = 2 * leds_per_col; i < LEDS_COUNT / 2; i++) {
    leds[i] = CRGB(0xFF, 0x00, 0x00);
    leds[i + LEDS_COUNT / 2] = leds[i];
  }
#endif
}

void LEDEffector::runFlag_v2(unsigned long T) {
#ifndef TV_LEDS
  memset(leds, 0, sizeof(CRGB) * LEDS_COUNT);

  uint8_t c;

  for (uint8_t i = 0; i < FLAG_LINES_PER_COLOR; i++) {
    c = (i * 64 + T / 3) % 255;
    setLineColor(i, CRGB(c, c, c));
    setLineColor(i + FLAG_LINES_PER_COLOR, CRGB(0, 0, c));
  }

  for (uint8_t i = FLAG_LINES_PER_COLOR * 2; i < max_lines; i++) {
    c = ((i - FLAG_LINES_PER_COLOR * 2) * 64 + T / 3) % 255;
    setLineColor(i, CRGB(c, 0x00, 0x00));
  }
#else
  uint8_t leds_per_col = LEDS_COUNT / 6;
  uint8_t C;
  memset(leds, 0, sizeof(CRGB) * LEDS_COUNT);
  for (int i = 0; i < leds_per_col; i++) {
    C = (i + T / 10) % 256;
    leds[i] = CRGB(C, C, C);
    leds[i + LEDS_COUNT / 2] = leds[i];

    leds[i + leds_per_col] = CRGB(0x00, 0x00, C);
    leds[i + leds_per_col + LEDS_COUNT / 2] = leds[i + leds_per_col];
  }
  for (int i = 2 * leds_per_col; i < LEDS_COUNT / 2; i++) {
    C = ((i - 2 * leds_per_col) + T / 10) % 256;
    leds[i] = CRGB(C, 0, 0);
    leds[i + LEDS_COUNT / 2] = leds[i];
  }
#endif
}

void LEDEffector::runFire(unsigned long T) {
#ifndef TV_LEDS
  for (int i = 0; i < ROWS_COUNT; i++) {
    int pn = inoise8(30 * i, T / 2);
    setRowColor(i, CHSV(constrain(map(pn, 0, 255, 7, 30), 0, 255), constrain(map(pn, 0, 255, 255, 245), 0, 255), constrain(map(pn, 0, 255, 70, 255), 0, 255)), false, true);
  }
#else
  for (int i = 0; i < LEDS_COUNT; i++) {
    int pn = inoise8(30 * i, T / 2);
    leds[i] = CHSV(constrain(map(pn, 0, 255, 7, 30), 0, 255), constrain(map(pn, 0, 255, 255, 245), 0, 255), constrain(map(pn, 0, 255, 70, 255), 0, 255));
  }
#endif
}

void LEDEffector::runFire2(unsigned long T) {
#ifndef TV_LEDS
  for (int i = 0; i < ROWS_COUNT; i++) {
    int pn = inoise8(30 * i, T / 2);
    setRowColor(i, CHSV(constrain(map(pn, 0, 255, 113, 48), 0, 255), constrain(map(pn, 0, 255, 255, 245), 0, 255), constrain(map(pn, 0, 255, 70, 255), 0, 255)), false, true);
  }
#else
  for (int i = 0; i < LEDS_COUNT; i++) {
    int pn = inoise8(30 * i, T / 2);
    leds[i] = CHSV(constrain(map(pn, 0, 255, 113, 48), 0, 255), constrain(map(pn, 0, 255, 255, 245), 0, 255), constrain(map(pn, 0, 255, 70, 255), 0, 255));
  }
#endif
}

void LEDEffector::runFire3(unsigned long T) {
#ifndef TV_LEDS
  for (int i = 0; i < ROWS_COUNT; i++) {
    int pn = inoise8(30 * i, T / 2);
    setRowColor(i, CHSV(constrain(map(pn, 0, 255, 124, 183), 0, 255), constrain(map(pn, 0, 255, 255, 200), 0, 255), constrain(map(pn, 0, 255, 0, 255), 0, 255)), false, true);
  }
#else
  for (int i = 0; i < LEDS_COUNT; i++) {
    int pn = inoise8(30 * i, T / 2);
    leds[i] = CHSV(constrain(map(pn, 0, 255, 124, 183), 0, 255), constrain(map(pn, 0, 255, 255, 245), 0, 255), constrain(map(pn, 0, 255, 70, 255), 0, 255));
  }
#endif
}

void LEDEffector::runFire4(unsigned long T) {
#ifndef TV_LEDS
  for (int i = 0; i < ROWS_COUNT; i++) {
    int pn = inoise8(30 * i, T / 2);
    setRowColor(i, CHSV(constrain(map(pn, 0, 255, 192, 231), 0, 255), constrain(map(pn, 0, 255, 255, 245), 0, 255), constrain(map(pn, 0, 255, 0, 255), 0, 255)), false, true);
  }
#else
  for (int i = 0; i < LEDS_COUNT; i++) {
    int pn = inoise8(30 * i, T / 2);
    leds[i] = CHSV(constrain(map(pn, 0, 255, 192, 231), 0, 255), constrain(map(pn, 0, 255, 255, 245), 0, 255), constrain(map(pn, 0, 255, 70, 255), 0, 255));
  }
#endif
}

#ifndef TV_LEDS
void LEDEffector::setRowColor(uint8_t lineNum, CRGB color, bool zeroOther, bool fadeToZero) {
  uint8_t idxStart = 0;
  uint8_t idxEnd = 0;
  if (zeroOther) zeroLEDS();
  lineNum = lineNum % ROWS_COUNT;

  for (int i = 0; i < lineNum; i++)
    idxStart += leds_per_col_count[i];

  idxEnd = idxStart + leds_per_col_count[lineNum] - 1;

  if (!fadeToZero) {
    for (int i = idxStart; i <= idxEnd; i++) {
      leds[i] = color;
    }
  } else {
    for (int i = idxStart; i <= idxEnd; i++) {
      leds[i].r = map(i, idxStart, idxEnd + 1, color.r, 0);
      leds[i].g = map(i, idxStart, idxEnd + 1, color.g, 0);
      leds[i].b = map(i, idxStart, idxEnd + 1, color.b, 0);
    }
  }
}

void LEDEffector::setLineColor(uint8_t lineNum, CRGB color) {
  if (lineNum >= max_lines) return;
  uint8_t idx = 0;
  for (uint8_t i = 0; i < ROWS_COUNT; i++) {
    if (lineNum < leds_per_col_count[i]) {
      leds[idx + lineNum] = color;
    }
    idx += leds_per_col_count[i];
  }
}
#endif

void LEDEffector::runWhite() {
  for (uint8_t i = 0; i < LEDS_COUNT; i++) {
    leds[i].r = 255;
    leds[i].g = 255;
    leds[i].b = 255;
  }
}

void LEDEffector::runRainbow(unsigned long T) {
#ifndef TV_LEDS
  for (uint8_t i = 0; i < max_lines; i++) {
  #ifdef HALL_LEDS
    setLineColor((i + T / 250) % max_lines, getColor(i / 2));
  #else
    setLineColor((i + T / 250) % max_lines, getColor(i));
  #endif
  }
#else
  uint8_t lights_per_col = LEDS_COUNT / 14;
  for (uint8_t i = 0; i < 6; i++) {
    for (uint8_t j = lights_per_col * i; j < lights_per_col * (i + 1); j++) {
      leds[(LEDS_COUNT - j - 1 + T / 250) % (LEDS_COUNT/2)] = getColor(6 - i);
      leds[(LEDS_COUNT - j - 1 + T / 250) % (LEDS_COUNT/2) +(LEDS_COUNT/2)] = leds[(LEDS_COUNT - j - 1 + T / 250) % (LEDS_COUNT/2)];
    }
  }

  for (uint8_t i = lights_per_col * 6; i < LEDS_COUNT/2; i++){
    leds[(LEDS_COUNT - i - 1 + T / 250) % (LEDS_COUNT/2)] = getColor(0);
    leds[(LEDS_COUNT - i - 1 + T / 250) % (LEDS_COUNT/2) + (LEDS_COUNT/2)] = leds[(LEDS_COUNT - i - 1 + T / 250) % (LEDS_COUNT/2)];
  }
#endif
}

void LEDEffector::runStaticRainbow() {
#ifndef TV_LEDS
  for (uint8_t i = 0; i < max_lines; i++) {
  #ifdef HALL_LEDS
    setLineColor(i % max_lines, getColor(i / 2));
  #else
    setLineColor(i % max_lines, getColor(MIN(i,6)));
  #endif
  }
#else
  uint8_t lines_per_color = LEDS_COUNT / 14;
  for (uint8_t i = 0; i < 6; i++) {
    for (uint8_t j = 0; j < lines_per_color; j++) {
      uint8_t idx = i * lines_per_color + j;
      leds[idx] = getColor(i);
      leds[idx + (LEDS_COUNT / 2)] = leds[idx];
    }
  }
  for (uint8_t i = 6 * lines_per_color; i < LEDS_COUNT / 2; i++) {
    leds[i] = getColor(6);
    leds[i + (LEDS_COUNT / 2)] = leds[i];
  }
#endif
}

CRGB LEDEffector::getColor(uint8_t colIdx) {
  colIdx = colIdx % 7;
  CRGB res;
  switch (colIdx) {
    case 0:
      res.r = 255;
      res.g = 0;
      res.b = 0;
      break;
    case 1:
      res.r = 255;
      res.g = 128;
      res.b = 0;
      break;
    case 2:
      res.r = 255;
      res.g = 255;
      res.b = 0;
      break;
    case 3:
      res.r = 0;
      res.g = 255;
      res.b = 0;
      break;
    case 4:
      res.r = 0;
      res.g = 255;
      res.b = 255;
      break;
    case 5:
      res.r = 0;
      res.g = 0;
      res.b = 255;
      break;
    case 6:
      res.r = 255;
      res.g = 0;
      res.b = 255;
      break;
  }
  return res;
}

#ifndef TV_LEDS
PointDir LEDEffector::tryPoint(int8_t x, int8_t y) {
  PointDir res;
  res.x = 0;
  res.y = 0;
  if (y < 0) res.y = -1;

  if (x >= ROWS_COUNT) {
    res.x = 1;
  } else if (x < 0) {
    res.x = -1;
  } else {
    res.x = 0;
    if (y >= leds_per_col_count[y]) res.y = 1;
  }

  return res;
}

void LEDEffector::putPixel(uint8_t x, uint8_t y, CRGB col) {
  uint8_t idx = 0;
  if (x < ROWS_COUNT) {
    for (uint8_t i = 0; i < x; i++) {
      idx += leds_per_col_count[i];
    }
    if (y < leds_per_col_count[x]) {
      idx += y;
      leds[idx] = col;
    }
  }
}

uint8_t LEDEffector::getPixelIdx(int8_t x, int8_t y) {
  uint8_t idx = 0xff;
  if ((x < ROWS_COUNT) && (x >= 0)) {
    for (uint8_t i = 0; i < x; i++) {
      idx += leds_per_col_count[i];
    }
    if ((y < leds_per_col_count[x]) && (y >= 0)) {
      idx += y;
    } else {
      idx = 0xff;
    }
  }
  return idx;
}

void LEDEffector::putPixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
  uint8_t idx = 0;
  if (x < ROWS_COUNT) {
    for (uint8_t i = 0; i < x; i++) {
      idx += leds_per_col_count[i];
    }
    if (y < leds_per_col_count[x]) {
      idx += y;
      leds[idx].r = r;
      leds[idx].g = g;
      leds[idx].b = b;
    }
  }
}

void LEDEffector::addPixel(uint8_t x, uint8_t y, CRGB col) {
  uint8_t idx = 0;
  if (x < ROWS_COUNT) {
    for (uint8_t i = 0; i < x; i++) {
      idx += leds_per_col_count[i];
    }
    if (y < leds_per_col_count[x]) {
      idx += y;
      leds[idx].r = MAX(col.r + leds[idx].r, 255);
      leds[idx].g = MAX(col.g + leds[idx].g, 255);
      leds[idx].b = MAX(col.b + leds[idx].b, 255);
    }
  }
}

void LEDEffector::addPixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
  uint8_t idx = 0;
  if (x < ROWS_COUNT) {
    for (uint8_t i = 0; i < x; i++) {
      idx += leds_per_col_count[i];
    }
    if (y < leds_per_col_count[x]) {
      idx += y;
      leds[idx].r = MAX(r + leds[idx].r, 255);
      leds[idx].g = MAX(g + leds[idx].g, 255);
      leds[idx].b = MAX(b + leds[idx].b, 255);
    }
  }
}
#endif

void LEDEffector::runTree(unsigned long T) {
  zeroLEDS();

#ifdef HALL_LEDS
  putPixel(3, 0, 0, 63 + ((T / 5) % 192), 0);
  putPixel(4, 0, 0, 63 + ((T / 5) % 192), 0);

  putPixel(2, 1, 0, 63 + (((T + 250) / 5) % 192), 0);
  putPixel(3, 1, 0, 63 + (((T + 250) / 5) % 192), 0);
  putPixel(4, 1, 0, 63 + (((T + 250) / 5) % 192), 0);
  putPixel(5, 1, 0, 63 + (((T + 250) / 5) % 192), 0);

  putPixel(3, 2, 0, 63 + (((T + 500) / 5) % 192), 0);
  putPixel(4, 2, 0, 63 + (((T + 500) / 5) % 192), 0);

  putPixel(2, 3, 0, 63 + (((T + 750) / 5) % 192), 0);
  putPixel(3, 3, 0, 63 + (((T + 750) / 5) % 192), 0);
  putPixel(4, 3, 0, 63 + (((T + 750) / 5) % 192), 0);
  putPixel(5, 3, 0, 63 + (((T + 750) / 5) % 192), 0);

  putPixel(1, 4, 0, 63 + (((T + 1000) / 5) % 192), 0);
  putPixel(2, 4, 0, 63 + (((T + 1000) / 5) % 192), 0);
  putPixel(3, 4, 0, 63 + (((T + 1000) / 5) % 192), 0);
  putPixel(4, 4, 0, 63 + (((T + 1000) / 5) % 192), 0);
  putPixel(5, 4, 0, 63 + (((T + 1000) / 5) % 192), 0);
  putPixel(6, 4, 0, 63 + (((T + 1000) / 5) % 192), 0);

  putPixel(3, 5, 0, 63 + (((T + 1250) / 5) % 192), 0);
  putPixel(4, 5, 0, 63 + (((T + 1250) / 5) % 192), 0);

  putPixel(2, 6, 0, 63 + (((T + 1500) / 5) % 192), 0);
  putPixel(3, 6, 0, 63 + (((T + 1500) / 5) % 192), 0);
  putPixel(4, 6, 0, 63 + (((T + 1500) / 5) % 192), 0);
  putPixel(5, 6, 0, 63 + (((T + 1500) / 5) % 192), 0);

  putPixel(1, 7, 0, 63 + (((T + 1750) / 5) % 192), 0);
  putPixel(2, 7, 0, 63 + (((T + 1750) / 5) % 192), 0);
  putPixel(3, 7, 0, 63 + (((T + 1750) / 5) % 192), 0);
  putPixel(4, 7, 0, 63 + (((T + 1750) / 5) % 192), 0);
  putPixel(5, 7, 0, 63 + (((T + 1750) / 5) % 192), 0);
  putPixel(6, 7, 0, 63 + (((T + 1750) / 5) % 192), 0);

  putPixel(0, 8, 0, 63 + (((T + 2000) / 5) % 192), 0);
  putPixel(1, 8, 0, 63 + (((T + 2000) / 5) % 192), 0);
  putPixel(2, 8, 0, 63 + (((T + 2000) / 5) % 192), 0);
  putPixel(3, 8, 0, 63 + (((T + 2000) / 5) % 192), 0);
  putPixel(4, 8, 0, 63 + (((T + 2000) / 5) % 192), 0);
  putPixel(5, 8, 0, 63 + (((T + 2000) / 5) % 192), 0);
  putPixel(6, 8, 0, 63 + (((T + 2000) / 5) % 192), 0);
  putPixel(7, 8, 0, 63 + (((T + 2000) / 5) % 192), 0);

  putPixel(3, 9, 128, 64, 48);
  putPixel(4, 9, 128, 64, 48);

  putPixel(3, 10, 128, 64, 48);
  putPixel(4, 10, 128, 64, 48);

  putPixel(3, 11, 128, 64, 48);
  putPixel(4, 11, 128, 64, 48);

  putPixel(3, 12, 128, 64, 48);
  putPixel(4, 12, 128, 64, 48);

  putPixel(3, 13, 128, 64, 48);
  putPixel(4, 13, 128, 64, 48);
#else
#ifndef TV_LEDS
  putPixel(4, 0, 0, 63 + ((T / 5) % 192), 0);

  putPixel(3, 1, 0, 63 + (((T + 250) / 5) % 192), 0);
  putPixel(4, 1, 0, 63 + (((T + 250) / 5) % 192), 0);
  putPixel(5, 1, 0, 63 + (((T + 250) / 5) % 192), 0);

  putPixel(4, 2, 0, 63 + (((T + 500) / 5) % 192), 0);

  putPixel(3, 3, 0, 63 + (((T + 750) / 5) % 192), 0);
  putPixel(4, 3, 0, 63 + (((T + 750) / 5) % 192), 0);
  putPixel(5, 3, 0, 63 + (((T + 750) / 5) % 192), 0);

  putPixel(2, 4, 0, 63 + (((T + 1000) / 5) % 192), 0);
  putPixel(3, 4, 0, 63 + (((T + 1000) / 5) % 192), 0);
  putPixel(4, 4, 0, 63 + (((T + 1000) / 5) % 192), 0);
  putPixel(5, 4, 0, 63 + (((T + 1000) / 5) % 192), 0);
  putPixel(6, 4, 0, 63 + (((T + 1000) / 5) % 192), 0);

  putPixel(4, 5, 0, 63 + (((T + 1250) / 5) % 192), 0);

  putPixel(3, 6, 0, 63 + (((T + 1500) / 5) % 192), 0);
  putPixel(4, 6, 0, 63 + (((T + 1500) / 5) % 192), 0);
  putPixel(5, 6, 0, 63 + (((T + 1500) / 5) % 192), 0);

  putPixel(3, 7, 0, 63 + (((T + 1750) / 5) % 192), 0);
  putPixel(4, 7, 0, 63 + (((T + 1750) / 5) % 192), 0);
  putPixel(5, 7, 0, 63 + (((T + 1750) / 5) % 192), 0);
  putPixel(5, 7, 0, 63 + (((T + 1750) / 5) % 192), 0);

  putPixel(4, 7, 128, 64, 48);
  putPixel(4, 8, 128, 64, 48);
  putPixel(4, 9, 128, 64, 48);
#else
  runWaveG(T);
#endif
#endif
}