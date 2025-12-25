#ifndef FIXBROT_GBUI_HPP
#define FIXBROT_GBUI_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#endif

#include "fixbrot/common.hpp"
#include "fixbrot/packed_bitmap.hpp"
#include "fixbrot/renderer.hpp"
#include "fixbrot/worker.hpp"

#define SHAPOFONT_INCLUDE_GFXFONT
#include "fixbrot/ShapoSansP_s12c09a01w02.h"

namespace fixbrot {

enum class button_t {
  NONE = 0,
  UP = (1 << 0),
  DOWN = (1 << 1),
  LEFT = (1 << 2),
  RIGHT = (1 << 3),
  A = (1 << 4),
  B = (1 << 5),
  X = (1 << 6),
  Y = (1 << 7),
};
static FIXBROT_INLINE button_t operator|(button_t a, button_t b) {
  return button_t(static_cast<int>(a) | static_cast<int>(b));
}
static FIXBROT_INLINE button_t operator&(button_t a, button_t b) {
  return button_t(static_cast<int>(a) & static_cast<int>(b));
}
static FIXBROT_INLINE button_t operator~(button_t a) {
  return button_t(~static_cast<int>(a));
}
static FIXBROT_INLINE bool operator!(button_t a) {
  return static_cast<int>(a) == 0;
}
static FIXBROT_INLINE button_t &operator|=(button_t &a, button_t b) {
  a = a | b;
  return a;
}
static FIXBROT_INLINE button_t &operator&=(button_t &a, button_t b) {
  a = a & b;
  return a;
}

enum class menu_key_t {
  FORMULA,
  SCENE_REAL,
  SCENE_IMAG,
  SCENE_ZOOM,
  SCENE_ITER,
  SCENE_VFLIP,
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

static const uint16_t MENU_PALETTE[4] = {
    0x1082,
    0x061F,
    0xC618,
    0xFFFF,
};
static constexpr uint8_t MENU_BACK = 0;
static constexpr uint8_t MENU_ACTIVE = 1;
static constexpr uint8_t MENU_LABEL = 2;
static constexpr uint8_t MENU_VALUE = 3;

static constexpr int PADDING = 5;

menu_item_t menu_items[] = {
    {menu_key_t::CAPTION, 0, 1, "FORMULA", ""},
    {menu_key_t::FORMULA, PADDING, 1, "", ""},
    {menu_key_t::BLANK, 0, 1, "", ""},
    {menu_key_t::CAPTION, 0, 1, "SCENE", ""},
    {menu_key_t::SCENE_REAL, 40, 2, "Real", ""},
    {menu_key_t::SCENE_IMAG, 40, 2, "Imag", ""},
    {menu_key_t::SCENE_ZOOM, 40, 1, "Zoom", ""},
    {menu_key_t::SCENE_ITER, 40, 1, "Iter", ""},
    {menu_key_t::SCENE_VFLIP, 40, 1, "VFlip", ""},
    {menu_key_t::BLANK, 0, 1, "", ""},
    {menu_key_t::CAPTION, 0, 1, "PALETTE", ""},
    {menu_key_t::PALETTE_TYPE, 50, 1, "Type", ""},
    {menu_key_t::PALETTE_SLOPE, 50, 1, "Slope", ""},
    {menu_key_t::PALETTE_PHASE, 50, 1, "Phase", ""},
};
const int NUM_MENU_ITEMS = sizeof(menu_items) / sizeof(menu_items[0]);

static constexpr int MENU_WIDTH = 160;

class GBUI {
 public:
  const int width;
  const int height;

  Renderer renderer;

  Gray2Bitmap menu_bmp;

  builtin_palette_t palette = builtin_palette_t::HEATMAP;
  int palette_slope = DEFAULT_PALETTE_SLOPE;
  bool palette_phase_shift_forward = true;

  button_t last_pressed = button_t::NONE;
  bool ctrl_pressed = false;
  bool ctrl_holded = false;

  uint64_t scroll_start_x_ms = 0;
  uint64_t scroll_start_y_ms = 0;
  int scroll_dir_x = 0;
  int scroll_dir_y = 0;

  bool paint_requested = false;
  bool menu_open = false;
  menu_key_t menu_cursor = (menu_key_t)0;
  int menu_pos = 0;

  static constexpr int SHADOW_SIZE = 16;
  int shadow_alpha[SHADOW_SIZE];

