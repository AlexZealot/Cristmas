#include "LEDEffector.h"

LEDEffector ledEffector;

#define MAX(a, b) (a>b)?a:b
#define MIN(a, b) (a<b)?a:b

CRGB    col_table[7];

LEDEffector::LEDEffector() {
}

void LEDEffector::init() {
  #ifdef TEST_MODE
    test_lineIdx = 0;
  #endif

  memset(leds, 0, sizeof(CRGB) * LEDS_COUNT);
  FastLED.addLeds<WS2812, PIN_LED, GRB>(leds, LEDS_COUNT).setCorrection(TypicalLEDStrip);
  setBrightness(255);
  zeroLEDS();
  mode = MODE_OFF;

  for (uint8_t i = 0; i < FLYING_DOTS_COUNT; i++){
    dots[i].start_point = random(LEDS_COUNT);
    dots[i].end_point = dots[i].start_point + random(10) + 5;
    dots[i].spd = random(5) + 5;
    dots[i].spdTick = dots[i].spd;
    dots[i].col = CRGB((62*i)%255, (62*(i+1))%255, (62*(i+2))%255);
  }

  for (uint8_t i = 0; i < 7; i++){
    col_table[i] = CRGB((62*i)%255, (62*(i+1))%255, (62*(i+2))%255);
  }

  dT = 1000 / FPS;
  lastUpd = millis();
  modeChanged = true;
}

void  LEDEffector::runFlyingDots(unsigned long T){
  zeroLEDS();
  for (uint8_t i = 0; i < FLYING_DOTS_COUNT; i++){
    addPixel(dots[i].start_point, dots[i].col);
    addPixel(dots[i].start_point-1, CRGB(dots[i].col.r * 0.5, dots[i].col.g * 0.5, dots[i].col.b * 0.5));
    addPixel(dots[i].start_point-2, CRGB(dots[i].col.r * 0.1, dots[i].col.g * 0.1, dots[i].col.b * 0.1));

    dots[i].spdTick--;
    if (dots[i].spdTick == 0){
      dots[i].start_point++;
      dots[i].spdTick = dots[i].spd;

      if (dots[i].start_point == dots[i].end_point){
        dots[i].start_point = random(LEDS_COUNT);
        dots[i].end_point = dots[i].start_point + random(10) + 5;
      }
    }
  }
}

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
      case MODE_FL_DOTS: runFlyingDots(T); break;
      case MODE_FLAG_DYN: runFlag_v2(T); break;
      case MODE_HSV_DIF: runLightsHSV2(T); break;
      case MODE_FIRE_1: runFire1(T); break;
      case MODE_FIRE_2: runFire2(T); break;
      case MODE_FIRE_3: runFire3(T); break;
    }
    lastUpd = T;
  }

  float cI = getCurrent() * 1000.0f;
  if (cI > MAX_CURRENT_MA)
    setBrightness((MAX_CURRENT_MA)*255 / cI);
  else
    if (br != 0xFF)
      setBrightness(0xFF);

  FastLED.show();  // записываем цвета в ленту
}

void  LEDEffector::runFire1(unsigned long T){
  for (int i = 0; i < LEDS_COUNT; i++) {
    int pn = inoise8(30*i, T/2);
    leds[i] = CHSV(constrain(map(pn, 0, 255, 113, 48), 0, 255), constrain(map(pn, 0, 255, 255, 245), 0, 255), constrain(map(pn, 0, 255, 70, 255), 0, 255));
  }
}

void  LEDEffector::runFire2(unsigned long T){
  for (int i = 0; i < LEDS_COUNT; i++) {
    int pn = inoise8(30*i, T/2);
    leds[i] = CHSV(constrain(map(pn, 0, 255, 124, 183), 0, 255), constrain(map(pn, 0, 255, 255, 200), 0, 255), constrain(map(pn, 0, 255, 0, 255), 0, 255));
  }
}

void  LEDEffector::runFire3(unsigned long T){
  for (int i = 0; i < LEDS_COUNT; i++) {
    int pn = inoise8(30*i, T/2);
    leds[i] = CHSV(constrain(map(pn, 0, 255, 192, 231), 0, 255), constrain(map(pn, 0, 255, 255, 245), 0, 255), constrain(map(pn, 0, 255, 0, 255), 0, 255));
  }
}

void LEDEffector::runRainV1(unsigned long T) {
  runWave(T);
}

void LEDEffector::runRainV3_mc(unsigned long T) {
  uint8_t K;
  uint8_t W;
  for (uint8_t i = 0; i < LEDS_COUNT; i++){
    K = 3 - (((LEDS_COUNT - i) + T / 100) % 4);
    W = 37.5 * K * K - 27.5 * K;
    float k = W / 255.0f;
    CRGB col = getTableColor(i/12);
    leds[LEDS_COUNT - 1 - i] = CRGB(col.r * k, col.g * k, col.b * k);
  }
}

void LEDEffector::runRainV2(unsigned long T) {
  runWave(T);
}

