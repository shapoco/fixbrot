#include "../include.h"

#define SHAPOFONT_INCLUDE_GFXFONT
#include "fixbrot/ShapoSansP_s12c09a01w02.h"
#include "fixbrot/fixbrot.hpp"

namespace fb = fixbrot;

static constexpr uint16_t NUM_WORKERS = 2;
static constexpr int MENU_WIDTH = WIDTH / 2;

static constexpr int PADDING = 5;
// static constexpr int CHAR_WIDTH = 8;
// static constexpr int LINE_HEIGHT = 16;
// static constexpr uint16_t FORE_COLOR = COLOR(240, 240, 240);
// static constexpr uint16_t BACK_COLOR = COLOR(16, 16, 16);

static const uint16_t MENU_PALETTE[4] = {
    COLOR(16, 16, 16),
    COLOR(64, 64, 64),
    COLOR(128, 128, 128),
    COLOR(240, 240, 240),
};
static constexpr uint8_t MENU_BLACK = 0;
static constexpr uint8_t MENU_GRAY = 1;
static constexpr uint8_t MENU_WHITE = 3;

static fb::Renderer<WIDTH, HEIGHT> renderer;
static fb::Worker workers[NUM_WORKERS];

static fb::Gray2Bitmap menu_bmp(MENU_WIDTH, HEIGHT);

static fb::builtin_palette_t palette = fb::builtin_palette_t::HEATMAP;
static int palette_slope = 0;
static bool palette_phase_shift_forward = true;

static int feed_index = 0;
static uint16_t line_buff[WIDTH];

static uint16_t last_pressed = 0;
static bool ctrl_pressed = false;
static bool ctrl_holded = false;

static bool paint_requested = false;
static bool menu_open = false;

static uint64_t t_start = 0;
static uint64_t t_elapsed = 0;

static void core1_main();
static void paint();
// static void fill_rect(int x, int y, int w, int h, uint16_t color);
// static void draw_coord(int x, int y, fb::real_t value);
static fb::result_t feed();

int main() {
  renderer.init(Time64() / 1000);

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
      if ((key_pressed & EXCEPT_X) != 0) {
        ctrl_holded = true;
      }
    } else {
      if ((key_up & (1 << KEY_X)) && !ctrl_holded) {
        menu_open = !menu_open;
        paint_requested = true;
      }
      if ((key_pressed & EXCEPT_X) == 0) {
        ctrl_pressed = false;
      }
      ctrl_holded = false;
    }

    if (ctrl_pressed) {
      if (key_down & (1 << KEY_LEFT)) {
        // change palette
        if (palette_slope < fb::MAX_PALETTE_SLOPE) {
          palette_slope++;
        } else {
          palette_slope = 0;
          palette = fb::next_palette_of(palette);
        }
        renderer.load_builtin_palette(palette, palette_slope);
      } else if (key_pressed & (1 << KEY_RIGHT)) {
        // change palette phase
        if (key_down & (1 << KEY_RIGHT)) {
          palette_phase_shift_forward = !palette_phase_shift_forward;
        }
        if (palette_phase_shift_forward) {
          renderer.set_palette_phase(renderer.get_palette_phase() + 1);
        } else {
          renderer.set_palette_phase(renderer.get_palette_phase() - 1);
        }
      }
    }

    if (!renderer.is_busy()) {
      if (ctrl_pressed) {
        // change iteration count
        if (key_down & (1 << KEY_DOWN)) {
          renderer.set_max_iter(renderer.get_max_iter() - 100);
        } else if (key_down & (1 << KEY_UP)) {
          renderer.set_max_iter(renderer.get_max_iter() + 100);
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
          renderer.scroll(scroll_delta.x, scroll_delta.y);
        }
      }

      // zoom
      if (key_down & (1 << KEY_A)) {
        renderer.zoom_in(now_ms);
      } else if (key_down & (1 << KEY_B)) {
        renderer.zoom_out(now_ms);
      }
    }

    if (key_up & (1 << KEY_Y)) {
      ResetToBootLoader();
    }

    feed();
    workers[0].service();
    renderer.service(now_ms);

    paint();

    // char buf[64];
    // snprintf(buf, sizeof(buf), "Time: %llu us", t_elapsed);
    // DispDrawText(buf, 0, 0, 0, 0, 0xFFFF, 0x0000);
  }
}

