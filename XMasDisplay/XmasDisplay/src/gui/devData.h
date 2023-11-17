#pragma once
#include <Arduino.h>

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