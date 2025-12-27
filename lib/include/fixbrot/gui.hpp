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

struct raw_touch_t {
  int id;
  pos_t x;
  pos_t y;
};

struct touch_t {
  bool pressed;
  int id;
  vec_t last_pos;
  vec_t down_pos;
};

enum class touch_state_t {
  IDLE,
  TAP_DOWN,
  TAP_RELEASE,
  DRAGGING,
};

enum class touch_target_t {
  NONE,
  CANVAS,
  MENU_TAB,
  MENU_ITEM,
  BUTTON_L,
  BUTTON_R,
  BUTTON_X,
};

static constexpr int NUM_MAX_TOUCHES = 1;

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
  pos_t y_center;
};

static const col_t MENU_PALETTE[4] = {
    color_pack_from_888(0x16, 0x16, 0x16),
    color_pack_from_888(0x00, 0x80, 0xFF),
    color_pack_from_888(0xC0, 0xC0, 0xC0),
    color_pack_from_888(0xFF, 0xFF, 0xFF),
};
static constexpr uint8_t MENU_BACK = 0;
static constexpr uint8_t MENU_ACTIVE = 1;
static constexpr uint8_t MENU_LABEL = 2;
static constexpr uint8_t MENU_VALUE = 3;

static constexpr int MENU_PADDING = 5;

menu_item_t menu_items[] = {
    {menu_key_t::CAPTION, 0, 1, "FORMULA"},
    {menu_key_t::FORMULA, MENU_PADDING, 1, ""},
    {menu_key_t::BLANK, 0, 1, ""},
    {menu_key_t::CAPTION, 0, 1, "SCENE"},
    {menu_key_t::SCENE_REAL, 40, 2, "Real"},
    {menu_key_t::SCENE_IMAG, 40, 2, "Imag"},
    {menu_key_t::SCENE_ZOOM, 40, 1, "Zoom"},
    {menu_key_t::SCENE_ITER, 40, 1, "Iter"},
    {menu_key_t::SCENE_VFLIP, 40, 1, "VFlip"},
    {menu_key_t::BLANK, 0, 1, ""},
    {menu_key_t::CAPTION, 0, 1, "PALETTE"},
    {menu_key_t::PALETTE_TYPE, 50, 1, "Type"},
    {menu_key_t::PALETTE_SLOPE, 50, 1, "Slope"},
    {menu_key_t::PALETTE_PHASE, 50, 1, "Phase"},
};
const int NUM_MENU_ITEMS = sizeof(menu_items) / sizeof(menu_items[0]);

static constexpr int MENU_WIDTH = 160;
static constexpr int KEYPAD_HEIGHT = 64;

class GUI {
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

  touch_state_t touch_state = touch_state_t::IDLE;
  touch_t touches[NUM_MAX_TOUCHES];
  vec_t touch_first_pos = {0, 0};
  uint64_t touch_last_event_time_ms = 0;
  int touch_tap_count = 0;

  touch_target_t touch_target = touch_target_t::NONE;
  int touch_drag_thresh = 20 * 20;
  vec_t touch_drag_last_pos = {0, 0};

  bool paint_requested = false;
  bool menu_open = false;
  menu_key_t menu_cursor = (menu_key_t)0;
  int menu_pos = 0;

  static constexpr int SHADOW_SIZE = 16;
  int shadow_alpha[SHADOW_SIZE];

  GUI(pos_t width, pos_t height)
      : width(width),
        height(height),
        renderer(width, height),
        menu_bmp(MENU_WIDTH, height) {}

  result_t init() {
    FIXBROT_TRY(renderer.init());
    for (int i = 0; i < SHADOW_SIZE; i++) {
      shadow_alpha[i] = 256 - (SHADOW_SIZE - i) * (SHADOW_SIZE - i) * 256 /
                                  (SHADOW_SIZE * SHADOW_SIZE);
    }
    for (int i = 0; i < NUM_MAX_TOUCHES; i++) {
      touches[i].pressed = false;
    }
    touch_drag_thresh = (width * height) / 256;
    return result_t::SUCCESS;
  }

