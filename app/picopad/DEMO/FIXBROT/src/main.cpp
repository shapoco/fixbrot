
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "../include.h"

#include "fixbrot/fixbrot.hpp"

using namespace fixbrot;

uint64_t now_us = 0;
App<WIDTH, HEIGHT> app;

int main() {
  app.init();
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

    app.service(delta_us, keys);
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

void fixbrot::compute_started() { LedOn(LED1); }

void fixbrot::compute_finished(result_t res) { LedOff(LED1); }