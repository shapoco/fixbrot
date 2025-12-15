#ifndef FIXBROT_HPP
#define FIXBROT_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#include <stdlib.h>
#endif

#include "fixbrot/common.hpp"
#include "fixbrot/engine.hpp"
#include "fixbrot/palette.hpp"

namespace fixbrot {

void on_render_start(scene_t &scene);
void on_render_finished(result_t res);
result_t on_iterate();
result_t on_dispatch(const vec_t &req);
bool on_collect(cell_t *resp);
result_t on_send_line(pos_t x, pos_t y, pos_t width, const col_t *data);

template <uint16_t prm_WIDTH, uint16_t prm_HEIGHT>
class App {
 public:
  static constexpr pos_t SCREEN_W = prm_WIDTH;
  static constexpr pos_t SCREEN_H = prm_HEIGHT;
  static constexpr int16_t ZOOM_ANIMATION_DURATION_MS = 300;
  col_t line_buff[SCREEN_W];
  iter_t work_buff[SCREEN_W * SCREEN_H];

  uint16_t busy_items = 0;

  real_t center_re = -0.5f;
  real_t center_im = 0;
  int16_t scale_exp = -2;
  int16_t screen_size_clog2 = 0;
  real_t pixel_step = 0;
  iter_t max_iter = 200;
  uint32_t iter_accum = 0;

  int16_t zoom_animation_ms = 0;
  float scroll_accum_x = 0;
  float scroll_accum_y = 0;

  Palette palette;
  pos_t x_buff[SCREEN_W];

  input_t last_keys = input_t::NONE;

  bool repaint_requested = false;

  App() {}

  result_t init() {
    center_re = -0.5f;
    center_im = 0;
    scale_exp = -2;
    screen_size_clog2 = 0;
    pos_t p = SCREEN_W > SCREEN_H ? SCREEN_W : SCREEN_H;
    while (p > 0) {
      screen_size_clog2++;
      p /= 2;
    }
    update_pixel_step();

    FIXBROT_TRY(clear_rect(rect_t{0, 0, SCREEN_W, SCREEN_H}));
    FIXBROT_TRY(start_render());
    return result_t::SUCCESS;
  }

  result_t service(uint32_t delta_us, input_t keys) {
    input_t down_keys = keys & (~last_keys);
    if (busy_items == 0) {
      if (!!(keys & input_t::SCROLL_MASK)) {
        FIXBROT_TRY(scroll(delta_us, keys));
      } else if (!!(down_keys & input_t::ZOOM_IN)) {
        FIXBROT_TRY(zoom_in());
      } else if (!!(down_keys & input_t::ZOOM_OUT)) {
        FIXBROT_TRY(zoom_out());
      } else if (!!(down_keys & input_t::ITER_INC)) {
        FIXBROT_TRY(set_max_iter(max_iter + 100));
      } else if (!!(down_keys & input_t::ITER_DEC)) {
        FIXBROT_TRY(set_max_iter(max_iter - 100));
      } else if (!!(down_keys & input_t::PALETTE_CHANGE)) {
        FIXBROT_TRY(palette.next());
        repaint_requested = true;
      }
    } else {
      FIXBROT_TRY(iterate());
    }

    if (repaint_requested) {
      repaint_requested = false;
      repaint(delta_us);
    }

    last_keys = keys;
    return result_t::SUCCESS;
  }