  bool is_busy() const { return renderer.is_busy() || paint_requested; }

  bool is_paint_requested() const {
    return renderer.is_repaint_requested() || paint_requested;
  }

  result_t button_update(button_t pressed) {
    uint64_t now_ms = get_time_ms();

    button_t down = pressed & (~last_pressed);
    button_t up = (~pressed) & last_pressed;
    last_pressed = pressed;

    button_t EXCEPT_X = button_t::LEFT | button_t::RIGHT | button_t::UP |
                        button_t::DOWN | button_t::A | button_t::B |
                        button_t::Y;
    if (!!(pressed & button_t::X)) {
      ctrl_pressed = true;
      if (!!(pressed & EXCEPT_X)) {
        ctrl_holded = true;
      }
    } else {
      if (!!(up & button_t::X) && !ctrl_holded) {
        if (menu_open) {
          close_menu();
        } else {
          open_menu();
        }
        paint_requested = true;
      }
      if (!(pressed & EXCEPT_X)) {
        ctrl_pressed = false;
      }
      ctrl_holded = false;
    }

    if (menu_open) {
      if (!!(down & button_t::UP)) {
        menu_cursor =
            menu_key_t((int(menu_cursor) - 1 + int(menu_key_t::LAST)) %
                       int(menu_key_t::LAST));
        paint_requested = true;
      } else if (!!(down & button_t::DOWN)) {
        menu_cursor =
            menu_key_t((int(menu_cursor) + 1) % int(menu_key_t::LAST));
        paint_requested = true;
      }

      int inc_dec = 0;
      if (!!(down & button_t::LEFT)) {
        inc_dec = -1;
      } else if (!!(down & button_t::RIGHT)) {
        inc_dec = 1;
      }

      menu_inc_dec(inc_dec);
    } else {
      if (ctrl_pressed) {
        if (!!(down & button_t::LEFT)) {
          // change palette
          if (palette_slope < MAX_PALETTE_SLOPE) {
            palette_slope++;
          } else {
            palette_slope = 0;
            palette = next_palette_of(palette);
          }
          renderer.load_builtin_palette(palette, palette_slope);
        } else if (!!(pressed & button_t::RIGHT)) {
          // change palette phase
          if (!!(down & button_t::RIGHT)) {
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
          if (!!(down & button_t::DOWN)) {
            renderer.set_max_iter(renderer.get_max_iter() - 100);
          } else if (!!(down & button_t::UP)) {
            renderer.set_max_iter(renderer.get_max_iter() + 100);
          }
        } else {
          // scroll
          int new_dir_x = 0;
          int scroll_x = 0;
          if (!!(pressed & button_t::LEFT)) {
            new_dir_x = -1;
          } else if (!!(pressed & button_t::RIGHT)) {
            new_dir_x = 1;
          }
          if (new_dir_x != scroll_dir_x) {
            scroll_dir_x = new_dir_x;
            scroll_start_x_ms = now_ms;
          }
          if (scroll_dir_x != 0) {
            int elapsed = now_ms - scroll_start_x_ms;
            scroll_x = scroll_dir_x * clamp(1, 12, 1 + elapsed / 32);
          }

          int new_dir_y = 0;
          int scroll_y = 0;
          if (!!(pressed & button_t::UP)) {
            new_dir_y = -1;
          } else if (!!(pressed & button_t::DOWN)) {
            new_dir_y = 1;
          }
          if (new_dir_y != scroll_dir_y) {
            scroll_dir_y = new_dir_y;
            scroll_start_y_ms = now_ms;
          }
          if (scroll_dir_y != 0) {
            int elapsed = now_ms - scroll_start_y_ms;
            scroll_y = scroll_dir_y * clamp(1, 12, 1 + elapsed / 32);
          }

          if (scroll_x != 0 || scroll_y != 0) {
            renderer.scroll(scroll_x, scroll_y);
          }
        }

        // zoom
        if (!!(down & button_t::A)) {
          renderer.zoom_in();
        } else if (!!(down & button_t::B)) {
          renderer.zoom_out();
        }
      }
    }

    return result_t::SUCCESS;
  }

  result_t touch_update_raw(int num_touches, const raw_touch_t *new_touches) {
    int marked_ids[NUM_MAX_TOUCHES] = {-1};
    int num_marked = 0;

    if (num_touches > NUM_MAX_TOUCHES) {
      num_touches = NUM_MAX_TOUCHES;
    }

    for (int i = 0; i < num_touches; i++) {
      bool found = false;

      int nid = new_touches[i].id;
      pos_t nx = new_touches[i].x;
      pos_t ny = new_touches[i].y;

      touch_t *t = get_touch(nid);
      if (t) {
        touch_move(t->id, nx, ny);
      } else {
        touch_down(nid, nx, ny);
      }
      marked_ids[num_marked++] = nid;
    }

    for (int j = 0; j < NUM_MAX_TOUCHES; j++) {
      touch_t *t = &touches[j];
      bool marked = false;
      for (int k = 0; k < num_marked; k++) {
        if (t->pressed && t->id == marked_ids[k]) {
          marked = true;
          break;
        }
      }
      if (t->pressed && !marked) {
        touch_up(t->id, t->last_pos.x, t->last_pos.y);
      }
    }

    return result_t::SUCCESS;
  }

  result_t touch_down(int id, pos_t x, pos_t y) {
    touch_t *t = get_touch(id);
    if (!t) {
      for (int i = 0; i < NUM_MAX_TOUCHES; i++) {
        if (!touches[i].pressed) {
          t = &touches[i];
          break;
        }
      }
      if (!t) return result_t::ERROR_TOO_MANY_TOUCHES;
    }
    t->pressed = true;
    t->id = id;
    t->down_pos.x = x;
    t->down_pos.y = y;
    t->last_pos.x = x;
    t->last_pos.y = y;

    uint64_t now_ms = get_time_ms();
    uint64_t elapsed_ms = now_ms - touch_last_event_time_ms;
    switch (touch_state) {
      case touch_state_t::IDLE:
        touch_state = touch_state_t::TAP_DOWN;
        touch_first_pos.x = x;
        touch_first_pos.y = y;
        touch_tap_count = 1;

        if (y < height) {
          if (menu_open) {
            if (x < MENU_WIDTH / 2) {
              touch_target = touch_target_t::MENU_ITEM;
            } else if (x < MENU_WIDTH) {
              touch_target = touch_target_t::MENU_TAB;
            } else {
              touch_target = touch_target_t::CANVAS;
            }
          } else {
            if (x < width / 16) {
              touch_target = touch_target_t::MENU_TAB;
            } else {
              touch_target = touch_target_t::CANVAS;
            }
          }
        } else {
          if (x < width / 3) {
            touch_target = touch_target_t::BUTTON_L;
          } else if (x < 2 * width / 3) {
            touch_target = touch_target_t::BUTTON_X;
          } else {
            touch_target = touch_target_t::BUTTON_R;
          }
        }
        break;

      case touch_state_t::TAP_RELEASE: {
        int dx = x - touch_first_pos.x;
        int dy = y - touch_first_pos.y;
        int d = dx * dx + dy * dy;
        if (d < touch_drag_thresh && elapsed_ms < 200) {
          touch_tap_count++;
          touch_state = touch_state_t::TAP_DOWN;
        } else {
          touch_state = touch_state_t::IDLE;
        }
      } break;
    }
    touch_last_event_time_ms = now_ms;

    return result_t::SUCCESS;
  }

  result_t touch_up(int id, pos_t x, pos_t y) {
    touch_t *t = get_touch(id);
    if (!t) return result_t::SUCCESS;
    t->pressed = false;

    uint64_t now_ms = get_time_ms();
    uint64_t elapsed_ms = now_ms - touch_last_event_time_ms;
    switch (touch_state) {
      case touch_state_t::TAP_DOWN: {
        int dx = x - touch_first_pos.x;
        int dy = y - touch_first_pos.y;
        int d = dx * dx + dy * dy;
        if (d < touch_drag_thresh && elapsed_ms < 200) {
          touch_state = touch_state_t::TAP_RELEASE;
        } else {
          touch_state = touch_state_t::IDLE;
        }
      } break;

      case touch_state_t::DRAGGING:
        touch_drag_end(x, y);
        touch_state = touch_state_t::IDLE;
        break;

      default:
        touch_state = touch_state_t::IDLE;
        break;
    }
    touch_last_event_time_ms = now_ms;

    return result_t::SUCCESS;
  }

  result_t touch_move(int id, pos_t x, pos_t y) {
    touch_t *t = get_touch(id);
    if (!t) return result_t::SUCCESS;
    t->last_pos.x = x;
    t->last_pos.y = y;

    switch (touch_state) {
      case touch_state_t::TAP_DOWN: {
        int dx = x - t->down_pos.x;
        int dy = y - t->down_pos.y;
        int d = dx * dx + dy * dy;
        if (d >= touch_drag_thresh) {
          touch_state = touch_state_t::DRAGGING;
          touch_drag_start(t->down_pos.x, t->down_pos.y);
        }
      } break;

      case touch_state_t::DRAGGING:
        touch_drag_move(x, y);
        break;
    }

    return result_t::SUCCESS;
  }

  touch_t *get_touch(int id) {
    for (int i = 0; i < NUM_MAX_TOUCHES; i++) {
      if (touches[i].pressed && touches[i].id == id) {
        return &touches[i];
      }
    }
    return nullptr;
  }

  result_t service() {
    uint64_t now_ms = get_time_ms();
    if (touch_state == touch_state_t::TAP_RELEASE) {
      uint64_t elapsed_ms = now_ms - touch_last_event_time_ms;
      if (elapsed_ms >= 200) {
        // tap confirmed
        for (int i = 0; i < touch_tap_count; i++) {
          touch_tap(touch_first_pos.x, touch_first_pos.y, touch_tap_count);
        }
        touch_tap_count = 0;
        touch_state = touch_state_t::IDLE;
      }
    }

    FIXBROT_TRY(renderer.service());
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
    pos_t canvas_w = width - menu_pos;
    pos_t canvas_sx = menu_pos / 2;
    renderer.paint_line(canvas_sx, y, canvas_w, line_buff + menu_pos);

    if (menu_pos > 0) {
      menu_bmp.render_to(MENU_WIDTH - menu_pos, y, menu_pos, 1, line_buff,
                         width, MENU_PALETTE);

      // menu shadow effect
      for (int i = 0; i < SHADOW_SIZE; i++) {
        uint8_t r, g, b;
        color_unpack(line_buff[menu_pos + i], &r, &g, &b);
        r = (int)(r * shadow_alpha[i]) >> 8;
        g = (int)(g * shadow_alpha[i]) >> 8;
        b = (int)(b * shadow_alpha[i]) >> 8;
        line_buff[menu_pos + i] = color_pack(r, g, b);
      }
    }

#if FIXBROT_BYTE_SWAP
    for (int x = 0; x < width; x++) {
      uint16_t c = line_buff[x];
      line_buff[x] = ((c >> 8) & 0x00FF) | ((c << 8) & 0xFF00);
    }
#endif

    return result_t::SUCCESS;
  }

  result_t paint_end() {
    renderer.paint_finished();
    return result_t::SUCCESS;
  }

 private:
  result_t touch_drag_start(pos_t x, pos_t y) {
    touch_drag_last_pos = {x, y};
    return result_t::SUCCESS;
  }

  result_t touch_drag_move(pos_t x, pos_t y) {
    if (touch_target == touch_target_t::CANVAS) {
      if (!renderer.is_busy()) {
        int dx = x - touch_drag_last_pos.x;
        int dy = y - touch_drag_last_pos.y;
        renderer.scroll(-dx, -dy);
        touch_drag_last_pos = {x, y};
      }
    } else if (touch_target == touch_target_t::MENU_TAB) {
      if (menu_open && x < MENU_WIDTH / 8) {
        close_menu();
        touch_target = touch_target_t::NONE;
      } else if (!menu_open && x >= MENU_WIDTH / 2) {
        open_menu();
        touch_target = touch_target_t::NONE;
      }
    }
    return result_t::SUCCESS;
  }

  result_t touch_drag_end(pos_t x, pos_t y) { return result_t::SUCCESS; }

  result_t touch_tap(pos_t x, pos_t y, int count) {
    if (menu_open) {
      switch (touch_target) {
        case touch_target_t::MENU_ITEM:
        case touch_target_t::MENU_TAB: {
          int i_menu_nearest = -1;
          int min_dy = 99999;
          for (int i_menu = 0; i_menu < NUM_MENU_ITEMS; i_menu++) {
            menu_item_t &item = menu_items[i_menu];
            if ((int)item.key >= (int)menu_key_t::LAST) continue;
            int dy = y - item.y_center;
            if (dy < 0) dy = -dy;
            if (dy < min_dy) {
              min_dy = dy;
              i_menu_nearest = i_menu;
            }
          }
          if (i_menu_nearest >= 0) {
            menu_cursor = menu_items[i_menu_nearest].key;
            paint_requested = true;
          }
        } break;
        case touch_target_t::BUTTON_L:
          menu_inc_dec(-1);
          break;
        case touch_target_t::BUTTON_R:
          menu_inc_dec(1);
          break;
        case touch_target_t::BUTTON_X:
          close_menu();
          break;
      }
    } else {
      switch (touch_target) {
        case touch_target_t::CANVAS:
          if (count == 1) {
            renderer.zoom_in();
          } else if (count == 2) {
            renderer.zoom_out();
          }
          break;
        case touch_target_t::BUTTON_L: {
          int delta = renderer.get_palette_size() / 8;
          renderer.set_palette_phase(renderer.get_palette_phase() - delta);
        } break;
        case touch_target_t::BUTTON_R: {
          int delta = renderer.get_palette_size() / 8;
          renderer.set_palette_phase(renderer.get_palette_phase() + delta);
        } break;
        case touch_target_t::BUTTON_X:
          open_menu();
          break;
      }
    }

    return result_t::SUCCESS;
  }

  void open_menu() {
    menu_open = true;
    paint_requested = true;
  }

  void close_menu() {
    menu_open = false;
    paint_requested = true;
  }

  void menu_inc_dec(int inc_dec) {
    if (inc_dec == 0) return;
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
          renderer.zoom_in();
        } else {
          renderer.zoom_out();
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

      case menu_key_t::PALETTE_PHASE: {
        int delta = inc_dec * renderer.get_palette_size() / 8;
        renderer.set_palette_phase(renderer.get_palette_phase() + delta);
      } break;
    }
  }

  void update_menu() {
    const GFXfont &font = ShapoSansP_s12c09a01w02;

    menu_bmp.clear(MENU_BACK);

    int scale_exp = renderer.get_scale_exp();
    int frac_digits = clamp(1, 20, scale_exp * 77 / 256 + 4);

    int line_height = font.yAdvance;
    int baseline = font.yAdvance * 4 / 5;

    int y = MENU_PADDING;

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
        menu_bmp.draw_text(MENU_PADDING, y + baseline, MENU_WIDTH,
                           item.label_text, font, MENU_BACK);
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
        int val_w = MENU_WIDTH - val_x - MENU_PADDING;
        menu_bmp.draw_text(MENU_PADDING, y + baseline, MENU_WIDTH,
                           item.label_text, font, label_color);
        menu_bmp.draw_text(val_x, y + baseline, val_w, item.value_text, font,
                           value_color);
      }

      int h = line_height * item.value_lines;
      item.y_center = y + h / 2;
      y += h;
    }
  }
};

}  // namespace fixbrot

#endif
