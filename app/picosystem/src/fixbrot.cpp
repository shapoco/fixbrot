#include <pico/multicore.h>
#include <picosystem.hpp>

#include "fixbrot/fixbrot.hpp"

namespace ps = picosystem;
namespace fb = fixbrot;

static constexpr uint16_t NUM_WORKERS = 2;
static constexpr fb::pos_t WIDTH = 240;
static constexpr fb::pos_t HEIGHT = 240;

static int feed_index = 0;

static uint64_t last_busy_time_ms = 0;
static volatile bool busy = false;

static void core1_main();
static fb::result_t feed();

fb::Worker workers[NUM_WORKERS];
fb::GUI gui(WIDTH, HEIGHT);

void init() {
  gui.init();
  multicore_launch_core1(core1_main);
}

void update(uint32_t tick) {
  fb::button_t key_pressed = fb::button_t::NONE;
  if (ps::button(ps::RIGHT)) key_pressed |= fb::button_t::RIGHT;
  if (ps::button(ps::LEFT)) key_pressed |= fb::button_t::LEFT;
  if (ps::button(ps::UP)) key_pressed |= fb::button_t::UP;
  if (ps::button(ps::DOWN)) key_pressed |= fb::button_t::DOWN;
  if (ps::button(ps::B)) key_pressed |= fb::button_t::A;
  if (ps::button(ps::A)) key_pressed |= fb::button_t::B;
  if (ps::button(ps::Y)) key_pressed |= fb::button_t::X;
  if (ps::button(ps::X)) key_pressed |= fb::button_t::Y;

  gui.button_update(key_pressed);

  for (int i = 0; i < 256; i++) {
    gui.service();
    feed();
    if (workers[0].num_queued() == 0) break;
    workers[0].service();
    if (gui.renderer.is_repaint_requested()) break;
  }

  if (gui.is_busy() || key_pressed != fb::button_t::NONE) {
    last_busy_time_ms = tick;
    busy = true;
  } else {
    busy = tick - last_busy_time_ms < 1000;
  }

  if (!busy) {
    sleep_ms(20);
  }
}

void draw(uint32_t tick) {
  if (!gui.is_paint_requested()) {
    return;
  }

  uint16_t *wr_ptr = ps::SCREEN->data;

  gui.paint_start();
  for (fb::pos_t y = 0; y < HEIGHT; y++) {
    gui.paint_line(y, wr_ptr);
    for (fb::pos_t x = 0; x < WIDTH; x++) {
      uint8_t r, g, b;
      fb::unpack565(wr_ptr[x], &r, &g, &b);
#if 0
      if ((x + y) & 1) {
        if (r < 31) r++;
        if (g < 63) g++;
        if (b < 31) b++;
      }
#endif
      wr_ptr[x] = ps::rgb(r >> 1, g >> 2, b >> 1);
    }
    wr_ptr += 240;
  }
  gui.paint_end();
}

static void core1_main() {
  while (true) {
    workers[1].service();
    if (!busy) {
      sleep_ms(20);
    }
  }
}

static fb::result_t feed() {
  bool stall = false;
  int n = gui.renderer.num_queued();
  while (n-- > 0 && !stall) {
    stall = true;
    for (int i = 0; i < NUM_WORKERS; i++) {
      fb::Worker &w = workers[feed_index];
      feed_index = (feed_index + 1) % NUM_WORKERS;
      fb::vec_t loc;
      if (!w.full() && gui.renderer.dequeue(&loc)) {
        FIXBROT_TRY(w.dispatch(loc));
        stall = false;
        break;
      }
    }
  }
  return fb::result_t::SUCCESS;
}

uint64_t fb::get_time_ms() { return ps::time(); }

void fb::on_render_start(const fb::scene_t &scene) {
  for (int i = 0; i < NUM_WORKERS; i++) {
    workers[i].init(scene);
  }
}

void fb::on_render_finished(fb::result_t res) {}

bool fb::on_collect(fb::cell_t *resp) {
  if (workers[0].num_processed() > workers[1].num_processed()) {
    return workers[0].collect(resp);
  } else {
    return workers[1].collect(resp);
  }
}