  GBUI(pos_t width, pos_t height)
      : width(width),
        height(height),
        renderer(width, height),
        menu_bmp(MENU_WIDTH, height) {}

  result_t init(uint64_t now_ms) {
    FIXBROT_TRY(renderer.init(now_ms));
    for (int i = 0; i < SHADOW_SIZE; i++) {
      shadow_alpha[i] = 256 - (SHADOW_SIZE - i) * (SHADOW_SIZE - i) * 256 /
                                  (SHADOW_SIZE * SHADOW_SIZE);
    }
    return result_t::SUCCESS;
  }

  bool is_busy() const { return renderer.is_busy() || paint_requested; }

  bool is_paint_requested() const {
    return renderer.is_repaint_requested() || paint_requested;
  }

  result_t service(uint64_t now_ms, button_t key_pressed) {
    button_t key_down = key_pressed & (~last_pressed);
    button_t key_up = (~key_pressed) & last_pressed;
    last_pressed = key_pressed;

    button_t EXCEPT_X = button_t::LEFT | button_t::RIGHT | button_t::UP |
                        button_t::DOWN | button_t::A | button_t::B |
                        button_t::Y;
    if (!!(key_pressed & button_t::X)) {
      ctrl_pressed = true;
      if (!!(key_pressed & EXCEPT_X)) {
        ctrl_holded = true;
      }
    } else {
      if (!!(key_up & button_t::X) && !ctrl_holded) {
        menu_open = !menu_open;
        paint_requested = true;
      }
      if (!(key_pressed & EXCEPT_X)) {
        ctrl_pressed = false;
      }
      ctrl_holded = false;
    }

    if (menu_open) {
      if (!!(key_down & button_t::UP)) {
        menu_cursor =
            menu_key_t((int(menu_cursor) - 1 + int(menu_key_t::LAST)) %
                       int(menu_key_t::LAST));
        paint_requested = true;
      } else if (!!(key_down & button_t::DOWN)) {
        menu_cursor =
            menu_key_t((int(menu_cursor) + 1) % int(menu_key_t::LAST));
        paint_requested = true;
      }

      int inc_dec = 0;
      if (!!(key_down & button_t::LEFT)) {
        inc_dec = -1;
      } else if (!!(key_down & button_t::RIGHT)) {
        inc_dec = 1;
      }

      if (inc_dec != 0) {
        switch (menu_cursor) {
          case menu_key_t::FORMULA: {
            formula_t f = renderer.get_formula();
            f = (formula_t)(((int)f + (int)formula_t::LAST + inc_dec) %
                            (int)formula_t::LAST);
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

          case menu_key_t::SCENE_VFLIP:
            renderer.set_vert_flip(!renderer.get_vert_flip());
            break;

          case menu_key_t::PALETTE_TYPE:
            if (inc_dec > 0) {
              palette = next_palette_of(palette);
            } else {
              palette = prev_palette_of(palette);
            }
            renderer.load_builtin_palette(palette, palette_slope);
            break;

          case menu_key_t::PALETTE_SLOPE:
            palette_slope = (palette_slope + MAX_PALETTE_SLOPE + 1 + inc_dec) %
                            (MAX_PALETTE_SLOPE + 1);
            renderer.load_builtin_palette(palette, palette_slope);
            break;

          case menu_key_t::PALETTE_PHASE:
            renderer.set_palette_phase(renderer.get_palette_phase() + inc_dec);
            break;
        }
      }
    } else {
      if (ctrl_pressed) {
        if (!!(key_down & button_t::LEFT)) {
          // change palette
          if (palette_slope < MAX_PALETTE_SLOPE) {
            palette_slope++;
          } else {
            palette_slope = 0;
            palette = next_palette_of(palette);
          }
          renderer.load_builtin_palette(palette, palette_slope);
        } else if (!!(key_pressed & button_t::RIGHT)) {
          // change palette phase
          if (!!(key_down & button_t::RIGHT)) {
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
          if (!!(key_down & button_t::DOWN)) {
            renderer.set_max_iter(renderer.get_max_iter() - 100);
          } else if (!!(key_down & button_t::UP)) {
            renderer.set_max_iter(renderer.get_max_iter() + 100);
          }
        } else {
          // scroll
          int new_dir_x = 0;
          int scroll_x = 0;
          if (!!(key_pressed & button_t::LEFT)) {
            new_dir_x = -1;
          } else if (!!(key_pressed & button_t::RIGHT)) {
            new_dir_x = 1;
          }
          if (new_dir_x != scroll_dir_x) {
            scroll_dir_x = new_dir_x;
            scroll_start_x_ms = now_ms;
          }
          if (scroll_dir_x != 0) {
            int elapsed = now_ms - scroll_start_x_ms;
            scroll_x = scroll_dir_x * clamp(1, 32, 1 + elapsed / 128);
          }

          int new_dir_y = 0;
          int scroll_y = 0;
          if (!!(key_pressed & button_t::UP)) {
            new_dir_y = -1;
          } else if (!!(key_pressed & button_t::DOWN)) {
            new_dir_y = 1;
          }
          if (new_dir_y != scroll_dir_y) {
            scroll_dir_y = new_dir_y;
            scroll_start_y_ms = now_ms;
          }
          if (scroll_dir_y != 0) {
            int elapsed = now_ms - scroll_start_y_ms;
            scroll_y = scroll_dir_y * clamp(1, 32, 1 + elapsed / 128);
          }

          if (scroll_x != 0 || scroll_y != 0) {
            renderer.scroll(scroll_x, scroll_y);
          }
        }

        // zoom
        if (!!(key_down & button_t::A)) {
          renderer.zoom_in(now_ms);
        } else if (!!(key_down & button_t::B)) {
          renderer.zoom_out(now_ms);
        }
      }
    }

    FIXBROT_TRY(renderer.service(now_ms));
    return result_t::SUCCESS;
  }

  result_t paint_start() {
    paint_requested = false;

    if (menu_open) {
      update_menu();
    }

    // menu animation
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

    renderer.paint_start();

    return result_t::SUCCESS;
  }

  result_t paint_line(pos_t y, uint16_t *line_buff) {
    renderer.paint_line(menu_pos / 2, y, width - menu_pos,
                        line_buff + menu_pos);

    if (menu_pos > 0) {
      menu_bmp.render_to(MENU_WIDTH - menu_pos, y, menu_pos, 1, line_buff,
                         width, MENU_PALETTE);

      // menu shadow effect
      for (int i = 0; i < SHADOW_SIZE; i++) {
        uint8_t r, g, b;
        unpack565(line_buff[menu_pos + i], &r, &g, &b);
        r = (int)(r * shadow_alpha[i]) >> 8;
        g = (int)(g * shadow_alpha[i]) >> 8;
        b = (int)(b * shadow_alpha[i]) >> 8;
        line_buff[menu_pos + i] = pack565(r, g, b);
      }
    }
    return result_t::SUCCESS;
  }

  result_t paint_end() {
    renderer.paint_finished();
    return result_t::SUCCESS;
  }

 private:
  void update_menu() {
    const GFXfont &font = ShapoSansP_s12c09a01w02;

    menu_bmp.clear(MENU_BACK);

    int scale_exp = renderer.get_scale_exp();
    int frac_digits = clamp(1, 20, scale_exp * 77 / 256 + 4);

    int line_height = font.yAdvance;
    int baseline = font.yAdvance * 4 / 5;

    int y = PADDING;

    for (int i_menu = 0; i_menu < NUM_MENU_ITEMS; i_menu++) {
      menu_item_t &item = menu_items[i_menu];

      // update menu item values
      switch (item.key) {
        case menu_key_t::FORMULA:
          strncpy(item.value_text, Mandelbrot::get_name(renderer.get_formula()),
                  sizeof(item.value_text));
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
          uint64_t zoom = 1ULL << (scale_exp - MIN_SCALE_EXP);
          snprintf(item.value_text, sizeof(item.value_text), "%llux", zoom);
        } break;

        case menu_key_t::SCENE_ITER:
          snprintf(item.value_text, sizeof(item.value_text), "%d",
                   renderer.get_max_iter());
          break;

        case menu_key_t::SCENE_VFLIP:
          snprintf(item.value_text, sizeof(item.value_text), "%s",
                   renderer.get_vert_flip() ? "On" : "Off");
          break;

        case menu_key_t::PALETTE_TYPE:
          snprintf(item.value_text, sizeof(item.value_text), "%d",
                   (int)palette);
          break;

        case menu_key_t::PALETTE_SLOPE:
          snprintf(item.value_text, sizeof(item.value_text), "%d",
                   palette_slope);
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
          menu_bmp.fill_rect(0, y, MENU_WIDTH,
                             line_height * item.value_lines - 1, MENU_ACTIVE);
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
};

}  // namespace fixbrot

#endif