static void core1_main() {
  while (true) {
    workers[1].service();
  }
}

static void paint() {
  char buf[32];

  if (!renderer.is_repaint_requested() && !paint_requested) {
    return;
  }
  paint_requested = false;

  if (menu_open) {
    const GFXfont &font = ShapoSansP_s12c09a01w02;

    // fill_rect(0, 0, MENU_WIDTH, HEIGHT, BACK_COLOR);
    menu_bmp.fill_rect(0, 0, MENU_WIDTH, HEIGHT, MENU_BLACK);

    int scale_exp = renderer.get_scale_exp();
    int frac_digits = fb::clamp(1, 20, scale_exp * 77 / 256 + 4);

    int lh = font.yAdvance;
    int bl = font.yAdvance * 4 / 5;

    int y = PADDING;

    {
      const int val_x = 40;
      const int val_w = MENU_WIDTH - val_x - PADDING;
      menu_bmp.fill_rect(0, y, MENU_WIDTH, lh, MENU_GRAY);
      menu_bmp.draw_text(PADDING, y + bl, MENU_WIDTH, "SCENE", font,
                         MENU_WHITE);
      y += lh;

      //  DispDrawText("real:", PADDING, y, 0, 0, FORE_COLOR, BACK_COLOR);
      menu_bmp.draw_text(PADDING, y + bl, MENU_WIDTH, "Real", font, MENU_WHITE);
      renderer.get_center_re().to_decimal_string(buf, sizeof(buf), frac_digits);
      menu_bmp.draw_text(val_x, y + bl, val_w, buf, font, MENU_WHITE);
      y += lh * 2;

      menu_bmp.draw_text(PADDING, y + bl, MENU_WIDTH, "Imag", font, MENU_WHITE);
      renderer.get_center_im().to_decimal_string(buf, sizeof(buf), frac_digits);
      menu_bmp.draw_text(val_x, y + bl, val_w, buf, font, MENU_WHITE);
      y += lh * 2;

      menu_bmp.draw_text(PADDING, y + bl, MENU_WIDTH, "Zoom", font, MENU_WHITE);
      uint64_t zoom = 1ULL << (scale_exp - fb::MIN_SCALE_EXP);
      snprintf(buf, sizeof(buf), "%llux", zoom);
      menu_bmp.draw_text(val_x, y + bl, val_w, buf, font, MENU_WHITE);
      y += lh;

      menu_bmp.draw_text(PADDING, y + bl, MENU_WIDTH, "Iter", font, MENU_WHITE);
      snprintf(buf, sizeof(buf), "%d", renderer.get_max_iter());
      menu_bmp.draw_text(val_x, y + bl, val_w, buf, font, MENU_WHITE);
      y += lh;
    }
    
    y += lh;

    {
      const int val_x = 50;
      const int val_w = MENU_WIDTH - val_x - PADDING;
      menu_bmp.fill_rect(0, y, MENU_WIDTH, lh, MENU_GRAY);
      menu_bmp.draw_text(PADDING, y + bl, MENU_WIDTH, "PALETTE", font,
                         MENU_WHITE);
      y += lh;

      menu_bmp.draw_text(PADDING, y + bl, MENU_WIDTH, "Mode", font, MENU_WHITE);
      snprintf(buf, sizeof(buf), "%d", (int)palette);
      menu_bmp.draw_text(val_x, y + bl, val_w, buf, font, MENU_WHITE);
      y += lh;

      menu_bmp.draw_text(PADDING, y + bl, MENU_WIDTH, "Slope", font,
                         MENU_WHITE);
      snprintf(buf, sizeof(buf), "%d", palette_slope);
      menu_bmp.draw_text(val_x, y + bl, val_w, buf, font, MENU_WHITE);
      y += lh;

      menu_bmp.draw_text(PADDING, y + bl, MENU_WIDTH, "Phase", font,
                         MENU_WHITE);
      int pal_size = renderer.get_palette_size();
      int pal_phase = renderer.get_palette_phase();
      snprintf(buf, sizeof(buf), "%d %%",
               100 * (pal_phase & (pal_size - 1)) / pal_size);
      menu_bmp.draw_text(val_x, y + bl, val_w, buf, font, MENU_WHITE);
      y += lh;
    }

    //  draw_coord(PADDING * 2, y, renderer.get_center_re());
    //  y += LINE_HEIGHT * 2;
    //  DispDrawText("imag:", PADDING, y, 0, 0, FORE_COLOR, BACK_COLOR);
    //  y += LINE_HEIGHT;
    //  draw_coord(PADDING * 2, y, renderer.get_center_im());
    //  y += LINE_HEIGHT * 2;
  }

  {
    int canvas_sx = 0;
    int canvas_dx = 0;
    int canvas_w = WIDTH;
    if (menu_open) {
      canvas_w = WIDTH - MENU_WIDTH;
      canvas_dx = WIDTH - canvas_w;
      canvas_sx = canvas_dx / 2;
    }
    renderer.paint_start();
    DispStartImg(0, WIDTH, 0, HEIGHT);
    for (fb::pos_t y = 0; y < HEIGHT; y++) {
      if (menu_open && canvas_dx > 0) {
        menu_bmp.render_to(0, y, canvas_dx, 1, line_buff, WIDTH, MENU_PALETTE);
      }
      renderer.paint_line(canvas_sx, y, canvas_w, line_buff + canvas_dx);
      for (fb::pos_t x = 0; x < WIDTH; x++) {
        DispSendImg2(line_buff[x]);
      }
    }
    DispStopImg();
    renderer.paint_finished();
  }
}

