
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "../include.h"

#include "fixbrot/fixbrot.hpp"

using namespace fixbrot;

uint64_t now_us = 0;
App<WIDTH, HEIGHT> game;

// // alarm callback
// void AlarmCB(sAlarm *a) {
//   LedFlip(LED1);
//   LedFlip(LED2);
// }

int main() {
  // register alarm
  //  sAlarm a;
  //  a.time = SysTime + 100; // time of first alarm in [ms]
  //  a.delta = 500;          // delta of next alarm in [ms]
  //  a.callback = AlarmCB;   // callback function
  //  AlarmReg(&a);           // register alarm

  //  DispInit(1);
  // DispDrawText("Hello World!\n", 0, 0, 0, 0, 0xFFFF, 0x0000);

  game.init();
  now_us = Time64();

  while (True) {
    uint64_t last_us = now_us;
    now_us = Time64();
    uint32_t delta_us = (uint32_t)(now_us - last_us);

    pad_t keys = pad_t::NONE;
    if (KeyPressedFast(KEY_DOWN)) {
      keys |= pad_t::DOWN;
    }
    if (KeyPressedFast(KEY_UP)) {
      keys |= pad_t::UP;
    }
    if (KeyPressedFast(KEY_LEFT)) {
      keys |= pad_t::LEFT;
    }
    if (KeyPressedFast(KEY_RIGHT)) {
      keys |= pad_t::RIGHT;
    }
    if (KeyPressedFast(KEY_A)) {
      keys |= pad_t::ZOOM_IN;
    }
    if (KeyPressedFast(KEY_B)) {
      keys |= pad_t::ZOOM_OUT;
    }
    if (KeyPressedFast(KEY_X)) {
      keys |= pad_t::X;
    }
    if (KeyPressedFast(KEY_Y)) {
      keys |= pad_t::Y;
    }

    if (keys & pad_t::Y) {
      ResetToBootLoader();
    }

    game.service(delta_us, keys);
  }
}

result_t fixbrot::send_line(pos_t x, pos_t y, pos_t width, const col_t *data) {
  DispStartImg(x, x + width, y, y + 1);
  for (pos_t i = 0; i < width; i++) {
    DispSendImg2(data[i]);
  }
  DispStopImg();
  return result_t::SUCCESS;
}
