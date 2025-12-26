#include <M5Unified.h>
#include "Fixbrot.h"

static constexpr uint16_t NUM_WORKERS = 2;

#define FIXBROT_USE_CANVAS (0)

#if FIXBROT_USE_CANVAS
M5Canvas *canvas;
#endif

int screen_w, screen_h;

namespace fb = fixbrot;
fb::GUI *gui;
fb::Worker workers[NUM_WORKERS];

static int feed_index = 0;
static uint16_t *line_buff;
static uint64_t last_busy_time_ms = 0;
static volatile bool busy = false;

static void worker1(void *arg);
static void paint();
static fb::result_t feed();

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);

  screen_w = M5.Display.width();
  screen_h = M5.Display.height();

#if FIXBROT_USE_CANVAS
  canvas = new M5Canvas(&M5.Display);
  canvas->createSprite(screen_w, screen_h);
#endif

  line_buff = new uint16_t[screen_w];

  gui = new fb::GUI(screen_w, screen_h);
  gui->init();

  xTaskCreatePinnedToCore(worker1, "Worker1", 8192, NULL, 3, NULL, PRO_CPU_NUM);
}

void loop() {
  uint64_t now_ms = millis();
  fb::button_t key_pressed = fb::button_t::NONE;

  lgfx::touch_point_t tp[fb::NUM_MAX_TOUCHES];
  fb::raw_touch_t touches[fb::NUM_MAX_TOUCHES];
  int num_touches = M5.Display.getTouch(tp, fb::NUM_MAX_TOUCHES);
  for (int it = 0; it < num_touches; it++) {
    touches[it].id = tp[it].id;
    touches[it].x = tp[it].x;
    touches[it].y = tp[it].y;
  }
  gui->touch_update_raw(num_touches, touches);
  gui->service();
  feed();

  workers[0].service();
  paint();
  if (gui->is_busy() || num_touches > 0) {
    last_busy_time_ms = now_ms;
    busy = true;
  } else {
    busy = now_ms - last_busy_time_ms < 1000;
  }

  if (!busy) {
    vTaskDelay(20);
  }
}

static void worker1(void *arg) {
  uint64_t next_wdt_reset_ms = 0;
  while (true) {
    workers[1].service();
    uint64_t now_ms = millis();
    if (!busy) {
      next_wdt_reset_ms = now_ms + 1000;
      vTaskDelay(20);
    } else if (now_ms > next_wdt_reset_ms) {
      next_wdt_reset_ms = now_ms + 1000;
      vTaskDelay(1);
    }
  }
}

static void paint() {
  if (!gui->is_paint_requested()) {
    return;
  }

  gui->paint_start();
#if FIXBROT_USE_CANVAS
  uint16_t *wptr = (uint16_t *)(canvas->getBuffer());
#endif
  for (fb::pos_t y = 0; y < screen_h; y++) {
    gui->paint_line(y, line_buff);
#if FIXBROT_USE_CANVAS
    for (fb::pos_t x = 0; x < screen_w; x++) {
      uint16_t tmp = line_buff[x];
      tmp = ((tmp << 8) & 0xFF00) | ((tmp >> 8) & 0xFF);
      line_buff[x] = tmp;
    }
    memcpy(wptr, line_buff, sizeof(uint16_t) * screen_w);
    wptr += screen_w;
#else
    M5.Display.drawBitmap(0, y, screen_w, 1, line_buff);
#endif
  }
  gui->paint_end();
#if FIXBROT_USE_CANVAS
  canvas->pushSprite(0, 0);
#endif
}

static fb::result_t feed() {
  bool stall = false;
  int n = gui->renderer.num_queued();
  while (n-- > 0 && !stall) {
    stall = true;
    for (int i = 0; i < NUM_WORKERS; i++) {
      fb::Worker &w = workers[feed_index];
      feed_index = (feed_index + 1) % NUM_WORKERS;
      fb::vec_t loc;
      if (!w.full() && gui->renderer.dequeue(&loc)) {
        FIXBROT_TRY(w.dispatch(loc));
        stall = false;
        break;
      }
    }
  }
  return fb::result_t::SUCCESS;
}

uint64_t fb::get_time_ms() {
  return millis();
}

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
