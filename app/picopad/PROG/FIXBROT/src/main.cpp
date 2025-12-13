#include "../include.h"

#include "fixbrot/fixbrot.hpp"

using namespace fixbrot;

uint64_t now_us = 0;
App<WIDTH, HEIGHT> app;
Engine<WIDTH * 4, WIDTH * 4> engine0;
Engine<WIDTH * 4, WIDTH * 4> engine1;

uint64_t t_start = 0;
uint64_t t_elapsed = 0;

void core1_main();

int main() {
  app.init();
  now_us = Time64();

  Core1Exec(core1_main);

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

    if (app.service(delta_us, keys) != result_t::SUCCESS) {
      LedFlip(LED1);
    }

    // char buf[64];
    // snprintf(buf, sizeof(buf), "Time: %llu us", t_elapsed);
    // DispDrawText(buf, 0, 0, 0, 0, 0xFFFF, 0x0000);
  }
}

void core1_main() {
  while (true) {
    engine1.service();
  }
}

result_t fixbrot::on_send_line(pos_t x, pos_t y, pos_t width,
                               const col_t *data) {
  DispStartImg(x, x + width, y, y + 1);
  for (pos_t i = 0; i < width; i++) {
    DispSendImg2(data[i]);
  }
  DispStopImg();
  return result_t::SUCCESS;
}

void fixbrot::on_render_start(scene_t &scene) {
  t_start = Time64();
  LedOn(LED1);
  engine0.init(scene);
  engine1.init(scene);
}

void fixbrot::on_render_finished(result_t res) {
  LedOff(LED1);
  t_elapsed = Time64() - t_start;
}

result_t fixbrot::on_iterate() { return engine0.service(); }

static bool use_engine0 = true;
result_t fixbrot::on_dispatch(const vec_t &loc) {
  if (use_engine0) {
    return engine0.dispatch(loc);
  } else {
    return engine1.dispatch(loc);
  }
  use_engine0 = !use_engine0;
}

bool fixbrot::on_collect(cell_t *resp) {
  if (engine0.load() >= engine1.load()) {
    if (engine0.collect(resp)) {
      return true;
    } else {
      return engine1.collect(resp);
    }
  } else {
    if (engine1.collect(resp)) {
      return true;
    } else {
      return engine0.collect(resp);
    }
  }
  return false;
}
