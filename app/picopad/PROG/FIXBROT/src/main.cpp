#include "../include.h"

#include "fixbrot/fixbrot.hpp"
#include "fixbrot/gui.hpp"

namespace fb = fixbrot;

static constexpr uint16_t NUM_WORKERS = 2;

static int feed_index = 0;
static uint16_t line_buff[WIDTH];

static uint64_t last_busy_time_ms = 0;
static bool busy = false;

static void core1_main();
static void paint();
static fb::result_t feed();

fb::Worker workers[NUM_WORKERS];
fb::GUI gbui(WIDTH, HEIGHT);

int main() {
#if USE_PICOPAD10 || USE_PICOPAD20
  vreg_set_voltage(VREG_VOLTAGE_1_20);
  WaitMs(100);
  set_sys_clock_khz(250000, true);
#endif

  gbui.init(Time64() / 1000);

  Core1Exec(core1_main);

  while (True) {
    uint64_t now_ms = Time64() / 1000;

    fb::button_t key_pressed = fb::button_t::NONE;
    if (KeyPressedFast(KEY_RIGHT)) key_pressed |= fb::button_t::RIGHT;
    if (KeyPressedFast(KEY_LEFT)) key_pressed |= fb::button_t::LEFT;
    if (KeyPressedFast(KEY_UP)) key_pressed |= fb::button_t::UP;
    if (KeyPressedFast(KEY_DOWN)) key_pressed |= fb::button_t::DOWN;
    if (KeyPressedFast(KEY_A)) key_pressed |= fb::button_t::A;
    if (KeyPressedFast(KEY_B)) key_pressed |= fb::button_t::B;
    if (KeyPressedFast(KEY_X)) key_pressed |= fb::button_t::X;
    if (KeyPressedFast(KEY_Y)) key_pressed |= fb::button_t::Y;

    if ((key_pressed & fb::button_t::Y) != fb::button_t::NONE) {
      ResetToBootLoader();
    }

    gbui.service(now_ms, key_pressed);
    feed();
    workers[0].service();

    paint();

    if (gbui.is_busy() || key_pressed != fb::button_t::NONE) {
      last_busy_time_ms = now_ms;
      busy = true;
    } else {
      busy = now_ms - last_busy_time_ms < 1000;
    }

    if (!busy) {
      WaitMs(20);
    }
  }
}

static void core1_main() {
  while (true) {
    if (!busy) {
      WaitMs(20);
    }
    workers[1].service();
  }
}

static void paint() {
  if (!gbui.is_paint_requested()) {
    return;
  }

  gbui.paint_start();
  DispStartImg(0, WIDTH, 0, HEIGHT);
  for (fb::pos_t y = 0; y < HEIGHT; y++) {
    gbui.paint_line(y, line_buff);
    for (fb::pos_t x = 0; x < WIDTH; x++) {
      DispSendImg2(line_buff[x]);
    }
  }
  DispStopImg();
  gbui.paint_end();
}

static fb::result_t feed() {
  bool stall = false;
  int n = gbui.renderer.num_queued();
  while (n-- > 0 && !stall) {
    stall = true;
    for (int i = 0; i < NUM_WORKERS; i++) {
      fb::Worker &w = workers[feed_index];
      feed_index = (feed_index + 1) % NUM_WORKERS;
      fb::vec_t loc;
      if (!w.full() && gbui.renderer.dequeue(&loc)) {
        FIXBROT_TRY(w.dispatch(loc));
        stall = false;
        break;
      }
    }
  }
  return fb::result_t::SUCCESS;
}

void fixbrot::on_render_start(const fb::scene_t &scene) {
  LedOn(LED1);
  for (int i = 0; i < NUM_WORKERS; i++) {
    workers[i].init(scene);
  }
}

void fixbrot::on_render_finished(fb::result_t res) { LedOff(LED1); }

bool fixbrot::on_collect(fb::cell_t *resp) {
  if (workers[0].num_processed() > workers[1].num_processed()) {
    return workers[0].collect(resp);
  } else {
    return workers[1].collect(resp);
  }
}
