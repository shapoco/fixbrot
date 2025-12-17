#include "../include.h"

#include "fixbrot/fixbrot.hpp"

namespace fb = fixbrot;

static constexpr uint16_t NUM_ENGINES = 2;

static fb::App<WIDTH, HEIGHT> app;
static fb::Engine engines[NUM_ENGINES];

static fb::builtin_palette_t palette = fb::builtin_palette_t::HEATMAP;
static int palette_slope = 0;
static bool palette_phase_shift_forward = true;

static int feed_index = 0;
static uint16_t line_buff[WIDTH];

static uint16_t last_pressed = 0;
static bool ctrl_pressed = false;

static uint64_t t_start = 0;
static uint64_t t_elapsed = 0;

static void core1_main();
static void paint();
static fb::result_t feed();

int main() {
  app.init(Time64() / 1000);

  Core1Exec(core1_main);

  fb::vec_t scroll_delta = {0, 0};

  while (True) {
    uint64_t now_ms = Time64() / 1000;

    uint16_t key_pressed = 0;
    if (KeyPressedFast(KEY_RIGHT)) key_pressed |= (1 << KEY_RIGHT);
    if (KeyPressedFast(KEY_LEFT)) key_pressed |= (1 << KEY_LEFT);
    if (KeyPressedFast(KEY_UP)) key_pressed |= (1 << KEY_UP);
    if (KeyPressedFast(KEY_DOWN)) key_pressed |= (1 << KEY_DOWN);
    if (KeyPressedFast(KEY_A)) key_pressed |= (1 << KEY_A);
    if (KeyPressedFast(KEY_B)) key_pressed |= (1 << KEY_B);
    if (KeyPressedFast(KEY_X)) key_pressed |= (1 << KEY_X);
    if (KeyPressedFast(KEY_Y)) key_pressed |= (1 << KEY_Y);
    uint16_t key_down = key_pressed & (~last_pressed);
    uint16_t key_up = (~key_pressed) & last_pressed;
    last_pressed = key_pressed;

    constexpr uint16_t EXCEPT_X = (1 << KEY_LEFT) | (1 << KEY_RIGHT) |
                                  (1 << KEY_UP) | (1 << KEY_DOWN) |
                                  (1 << KEY_A) | (1 << KEY_B) | (1 << KEY_Y);
    if (key_pressed & (1 << KEY_X)) {
      ctrl_pressed = true;
    } else if ((key_pressed & EXCEPT_X) == 0) {
      ctrl_pressed = false;
    }

    if (ctrl_pressed) {
      if (key_down & (1 << KEY_LEFT)) {
        if (palette_slope < fb::MAX_PALETTE_SLOPE) {
          palette_slope++;
        } else {
          palette_slope = 0;
          palette = fb::next_palette_of(palette);
        }
        app.load_builtin_palette(palette, palette_slope);
      } else if (key_pressed & (1 << KEY_RIGHT)) {
        if (key_down & (1 << KEY_RIGHT)) {
          palette_phase_shift_forward = !palette_phase_shift_forward;
        }
        if (palette_phase_shift_forward) {
          app.set_palette_phase(app.get_palette_phase() + 1);
        } else {
          app.set_palette_phase(app.get_palette_phase() - 1);
        }
      }
    }

    if (!app.is_busy()) {
      if (ctrl_pressed) {
        // change iteration count
        if (key_down & (1 << KEY_DOWN)) {
          app.set_max_iter(app.get_max_iter() - 100);
        } else if (key_down & (1 << KEY_UP)) {
          app.set_max_iter(app.get_max_iter() + 100);
        }
      } else {
        // scroll
        if (key_pressed & (1 << KEY_LEFT)) {
          scroll_delta.x = fb::clamp(-32, -1, scroll_delta.x - 1);
        } else if (key_pressed & (1 << KEY_RIGHT)) {
          scroll_delta.x = fb::clamp(1, 32, scroll_delta.x + 1);
        } else {
          scroll_delta.x = 0;
        }
        if (key_pressed & (1 << KEY_UP)) {
          scroll_delta.y = fb::clamp(-32, -1, scroll_delta.y - 1);
        } else if (key_pressed & (1 << KEY_DOWN)) {
          scroll_delta.y = fb::clamp(1, 32, scroll_delta.y + 1);
        } else {
          scroll_delta.y = 0;
        }
        if (scroll_delta.x != 0 || scroll_delta.y != 0) {
          app.scroll(scroll_delta.x, scroll_delta.y);
        }
      }

      // zoom
      if (key_down & (1 << KEY_A)) {
        app.zoom_in(now_ms);
      } else if (key_down & (1 << KEY_B)) {
        app.zoom_out(now_ms);
      }
    }

    if (key_up & (1 << KEY_Y)) {
      ResetToBootLoader();
    }

    feed();
    engines[0].service();
    app.service(now_ms);

    paint();

    // char buf[64];
    // snprintf(buf, sizeof(buf), "Time: %llu us", t_elapsed);
    // DispDrawText(buf, 0, 0, 0, 0, 0xFFFF, 0x0000);
  }
}

static void core1_main() {
  while (true) {
    engines[1].service();
  }
}

static void paint() {
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

static fb::result_t feed() {
  bool stall = false;
  int n = app.num_queued();
  while (n-- > 0 && !stall) {
    stall = true;
    for (int i = 0; i < NUM_ENGINES; i++) {
      fb::Engine &e = engines[feed_index];
      feed_index = (feed_index + 1) % NUM_ENGINES;
      fb::vec_t loc;
      if (!e.full() && app.dequeue(&loc)) {
        FIXBROT_TRY(e.dispatch(loc));
        stall = false;
        break;
      }
    }
  }
  return fb::result_t::SUCCESS;
}

void fixbrot::on_render_start(fb::scene_t &scene) {
  t_start = Time64();
  LedOn(LED1);
  for (int i = 0; i < NUM_ENGINES; i++) {
    engines[i].init(scene);
  }
}

void fixbrot::on_render_finished(fb::result_t res) {
  LedOff(LED1);
  t_elapsed = Time64() - t_start;
}

bool fixbrot::on_collect(fb::cell_t *resp) {
  if (engines[0].num_processed() > engines[1].num_processed()) {
    return engines[0].collect(resp);
  } else {
    return engines[1].collect(resp);
  }
}