 private:
  result_t repaint(uint32_t delta_us) {
    // zoom animation
    int16_t scaling = 256;
    if (zoom_animation_ms > 0) {
      zoom_animation_ms -= delta_us / 1000;
      if (zoom_animation_ms < 0) {
        zoom_animation_ms = 0;
      }
      scaling =
          256 + (int32_t)zoom_animation_ms * 256 / ZOOM_ANIMATION_DURATION_MS;
      repaint_requested = true;
    } else if (zoom_animation_ms < 0) {
      zoom_animation_ms += delta_us / 1000;
      if (zoom_animation_ms > 0) {
        zoom_animation_ms = 0;
      }
      scaling =
          256 + (int32_t)zoom_animation_ms * 128 / ZOOM_ANIMATION_DURATION_MS;
      repaint_requested = true;
    }

    for (pos_t x = 0; x < SCREEN_W; x++) {
      pos_t sx = x;
      if (scaling != 256) {
        sx = (int32_t)(x - SCREEN_W / 2) * scaling / 256 + (SCREEN_W / 2);
        if (scaling > 256) {
          sx = (sx / 2) * 2;
        }
      }
      x_buff[x] = sx;
    }

    for (pos_t y = 0; y < SCREEN_H; y++) {
      pos_t sy = y;
      if (scaling != 256) {
        sy = (int32_t)(y - SCREEN_H / 2) * scaling / 256 + (SCREEN_H / 2);
        if (scaling > 256) {
          sy = (sy / 2) * 2;
        }
      }

      for (pos_t x = 0; x < SCREEN_W; x++) {
        pos_t sx = x_buff[x];

        if (sx < 0 || sx >= SCREEN_W || sy < 0 || sy >= SCREEN_H) {
          line_buff[x] = 0x0000;
          continue;
        }

        iter_t iter = work_buff[sy * SCREEN_W + sx];
        if (iter == ITER_BLANK) {
          line_buff[x] = 0x0000;
        } else if (iter == ITER_QUEUED) {
          line_buff[x] = 0xFFE0;
        } else {
          line_buff[x] = palette.get_color(iter);
        }
      }
      FIXBROT_TRY(on_send_line(0, y, SCREEN_W, line_buff));
    }
    return result_t::SUCCESS;
  }

  result_t scroll(uint32_t delta_us, input_t keys) {
    if (delta_us > 500 * 1000) {
      delta_us = 500 * 1000;
    }

    if (!!(keys & input_t::SCROLL_LEFT) && center_re > -2) {
      scroll_accum_x -= (float)delta_us * 0.0001f;
    }
    if (!!(keys & input_t::SCROLL_RIGHT) && center_re < 2) {
      scroll_accum_x += (float)delta_us * 0.0001f;
    }
    if (!!(keys & input_t::SCROLL_UP) && center_im > -2) {
      scroll_accum_y -= (float)delta_us * 0.0001f;
    }
    if (!!(keys & input_t::SCROLL_DOWN) && center_im < 2) {
      scroll_accum_y += (float)delta_us * 0.0001f;
    }

    vec_t delta{0, 0};
    if (scroll_accum_x <= -1.0f) {
      delta.x = (pos_t)scroll_accum_x;
      scroll_accum_x -= (float)delta.x;
    } else if (scroll_accum_x >= 1.0f) {
      delta.x = (pos_t)scroll_accum_x;
      scroll_accum_x -= (float)delta.x;
    }
    if (scroll_accum_y <= -1.0f) {
      delta.y = (pos_t)scroll_accum_y;
      scroll_accum_y -= (float)delta.y;
    } else if (scroll_accum_y >= 1.0f) {
      delta.y = (pos_t)scroll_accum_y;
      scroll_accum_y -= (float)delta.y;
    }
    if (delta.x == 0 && delta.y == 0) {
      return result_t::SUCCESS;
    }

    center_re += pixel_step * delta.x;
    center_im += pixel_step * delta.y;

    rect_t dest;
    dest.x = (delta.x >= 0) ? 0 : -delta.x;
    dest.y = (delta.y >= 0) ? 0 : -delta.y;
    dest.h = SCREEN_H - ((delta.y >= 0) ? delta.y : -delta.y);
    dest.w = SCREEN_W - ((delta.x >= 0) ? delta.x : -delta.x);

    // scroll image data
    iter_t *src_line = work_buff;
    iter_t *dst_line = work_buff;
    if (delta.x >= 0) {
      src_line += delta.x;
    } else {
      dst_line -= delta.x;
    }
    if (delta.y >= 0) {
      src_line += delta.y * SCREEN_W;
      for (pos_t i = 0; i < dest.h; i++) {
        memmove(dst_line, src_line, sizeof(iter_t) * dest.w);
        src_line += SCREEN_W;
        dst_line += SCREEN_W;
      }
    } else {
      src_line += (dest.h - 1) * SCREEN_W;
      dst_line += (dest.h - 1 - delta.y) * SCREEN_W;
      for (pos_t i = 0; i < dest.h; i++) {
        memmove(dst_line, src_line, sizeof(iter_t) * dest.w);
        src_line -= SCREEN_W;
        dst_line -= SCREEN_W;
      }
    }

    // clear new area
    if (delta.x > 0) {
      FIXBROT_TRY(clear_rect(rect_t{dest.right(), 0, delta.x, SCREEN_H}));
    } else if (delta.x < 0) {
      FIXBROT_TRY(clear_rect(rect_t{0, 0, (pos_t)-delta.x, SCREEN_H}));
    }
    if (delta.y > 0) {
      FIXBROT_TRY(clear_rect(rect_t{dest.x, dest.bottom(), dest.w, delta.y}));
    } else if (delta.y < 0) {
      FIXBROT_TRY(clear_rect(rect_t{dest.x, 0, dest.w, (pos_t)-delta.y}));
    }

    // render new area
    FIXBROT_TRY(start_render());
    if (delta.x != 0) {
      pos_t x0 = (delta.x > 0) ? (dest.right() - 1) : dest.x;
      pos_t x1 = (delta.x > 0) ? dest.right() : (dest.x - 1);
      FIXBROT_TRY(scan_vert(x0, x1, dest.y, dest.h));
    }
    if (delta.y != 0) {
      pos_t y0 = (delta.y > 0) ? (dest.bottom() - 1) : dest.y;
      pos_t y1 = (delta.y > 0) ? dest.bottom() : (dest.y - 1);
      FIXBROT_TRY(scan_hori(dest.x, y0, y1, dest.w));
    }

    return result_t::SUCCESS;
  }

