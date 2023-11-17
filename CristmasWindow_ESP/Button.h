#ifndef __BUTTON__
#define __BUTTON__
#include <Arduino.h>

#define UPD_INTERVAL  100

class Button {
  private:
    uint32_t      lastTick;
    uint32_t      whenPressed;
    uint8_t       PIN;
    int32_t       downLeft;

    bool          state;
    bool          lastState;
  public:
    Button(int _pin);

    void          update(unsigned long T);
    bool          isDown();
    bool          isPressed();
    bool          isReleased();
    uint32_t      holdInterval(unsigned long T);
};

enum  btn_callback_t  {
  BTN_CB_RELEASE = 0,
  BTN_CB_PRESS = 1,
};

class ButtonCB {
  public:
    ButtonCB(uint8_t _pin);
    ~ButtonCB();
    void  update(uint32_t T);
    void  update();

    typedef std::function<void(uint32_t ms)> cb_btn_event_hold;
    typedef std::function<void()> cb_btn_event;

    void  setCallback(btn_callback_t  callback_type, cb_btn_event cb);
    void  setCallback(cb_btn_event_hold cb);
    bool  isDown(){return state;}

  private:
    cb_btn_event        _on_release;
    cb_btn_event        _on_press;
    cb_btn_event_hold   _on_hold;

    uint32_t      lastTick;
    uint32_t      whenPressed;
    uint8_t       PIN;
    int32_t       downLeft;

    bool          state;
    bool          lastState;       
};

#endif //__BUTTON__
