#include "../include.h"

#define SHAPOFONT_INCLUDE_GFXFONT
#include "fixbrot/ShapoSansP_s12c09a01w02.h"
#include "fixbrot/fixbrot.hpp"

namespace fb = fixbrot;

enum class menu_mode_t {
  HIDDEN,
  VIEW,
  OPERATE,
  LAST,
};

enum class menu_key_t {
  FORMULA,
  SCENE_REAL,
  SCENE_IMAG,
  SCENE_ZOOM,
  SCENE_ITER,
  PALETTE_TYPE,
  PALETTE_SLOPE,
  PALETTE_PHASE,
  LAST,
  CAPTION,
  BLANK,
};

struct menu_item_t {
  const menu_key_t key;
  const int label_width;
  const int value_lines;
  const char *label_text;
  char value_text[32];
};

static constexpr uint16_t NUM_WORKERS = 2;
static constexpr int MENU_WIDTH = WIDTH / 2;

static constexpr int PADDING = 5;

static const uint16_t MENU_PALETTE[4] = {
    COLOR(16, 16, 16),
    COLOR(0, 160, 255 - 32),
    COLOR(160, 160, 160),
    COLOR(255 - 32, 255 - 32, 255 - 32),
};
static constexpr uint8_t MENU_BACK = 0;
static constexpr uint8_t MENU_ACTIVE = 1;
static constexpr uint8_t MENU_LABEL = 2;
static constexpr uint8_t MENU_VALUE = 3;

menu_item_t menu_items[] = {
    {menu_key_t::CAPTION, 0, 1, "FORMULA", ""},
    {menu_key_t::FORMULA, PADDING, 1, "", ""},
    {menu_key_t::BLANK, 0, 1, "", ""},
    {menu_key_t::CAPTION, 0, 1, "SCENE", ""},
    {menu_key_t::SCENE_REAL, 40, 2, "Real", ""},
    {menu_key_t::SCENE_IMAG, 40, 2, "Imag", ""},
    {menu_key_t::SCENE_ZOOM, 40, 1, "Zoom", ""},
    {menu_key_t::SCENE_ITER, 40, 1, "Iter", ""},
    {menu_key_t::BLANK, 0, 1, "", ""},
    {menu_key_t::CAPTION, 0, 1, "PALETTE", ""},
    {menu_key_t::PALETTE_TYPE, 50, 1, "Type", ""},
    {menu_key_t::PALETTE_SLOPE, 50, 1, "Slope", ""},
    {menu_key_t::PALETTE_PHASE, 50, 1, "Phase", ""},
};
const int NUM_MENU_ITEMS = sizeof(menu_items) / sizeof(menu_items[0]);

static fb::Renderer<WIDTH, HEIGHT> renderer;
static fb::Worker workers[NUM_WORKERS];

static fb::Gray2Bitmap menu_bmp(MENU_WIDTH, HEIGHT);

static fb::builtin_palette_t palette = fb::builtin_palette_t::HEATMAP;
static int palette_slope = fb::DEFAULT_PALETTE_SLOPE;
static bool palette_phase_shift_forward = true;

static int feed_index = 0;
static uint16_t line_buff[WIDTH];

static uint16_t last_pressed = 0;
static bool ctrl_pressed = false;
static bool ctrl_holded = false;

static bool paint_requested = false;
static bool menu_open = false;
static menu_key_t menu_cursor = (menu_key_t)0;
static int menu_pos = 0;

static uint64_t t_start = 0;
static uint64_t t_elapsed = 0;

static volatile bool busy = false;