  result_t zoom_in() {
    scale_exp++;
    bool last_is_fixed32 = pixel_step.is_fixed32();
    update_pixel_step();
    bool prec_changed = pixel_step.is_fixed32() != last_is_fixed32;

    if (prec_changed) {
      // clear all
      FIXBROT_TRY(clear_rect(rect_t{0, 0, SCREEN_W, SCREEN_H}));
    } else {
      // upscale last image and reuse pixels
      for (pos_t i = 0; i < SCREEN_H; i++) {
        pos_t dy = (i < SCREEN_H / 2) ? i : (SCREEN_H * 3 / 2 - 1 - i);
        pos_t sy = SCREEN_H / 4 + (dy / 2);
        for (pos_t j = 0; j < SCREEN_W; j++) {
          pos_t dx = (j < SCREEN_W / 2) ? j : (SCREEN_W * 3 / 2 - 1 - j);
          pos_t sx = SCREEN_W / 4 + (dx / 2);
          if ((dx & 1) == 0 && (dy & 1) == 0) {
            work_buff[dy * SCREEN_W + dx] = work_buff[sy * SCREEN_W + sx];
          } else {
            work_buff[dy * SCREEN_W + dx] = ITER_BLANK;
          }
        }
      }
    }
    FIXBROT_TRY(start_render());

    zoom_animation_ms = ZOOM_ANIMATION_DURATION_MS;
    repaint_requested = true;

    return result_t::SUCCESS;
  }

  result_t zoom_out() {
    if (scale_exp <= -3) {
      return result_t::SUCCESS;
    }
    scale_exp--;
    bool last_is_fixed32 = pixel_step.is_fixed32();
    update_pixel_step();
    bool prec_changed = pixel_step.is_fixed32() != last_is_fixed32;

    if (prec_changed) {
      // clear all
      FIXBROT_TRY(clear_rect(rect_t{0, 0, SCREEN_W, SCREEN_H}));
      FIXBROT_TRY(start_render());
    } else {
      // downscale last image and reuse pixels
      for (pos_t i = 0; i < SCREEN_H; i++) {
        pos_t dy = (i < SCREEN_H / 2) ? (SCREEN_H / 2 - 1 - i) : i;
        pos_t sy = dy * 2 - SCREEN_H / 2;
        for (pos_t j = 0; j < SCREEN_W; j++) {
          pos_t dx = (j < SCREEN_W / 2) ? (SCREEN_W / 2 - 1 - j) : j;
          pos_t sx = dx * 2 - SCREEN_W / 2;
          if (0 <= sx && sx < SCREEN_W && 0 <= sy && sy < SCREEN_H) {
            work_buff[dy * SCREEN_W + dx] = work_buff[sy * SCREEN_W + sx];
          } else {
            work_buff[dy * SCREEN_W + dx] = ITER_BLANK;
          }
        }
      }
      FIXBROT_TRY(start_render());
      rect_t rect{SCREEN_W / 4, SCREEN_H / 4, SCREEN_W / 2, SCREEN_H / 2};
      FIXBROT_TRY(scan_vert(rect.x, rect.x - 1, rect.y, rect.h));
      FIXBROT_TRY(scan_vert(rect.right() - 1, rect.right(), rect.y, rect.h));
      FIXBROT_TRY(scan_hori(rect.x, rect.y, rect.y - 1, rect.w));
      FIXBROT_TRY(scan_hori(rect.x, rect.bottom() - 1, rect.bottom(), rect.w));
    }

    zoom_animation_ms = -ZOOM_ANIMATION_DURATION_MS;
    repaint_requested = true;

    return result_t::SUCCESS;
  }

