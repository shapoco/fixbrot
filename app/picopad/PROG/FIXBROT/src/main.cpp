#include "../include.h"

#include "fixbrot/fixbrot.hpp"

namespace fb = fixbrot;

static constexpr uint16_t NUM_ENGINES = 2;

uint64_t now_us = 0;
bool ctrl_pressed = false;

fb::App<WIDTH, HEIGHT> app;
fb::ArrayQueue<fb::vec_t, WIDTH * 32> queue;
fb::Engine engines[NUM_ENGINES];
uint16_t line_buff[WIDTH];

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

    fb::input_t keys = fb::input_t::NONE;
    if (ctrl_pressed) {
      if (KeyPressedFast(KEY_LEFT)) {
        keys |= fb::input_t::CHANGE_SLOPE;
      }
      if (KeyPressedFast(KEY_RIGHT)) {
        keys |= fb::input_t::CHANGE_PATTERN;
      }
      if (KeyPressedFast(KEY_DOWN)) {
        keys |= fb::input_t::ITER_DEC;
      }
      if (KeyPressedFast(KEY_UP)) {
        keys |= fb::input_t::ITER_INC;
      }
    } else {
      if (KeyPressedFast(KEY_LEFT)) {
        keys |= fb::input_t::SCROLL_LEFT;
      }
      if (KeyPressedFast(KEY_RIGHT)) {
        keys |= fb::input_t::SCROLL_RIGHT;
      }
      if (KeyPressedFast(KEY_DOWN)) {
        keys |= fb::input_t::SCROLL_DOWN;
      }
      if (KeyPressedFast(KEY_UP)) {
        keys |= fb::input_t::SCROLL_UP;
      }
    }

    if (KeyPressedFast(KEY_A)) {
      keys |= fb::input_t::ZOOM_IN;
    }
    if (KeyPressedFast(KEY_B)) {
      keys |= fb::input_t::ZOOM_OUT;
    }

    if (KeyPressedFast(KEY_X)) {
      ctrl_pressed = true;
    } else if (keys == fb::input_t::NONE) {
      ctrl_pressed = false;
    }

    if (KeyPressedFast(KEY_Y)) {
      ResetToBootLoader();
    }

    if (app.service(delta_us, keys) != fb::result_t::SUCCESS) {
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
    engines[1].service();
  }
}

void paint() {
  if (!app.is_repaint_requested()) {
    return;
  }
  app.paint_start();
  for (fb::pos_t y = 0; y < HEIGHT; y++) {
    app.paint_line(y, line_buff);
    if (y > 0) DispStopImg();
    DispStartImg(0, WIDTH, y, y + 1);
    for (fb::pos_t x = 0; x < WIDTH; x++) {
      DispSendImg2(line_buff[x]);
    }
  }
  DispStopImg();
  app.paint_finished();
}

void fixbrot::on_render_start(fb::scene_t &scene) {
  t_start = Time64();
  LedOn(LED1);
  queue.clear();
  for (int i = 0; i < NUM_ENGINES; i++) {
    engines[i].init(scene);
  }
}

void fixbrot::on_render_finished(fb::result_t res) {
  LedOff(LED1);
  t_elapsed = Time64() - t_start;
}

static int engine_index = 0;
static fb::result_t fetch() {
  fb::vec_t loc;
  bool stall;
  do {
    stall = true;
    fb::Engine &e = engines[engine_index];
    if (!e.full() && queue.dequeue(&loc)) {
      FIXBROT_TRY(e.dispatch(loc));
      stall = false;
    }
    engine_index = (engine_index + 1) % NUM_ENGINES;
  } while (!stall);
  return fb::result_t::SUCCESS;
}

fb::result_t fixbrot::on_iterate() {
  FIXBROT_TRY(fetch());
  return engines[0].service();
}

fb::result_t fixbrot::on_dispatch(const fb::vec_t &loc) {
  FIXBROT_TRY(queue.enqueue(loc));
  return fetch();
}

bool fixbrot::on_collect(fb::cell_t *resp) {
  if (engines[0].load() > engines[1].load()) {
    if (engines[0].collect(resp)) {
      return true;
    } else {
      return engines[1].collect(resp);
    }
  } else {
    if (engines[1].collect(resp)) {
      return true;
    } else {
      return engines[0].collect(resp);
    }
  }
}