// static void fill_rect(int x, int y, int w, int h, uint16_t color) {
//   DispStartImg(x, x + w, y, y + h);
//   for (int iy = 0; iy < h; iy++) {
//     for (int ix = 0; ix < w; ix++) {
//       DispSendImg2(color);
//     }
//   }
//   DispStopImg();
// }
//
// static void draw_coord(int x, int y, fb::real_t value) {
//   constexpr int LINE_LEN = 16;
//   char line0[32];
//   char line1[32] = {0};
//   value.to_decimal_string(line0, sizeof(line0), 20);
//   int n0 = strnlen(line0, sizeof(line0));
//   int n1 = 0;
//   if (n0 > LINE_LEN) {
//     n1 = n0 - LINE_LEN;
//     n0 = LINE_LEN;
//     memcpy(line1, &line0[LINE_LEN], n1);
//     line1[n1] = '\0';
//     line0[LINE_LEN] = '\0';
//   }
//   DispDrawText(line0, x, y, 0, 0, FORE_COLOR, BACK_COLOR);
//   if (n1 > 0) {
//     DispDrawText(line1, x, y + LINE_HEIGHT, 0, 0, FORE_COLOR, BACK_COLOR);
//   }
// }

static fb::result_t feed() {
  bool stall = false;
  int n = renderer.num_queued();
  while (n-- > 0 && !stall) {
    stall = true;
    for (int i = 0; i < NUM_WORKERS; i++) {
      fb::Worker &w = workers[feed_index];
      feed_index = (feed_index + 1) % NUM_WORKERS;
      fb::vec_t loc;
      if (!w.full() && renderer.dequeue(&loc)) {
        FIXBROT_TRY(w.dispatch(loc));
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
  for (int i = 0; i < NUM_WORKERS; i++) {
    workers[i].init(scene);
  }
}

void fixbrot::on_render_finished(fb::result_t res) {
  LedOff(LED1);
  t_elapsed = Time64() - t_start;
}

bool fixbrot::on_collect(fb::cell_t *resp) {
  if (workers[0].num_processed() > workers[1].num_processed()) {
    return workers[0].collect(resp);
  } else {
    return workers[1].collect(resp);
  }
}