  result_t set_max_iter(iter_t max_iter) {
    if (max_iter < 100) {
      max_iter = 100;
    } else if (max_iter > ITER_MAX) {
      max_iter = ITER_MAX;
    }
    if (this->max_iter == max_iter) {
      return result_t::SUCCESS;
    }
    bool increasing = (max_iter > this->max_iter);
    this->max_iter = max_iter;
    if (increasing) {
      FIXBROT_TRY(start_render());
      for (pos_t y = 1; y < SCREEN_H - 1; y++) {
        for (pos_t x = 1; x < SCREEN_W - 1; x++) {
          if (work_buff[y * SCREEN_W + x] == ITER_MAX) {
            work_buff[y * SCREEN_W + x] = ITER_BLANK;
          }
        }
      }
      for (pos_t y0 = 8; y0 < SCREEN_H - 1; y0 += 16) {
        pos_t y1 = y0 + 1;
        if (y0 < SCREEN_H / 2) {
          FIXBROT_TRY(scan_hori(0, y0, y1, SCREEN_W));
        } else {
          FIXBROT_TRY(scan_hori(0, y1, y0, SCREEN_W));
        }
      }
      for (pos_t x0 = 8; x0 < SCREEN_W - 1; x0 += 16) {
        pos_t x1 = x0 + 1;
        if (x0 < SCREEN_W / 2) {
          FIXBROT_TRY(scan_vert(x0, x1, 0, SCREEN_H));
        } else {
          FIXBROT_TRY(scan_vert(x1, x0, 0, SCREEN_H));
        }
      }
    }
    repaint_requested = true;
    return result_t::SUCCESS;
  }

  void update_pixel_step() {
    pixel_step = real_exp2(-scale_exp - screen_size_clog2);
  }

  result_t scan_vert(pos_t x0, pos_t x1, pos_t y0, pos_t h) {
    cell_t a = get_cell(x0, y0);
    cell_t c = get_cell(x1, y0);
    for (pos_t y = y0; y < y0 + h - 1; y++) {
      cell_t b = get_cell(x0, y + 1);
      cell_t d = get_cell(x1, y + 1);
      FIXBROT_TRY(compare(a, b, c, d));
      a = b;
      c = d;
    }
    return result_t::SUCCESS;
  }

  result_t scan_hori(pos_t x0, pos_t y0, pos_t y1, pos_t w) {
    cell_t a = get_cell(x0, y0);
    cell_t c = get_cell(x0, y1);
    for (pos_t x = x0; x < x0 + w - 1; x++) {
      cell_t b = get_cell(x + 1, y0);
      cell_t d = get_cell(x + 1, y1);
      FIXBROT_TRY(compare(a, b, c, d));
      a = b;
      c = d;
    }
    return result_t::SUCCESS;
  }

  result_t clear_rect(rect_t view) {
    iter_t *line_ptr = work_buff + (view.y * SCREEN_W + view.x);
    for (pos_t i = 0; i < view.h; i++) {
      memset(line_ptr, 0, sizeof(iter_t) * view.w);
      line_ptr += SCREEN_W;
    }
    return result_t::SUCCESS;
  }

  result_t start_render() {
    if (busy_items > 0) {
      return result_t::ERROR_BUSY;
    }

    scene_t scene;
    scene.real = center_re - pixel_step * (SCREEN_W / 2);
    scene.imag = center_im - pixel_step * (SCREEN_H / 2);
    scene.step = pixel_step;
    scene.max_iter = max_iter;
    on_render_start(scene);

    for (pos_t x = 0; x < SCREEN_W; x++) {
      enqueue(vec_t{x, 0});
      enqueue(vec_t{x, (pos_t)(SCREEN_H - 1)});
    }
    for (pos_t y = 1; y < SCREEN_H - 1; y++) {
      enqueue(vec_t{0, y});
      enqueue(vec_t{(pos_t)(SCREEN_W - 1), y});
    }

    iter_accum = 0;

    return result_t::SUCCESS;
  }

