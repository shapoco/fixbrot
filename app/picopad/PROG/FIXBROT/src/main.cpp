#include "../include.h"

#include "fixbrot/fixbrot.hpp"

using namespace fixbrot;

uint64_t now_us = 0;
bool ctrl_pressed = false;

App<WIDTH, HEIGHT> app;
Engine<WIDTH * 8, WIDTH * 8> engine0;
Engine<WIDTH * 8, WIDTH * 8> engine1;
col_t line_buff[WIDTH];

uint64_t t_start = 0;
uint64_t t_elapsed = 0;

void core1_main();
void paint();

int main() {
  app.init();
  now_us = Time64();

  Core1Exec(core1_main);

  while (True) {
    uint64_t last_us = now_us;
    now_us = Time64();
    uint32_t delta_us = (uint32_t)(now_us - last_us);

    input_t keys = input_t::NONE;
    if (ctrl_pressed) {
      if (KeyPressedFast(KEY_LEFT)) {
        keys |= input_t::CHANGE_SLOPE;
      }
      if (KeyPressedFast(KEY_RIGHT)) {
        keys |= input_t::CHANGE_PATTERN;
      }
      if (KeyPressedFast(KEY_DOWN)) {
        keys |= input_t::ITER_DEC;
      }
      if (KeyPressedFast(KEY_UP)) {
        keys |= input_t::ITER_INC;
      }
    } else {
      if (KeyPressedFast(KEY_LEFT)) {
        keys |= input_t::SCROLL_LEFT;
      }
      if (KeyPressedFast(KEY_RIGHT)) {
        keys |= input_t::SCROLL_RIGHT;
      }
      if (KeyPressedFast(KEY_DOWN)) {
        keys |= input_t::SCROLL_DOWN;
      }
      if (KeyPressedFast(KEY_UP)) {
        keys |= input_t::SCROLL_UP;
      }
    }

    if (KeyPressedFast(KEY_A)) {
      keys |= input_t::ZOOM_IN;
    }
    if (KeyPressedFast(KEY_B)) {
      keys |= input_t::ZOOM_OUT;
    }

    if (KeyPressedFast(KEY_X)) {
      ctrl_pressed = true;
    } else if (keys == input_t::NONE) {
      ctrl_pressed = false;
    }

    if (KeyPressedFast(KEY_Y)) {
      ResetToBootLoader();
    }

    if (app.service(delta_us, keys) != result_t::SUCCESS) {
      LedFlip(LED1);
    }
    paint();

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

void paint() {
  if (app.is_repaint_requested()) {
    app.paint_start();
    for (pos_t y = 0; y < HEIGHT; y++) {
      app.paint_line(y, line_buff);
      if (y > 0) DispStopImg();
      DispStartImg(0, WIDTH, y, y + 1);
      for (pos_t x = 0; x < WIDTH; x++) {
        DispSendImg2(line_buff[x]);
      }
    }
    DispStopImg();
    app.paint_finished();
  }
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
