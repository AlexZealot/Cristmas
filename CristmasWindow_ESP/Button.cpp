#include "Button.h"
#include <Arduino.h>

/*BUTTON*/
Button::Button(int _pin){
  PIN = _pin;
  downLeft = UPD_INTERVAL;
  pinMode(PIN, INPUT_PULLUP);
  state = false;
  lastTick = millis();
  whenPressed = 0;
}

void Button::update(unsigned long T){
  lastState = state;
  int tmp = !digitalRead(PIN);
  if (tmp) {
    if (!state) {
      downLeft -= (T - lastTick);
      if (downLeft <= 0)
        state = true;
        whenPressed = T;
    }
  } else {
    state = false;
    downLeft = UPD_INTERVAL;
  }
  lastTick = T;
}

bool  Button::isDown(){
  return state;
}

bool  Button::isPressed(){
  return (state != lastState) && state;
}

bool Button::isReleased(){
  return (state != lastState) && !state;
}

uint32_t Button::holdInterval(unsigned long T){
  if (!state) return 0;
  return T - whenPressed;
}

/*CALLBACK_BUTTON*/
ButtonCB::ButtonCB(uint8_t _pin)
{
  PIN = _pin;
  downLeft = UPD_INTERVAL;
  state = false;
  whenPressed = 0;
  _on_release = NULL;
  _on_press = NULL;
  _on_hold = NULL;
  
  pinMode(PIN, INPUT_PULLUP);
  lastTick = millis();
}
ButtonCB::~ButtonCB(){}

void  ButtonCB::update(uint32_t T){
  lastState = state;
  uint8_t tmp = !digitalRead(PIN);
  if (tmp) {
    if (!state) {
      downLeft -= (T - lastTick);
      if (downLeft <= 0)
        state = true;
        whenPressed = T;
    }
  } else {
    state = false;
    downLeft = UPD_INTERVAL;
  }
  lastTick = T;

  if (((state != lastState) && state) && (_on_press != NULL))
    _on_press();

  if (((state != lastState) && !state) && (_on_release != NULL))
    _on_release();

  if (isDown() && (_on_hold != NULL)){
    _on_hold(T - whenPressed);
  }
}

void  ButtonCB::update(){
  update(millis());
}

void  ButtonCB::setCallback(cb_btn_event_hold cb){
  _on_hold = cb;
}

void  ButtonCB::setCallback(btn_callback_t  callback_type, cb_btn_event cb){
  switch (callback_type) {
    case BTN_CB_PRESS:
      _on_press = cb;
    break;

    case BTN_CB_RELEASE:
      _on_release = cb;
    break;
  }
}