  result_t iterate() {
    if (busy_items == 0) {
      return result_t::SUCCESS;
    }

    // border-tracing
    while (true) {
      FIXBROT_TRY(on_iterate());
      cell_t c;
      if (!dequeue(&c)) {
        break;
      }
      if (c.iter == ITER_MAX) {
        iter_accum += max_iter;
      } else {
        iter_accum += c.iter;
      }
      pos_t x = c.loc.x;
      pos_t y = c.loc.y;
      work_buff[y * SCREEN_W + x] = c.iter;
      cell_t l = get_cell(x - 1, y);
      cell_t r = get_cell(x + 1, y);
      cell_t u = get_cell(x, y - 1);
      cell_t d = get_cell(x, y + 1);
      cell_t lu = get_cell(x - 1, y - 1);
      cell_t ru = get_cell(x + 1, y - 1);
      cell_t ld = get_cell(x - 1, y + 1);
      cell_t rd = get_cell(x + 1, y + 1);
      FIXBROT_TRY(compare(c, u, l, lu));
      FIXBROT_TRY(compare(c, d, l, ld));
      FIXBROT_TRY(compare(c, u, r, ru));
      FIXBROT_TRY(compare(c, d, r, rd));
      FIXBROT_TRY(compare(c, l, u, lu));
      FIXBROT_TRY(compare(c, r, u, ru));
      FIXBROT_TRY(compare(c, l, d, ld));
      FIXBROT_TRY(compare(c, r, d, rd));

      uint32_t iter_thresh = SCREEN_W * SCREEN_H * 2;
      if (zoom_animation_ms != 0) {
        iter_thresh /= 4;
      }
      if (pixel_step.is_fixed32()) {
        iter_thresh *= 2;
      }
      if (iter_accum >= iter_thresh) {
        iter_accum = 0;
        repaint_requested = true;
        break;
      }
    }

    if (busy_items == 0) {
      // fill blanks
      iter_t *line_ptr = work_buff;
      for (pos_t y = 0; y < SCREEN_H; y++) {
        iter_t *ptr = line_ptr;
        iter_t last = 1;
        for (pos_t x = 0; x < SCREEN_W; x++) {
          iter_t iter = *ptr;
          if (iter == ITER_BLANK) {
            *ptr = last;
          } else {
            last = iter;
          }
          ptr++;
        }
        line_ptr += SCREEN_W;
      }

      on_render_finished(result_t::SUCCESS);
      repaint_requested = true;
    }

    return result_t::SUCCESS;
  }

  inline cell_t get_cell(pos_t x, pos_t y) {
    vec_t loc{x, y};
    cell_t cell;
    if (0 <= x && x < SCREEN_W && 0 <= y && y < SCREEN_H) {
      cell.loc = loc;
      cell.iter = work_buff[y * SCREEN_W + x];
    } else {
      cell.loc = loc;
      cell.iter = ITER_WALL;
    }
    return cell;
  }

  // 境界線の処理
  // a: 注目ピクセル
  // b: 境界線検出用の隣接ピクセル
  // c, d: 新たに処理対象とするピクセル
  //   |   |   |
  // --+---+---+--
  //   | a | b |
  // --+---+---+--
  //   | c | d |
  // --+---+---+--
  //   |   |   |
  result_t compare(cell_t &a, cell_t &b, cell_t &c, cell_t &d) {
    if (b.is_finished() && b.iter != a.iter) {
      // b ピクセルが処理完了済みかつ値が a と異なる
      // c, d が未処理であればエンキュー
      if (c.is_blank()) {
        c.iter = ITER_QUEUED;
        FIXBROT_TRY(enqueue(c.loc));
      }
      if (d.is_blank()) {
        d.iter = ITER_QUEUED;
        FIXBROT_TRY(enqueue(d.loc));
      }
    }
    return result_t::SUCCESS;
  }

  result_t enqueue(vec_t loc) {
    iter_t &target = work_buff[loc.y * SCREEN_W + loc.x];
    if (target != ITER_BLANK) {
      return result_t::SUCCESS;
    }
    target = ITER_QUEUED;
    FIXBROT_TRY(on_dispatch(loc));
    busy_items++;
    return result_t::SUCCESS;
  }

  bool dequeue(cell_t *cell) {
    if (on_collect(cell)) {
      busy_items--;
      return true;
    } else {
      return false;
    }
  }
};

}  // namespace fixbrot

#endif