static void core1_main();
static void paint();
static void update_menu();
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

    busy = !!key_pressed || renderer.is_busy();

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

    if (menu_open) {
      if (key_down & (1 << KEY_UP)) {
        menu_cursor =
            menu_key_t((int(menu_cursor) - 1 + int(menu_key_t::LAST)) %
                       int(menu_key_t::LAST));
        paint_requested = true;
      } else if (key_down & (1 << KEY_DOWN)) {
        menu_cursor =
            menu_key_t((int(menu_cursor) + 1) % int(menu_key_t::LAST));
        paint_requested = true;
      }

      int inc_dec = 0;
      if (key_down & (1 << KEY_LEFT)) {
        inc_dec = -1;
      } else if (key_down & (1 << KEY_RIGHT)) {
        inc_dec = 1;
      }

      if (inc_dec != 0) {
        switch (menu_cursor) {
          case menu_key_t::FORMULA: {
            fb::formula_t f = renderer.get_formula();
            f = (fb::formula_t)(((int)f + (int)fb::formula_t::LAST + inc_dec) %
                                (int)fb::formula_t::LAST);
            renderer.set_formula(f);
            break;
          }

          case menu_key_t::SCENE_REAL:
            renderer.scroll(inc_dec * 64, 0);
            break;

          case menu_key_t::SCENE_IMAG:
            renderer.scroll(0, inc_dec * 64);
            break;

          case menu_key_t::SCENE_ZOOM:
            if (inc_dec > 0) {
              renderer.zoom_in(now_ms);
            } else {
              renderer.zoom_out(now_ms);
            }
            break;

          case menu_key_t::SCENE_ITER:
            renderer.set_max_iter(renderer.get_max_iter() + inc_dec * 100);
            break;

          case menu_key_t::PALETTE_TYPE:
            if (inc_dec > 0) {
              palette = fb::next_palette_of(palette);
            } else {
              palette = fb::prev_palette_of(palette);
            }
            renderer.load_builtin_palette(palette, palette_slope);
            break;

          case menu_key_t::PALETTE_SLOPE:
            palette_slope =
                (palette_slope + fb::MAX_PALETTE_SLOPE + 1 + inc_dec) %
                (fb::MAX_PALETTE_SLOPE + 1);
            renderer.load_builtin_palette(palette, palette_slope);
            break;

          case menu_key_t::PALETTE_PHASE:
            renderer.set_palette_phase(renderer.get_palette_phase() + inc_dec);
            break;
        }
      }
    } else {
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
    }

    if (key_up & (1 << KEY_Y)) {
      ResetToBootLoader();
    }

    feed();
    workers[0].service();
    renderer.service(now_ms);

    paint();

    if (!busy) {
      WaitMs(20);
    }
    // char buf[64];
    // snprintf(buf, sizeof(buf), "Time: %llu us", t_elapsed);
    // DispDrawText(buf, 0, 0, 0, 0, 0xFFFF, 0x0000);
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
  char buf[32];

  if (!renderer.is_repaint_requested() && !paint_requested) {
    return;
  }
  paint_requested = false;

  if (menu_open) {
    update_menu();
  }

  {
    int canvas_sx = 0;
    int canvas_w = WIDTH;
    if (menu_open) {
      if (menu_pos < MENU_WIDTH) {
        menu_pos += (MENU_WIDTH - menu_pos) * 3 / 4 + 1;
        paint_requested = true;
      }
    } else {
      if (menu_pos > 0) {
        menu_pos /= 4;
        paint_requested = true;
      }
    }
    canvas_w = WIDTH - menu_pos;
    canvas_sx = menu_pos / 2;

    renderer.paint_start();
    DispStartImg(menu_pos, WIDTH, 0, HEIGHT);
    for (fb::pos_t y = 0; y < HEIGHT; y++) {
      renderer.paint_line(canvas_sx, y, canvas_w, line_buff);

      if (menu_pos > 0) {
        constexpr int SHADOW_SIZE = 16;
        for (int i = 0; i < SHADOW_SIZE; i++) {
          int coeff = 256 - (SHADOW_SIZE - i) * (SHADOW_SIZE - i) * 256 /
                                (SHADOW_SIZE * SHADOW_SIZE);
          uint8_t r, g, b;
          fb::unpack565(line_buff[i], &r, &g, &b);
          r = r * coeff / 256;
          g = g * coeff / 256;
          b = b * coeff / 256;
          line_buff[i] = fb::pack565(r, g, b);
        }
      }

      for (fb::pos_t x = 0; x < canvas_w; x++) {
        DispSendImg2(line_buff[x]);
      }
    }
    DispStopImg();
    renderer.paint_finished();

    if (menu_pos > 0) {
      DispStartImg(0, menu_pos, 0, HEIGHT);
      for (fb::pos_t y = 0; y < HEIGHT; y++) {
        menu_bmp.render_to(MENU_WIDTH - menu_pos, y, menu_pos, 1, line_buff,
                           WIDTH, MENU_PALETTE);
        for (fb::pos_t x = 0; x < menu_pos; x++) {
          DispSendImg2(line_buff[x]);
        }
      }
      DispStopImg();
    }
  }
}