void LEDEffector::runRainV4_mc(unsigned long T) {
  uint8_t K;
  uint8_t W;
  for (uint8_t i = 0; i < LEDS_COUNT; i++){
    K = 3 - (((LEDS_COUNT - i) + T / 100) % 4);
    W = 37.5 * K * K - 27.5 * K;
    float k = W / 255.0f;
    CRGB col = getTableColor(i/12 + T/200);
    leds[LEDS_COUNT - 1 - i] = CRGB(col.r * k, col.g * k, col.b * k);
  }
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
  uint8_t K;
  uint8_t W;
  for (uint8_t i = 0; i < LEDS_COUNT; i++){
    K = 3 - (((LEDS_COUNT - i) + T / 100) % 4);
    W = 37.5 * K * K - 27.5 * K;
    leds[LEDS_COUNT - 1 - i] = CRGB(W, W, W);
  }
}

void LEDEffector::runWaveR(unsigned long T) {
  uint8_t K;
  uint8_t W;
  for (uint8_t i = 0; i < LEDS_COUNT; i++){
    K = 3 - (((LEDS_COUNT - i) + T / 100) % 4);
    W = 37.5 * K * K - 27.5 * K;
    leds[LEDS_COUNT - 1 - i] = CRGB(W, 0, 0);
  }
}

void LEDEffector::runWaveG(unsigned long T) {
  uint8_t K;
  uint8_t W;
  for (uint8_t i = 0; i < LEDS_COUNT; i++){
    K = 3 - (((LEDS_COUNT - i) + T / 100) % 4);
    W = 37.5 * K * K - 27.5 * K;
    leds[LEDS_COUNT - 1 - i] = CRGB(0, W, 0);
  }
}

void LEDEffector::runWaveB(unsigned long T) {
  uint8_t K;
  uint8_t W;
  for (uint8_t i = 0; i < LEDS_COUNT; i++){
    K = 3 - (((LEDS_COUNT - i) + T / 100) % 4);
    W = 37.5 * K * K - 27.5 * K;
    leds[LEDS_COUNT - 1 - i] = CRGB(0, 0, W);
  }
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

void  LEDEffector::runLightsHSV2(unsigned long T){
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
  uint8_t leds_per_col = LEDS_COUNT / 3;
  memset(leds, 0, sizeof(CRGB) * LEDS_COUNT);
  for (uint8_t i = 0; i < leds_per_col; i++){
    leds[i] = CRGB(0xFF, 0x00, 0x00);
    leds[i + leds_per_col] = CRGB(0x00, 0x00, 0xFF);
  }
  for (uint8_t i = 2*leds_per_col; i < LEDS_COUNT; i++){
    leds[i] = CRGB(0xFF, 0xFF, 0xFF);
  }
}

void LEDEffector::runFlag_v2(unsigned long T) {
  uint8_t leds_per_col = LEDS_COUNT / 3;
  uint8_t C;
  memset(leds, 0, sizeof(CRGB) * LEDS_COUNT);
  for (int i = 0; i < leds_per_col; i++){
    C = (i + T / 10) % 256;
    leds[i] = CRGB(C, 0x00, 0x00);
    leds[i + leds_per_col] = CRGB(0x00, 0x00, C);
  }
  for (int i = 2*leds_per_col; i < LEDS_COUNT; i++){
    C = ((i - leds_per_col) + T / 10) % 256;
    leds[i] = CRGB(C, C, C);
  }
}

void LEDEffector::runFire(unsigned long T) {
  for (int i = 0; i < LEDS_COUNT; i++) {
    int pn = inoise8(30*i, T/2);
    leds[i] = CHSV(constrain(map(pn, 0, 255, 7, 30), 0, 255), constrain(map(pn, 0, 255, 255, 245), 0, 255), constrain(map(pn, 0, 255, 70, 255), 0, 255));
  }
}

void LEDEffector::runWhite() {
  for (uint8_t i = 0; i < LEDS_COUNT; i++) {
    leds[i].r = 255;
    leds[i].g = 255;
    leds[i].b = 255;
  }
}

void LEDEffector::runRainbow(unsigned long T) {
  uint8_t lights_per_col = LEDS_COUNT / 7;
  for (uint8_t i = 0; i < 6; i++){
    for (uint8_t j = lights_per_col*i; j < lights_per_col*(i+1); j++){
      leds[(LEDS_COUNT - j - 1 + T/250) % LEDS_COUNT] = getColor(i);
    }
  }

  for (uint8_t i = lights_per_col * 6; i < LEDS_COUNT; i++)
    leds[(LEDS_COUNT - i - 1 + T/250) % LEDS_COUNT] = getColor(6);
}

void LEDEffector::runStaticRainbow() {
  uint8_t lights_per_col = LEDS_COUNT / 7;
  for (uint8_t i = 0; i < 6; i++){
    for (uint8_t j = lights_per_col*i; j < lights_per_col*(i+1); j++){
      leds[LEDS_COUNT - j - 1] = getColor(i);
    }
  }

  for (uint8_t i = lights_per_col * 6; i < LEDS_COUNT; i++)
    leds[LEDS_COUNT - i - 1] = getColor(6);
}

CRGB  LEDEffector::getTableColor(int colIdx){
  colIdx = colIdx%7;
  return col_table[colIdx];
}

CRGB LEDEffector::getColor(int colIdx) {
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

void LEDEffector::runTree(unsigned long T) {
  runWaveG(T);
}

void  LEDEffector::addPixel(uint8_t idx, CRGB col){
  if (idx < LEDS_COUNT){
    leds[idx].r = MIN(leds[idx].r + col.r, 0xFF);
    leds[idx].g = MIN(leds[idx].g + col.g, 0xFF);
    leds[idx].b = MIN(leds[idx].b + col.b, 0xFF);
  }
}