static void update_menu() {
  const GFXfont &font = ShapoSansP_s12c09a01w02;

  menu_bmp.clear(MENU_BACK);

  int scale_exp = renderer.get_scale_exp();
  int frac_digits = fb::clamp(1, 20, scale_exp * 77 / 256 + 4);

  int line_height = font.yAdvance;
  int baseline = font.yAdvance * 4 / 5;

  int y = PADDING;

  for (int i_menu = 0; i_menu < NUM_MENU_ITEMS; i_menu++) {
    menu_item_t &item = menu_items[i_menu];

    // update menu item values
    switch (item.key) {
      case menu_key_t::FORMULA:
        switch (renderer.get_formula()) {
          case fb::formula_t::BURNING_SHIP:
            strncpy(item.value_text, "Burning Ship", sizeof(item.value_text));
            break;
          case fb::formula_t::CELTIC:
            strncpy(item.value_text, "Celtic", sizeof(item.value_text));
            break;
          case fb::formula_t::BUFFALO:
            strncpy(item.value_text, "Buffalo", sizeof(item.value_text));
            break;
          case fb::formula_t::PERP_BURNING_SHIP:
            strncpy(item.value_text, "Perp. Burning Ship",
                    sizeof(item.value_text));
            break;
          case fb::formula_t::AIRSHIP:
            strncpy(item.value_text, "Airship", sizeof(item.value_text));
            break;
          case fb::formula_t::SHARK_FIN:
            strncpy(item.value_text, "Shark Fin", sizeof(item.value_text));
            break;
          case fb::formula_t::POWER_DRILL:
            strncpy(item.value_text, "Power Drill", sizeof(item.value_text));
            break;
          case fb::formula_t::FEATHER:
            strncpy(item.value_text, "Feather", sizeof(item.value_text));
            break;
          default:  // fb::formula_t::MANDELBROT:
            strncpy(item.value_text, "Mandelbrot", sizeof(item.value_text));
            break;
        }
        break;

      case menu_key_t::SCENE_REAL:
        renderer.get_center_re().to_decimal_string(
            item.value_text, sizeof(item.value_text), frac_digits);
        break;

      case menu_key_t::SCENE_IMAG:
        renderer.get_center_im().to_decimal_string(
            item.value_text, sizeof(item.value_text), frac_digits);
        break;

      case menu_key_t::SCENE_ZOOM: {
        uint64_t zoom = 1ULL << (scale_exp - fb::MIN_SCALE_EXP);
        snprintf(item.value_text, sizeof(item.value_text), "%llux", zoom);
      } break;

      case menu_key_t::SCENE_ITER:
        snprintf(item.value_text, sizeof(item.value_text), "%d",
                 renderer.get_max_iter());
        break;

      case menu_key_t::PALETTE_TYPE:
        snprintf(item.value_text, sizeof(item.value_text), "%d", (int)palette);
        break;

      case menu_key_t::PALETTE_SLOPE:
        snprintf(item.value_text, sizeof(item.value_text), "%d", palette_slope);
        break;

      case menu_key_t::PALETTE_PHASE: {
        int size = renderer.get_palette_size();
        int phase = renderer.get_palette_phase();
        int percent = 100 * (phase & (size - 1)) / size;
        snprintf(item.value_text, sizeof(item.value_text), "%d %%", percent);
      } break;
    }

    // render menu item
    if (item.key == menu_key_t::CAPTION) {
      menu_bmp.fill_rect(0, y, MENU_WIDTH, line_height * item.value_lines - 1,
                         MENU_LABEL);
      menu_bmp.draw_text(PADDING, y + baseline, MENU_WIDTH, item.label_text,
                         font, MENU_BACK);
    } else if (item.key == menu_key_t::BLANK) {
      // do nothing
    } else {
      uint8_t label_color = MENU_LABEL;
      uint8_t value_color = MENU_VALUE;
      if (menu_cursor == item.key) {
        menu_bmp.fill_rect(0, y, MENU_WIDTH, line_height * item.value_lines - 1,
                           MENU_ACTIVE);
        label_color = MENU_BACK;
        value_color = MENU_BACK;
      }

      int val_x = item.label_width;
      int val_w = MENU_WIDTH - val_x - PADDING;
      menu_bmp.draw_text(PADDING, y + baseline, MENU_WIDTH, item.label_text,
                         font, label_color);
      menu_bmp.draw_text(val_x, y + baseline, val_w, item.value_text, font,
                         value_color);
    }
    y += line_height * item.value_lines;
  }
}

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
