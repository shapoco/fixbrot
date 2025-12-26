#ifndef FIXBROT_RENDERER_HPP
#define FIXBROT_RENDERER_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#endif

#include "fixbrot/array_queue.hpp"
#include "fixbrot/common.hpp"
#include "fixbrot/mandelbrot.hpp"

namespace fixbrot {

void on_render_start(const scene_t &scene);
void on_render_finished(result_t res);
bool on_collect(cell_t *resp);
uint64_t get_time_ms();

class Renderer {
 public:
  const pos_t width;
  const pos_t height;
  static constexpr int ZOOM_DURATION_MS = 200;

 private:
  uint64_t last_ms = 0;

  int busy_items = 0;
  ArrayQueue<vec_t> queue;
  iter_t *work_buff;

  scene_t scene;
  int scale_exp = -2;
  int screen_size_clog2 = 0;
  bool vert_flip = false;
  uint32_t iter_accum = 0;

  pos_t correct_x = 0;
  pos_t correct_y = height;

  col_t palette[MAX_PALETTE_SIZE] = {0};
  col_t max_iter_color = 0x0000;
  int palette_slope = 0;
  int palette_phase = 0;
  int palette_size = MAX_PALETTE_SIZE;

  bool paint_requested = false;
  bool paint_zoom_inprog = false;
  bool paint_zoom_dir_in = false;
  uint64_t paint_zoom_end_ms = 0;
  pos_t *paint_x_buff;
  float paint_scale = 1.0f;

 public:
  Renderer(pos_t width, pos_t height)
      : width(width),
        height(height),
        queue((width + height) * 16),
        work_buff(new iter_t[width * height]),
        paint_x_buff(new pos_t[width]) {}

  ~Renderer() {
    delete[] work_buff;
    delete[] paint_x_buff;
  }

  real_t get_center_re() const { return scene.real; }
  real_t get_center_im() const { return scene.imag; }
  int get_scale_exp() const { return scale_exp; }

  result_t init() {
    scene.formula = formula_t::MANDELBROT;
    scene.real = -0.5f;
    scene.imag = 0;
    scene.max_iter = 200;

    scale_exp = -2;
    screen_size_clog2 = 0;
    pos_t p = width > height ? width : height;
    while (p > 0) {
      screen_size_clog2++;
      p /= 2;
    }
    update_pixel_step();

    palette_load_heatmap(DEFAULT_PALETTE_SLOPE);

    FIXBROT_TRY(clear_rect(rect_t{0, 0, width, height}));
    FIXBROT_TRY(start_render(true));
    paint_requested = true;

    return result_t::SUCCESS;
  }

  result_t service() {
    uint64_t now_ms = get_time_ms();
    last_ms = now_ms;

    if (is_busy()) {
      FIXBROT_TRY(iterate());
    }

    // zoom animation
    paint_scale = 1.0f;
    if (paint_zoom_inprog) {
      int32_t t = (int32_t)(paint_zoom_end_ms - now_ms);
      if (t < 0) {
        t = 0;
        paint_zoom_inprog = false;
      }
      paint_requested = true;
      if (paint_zoom_dir_in) {
        paint_scale = 1.0f + (float)t / ZOOM_DURATION_MS;
      } else {
        paint_scale = 1.0f - (float)t / (ZOOM_DURATION_MS * 2);
      }
    }

    return result_t::SUCCESS;
  }

  result_t scroll(pos_t delta_x, pos_t delta_y) {
    if (is_busy()) return result_t::ERROR_BUSY;

    if (vert_flip) {
      delta_y = -delta_y;
    }

    delta_x = clamp((pos_t)(-width + 2), (pos_t)(width - 2), delta_x);
    delta_y = clamp((pos_t)(-height + 2), (pos_t)(height - 2), delta_y);

    if (scene.real < -2 && delta_x < 0) delta_x = 0;
    if (scene.real > 2 && delta_x > 0) delta_x = 0;
    if (scene.imag < -2 && delta_y < 0) delta_y = 0;
    if (scene.imag > 2 && delta_y > 0) delta_y = 0;

    if (delta_x == 0 && delta_y == 0) return result_t::SUCCESS;

    scene.real += scene.step * delta_x;
    scene.imag += scene.step * delta_y;

    rect_t dest;
    dest.x = (delta_x >= 0) ? 0 : -delta_x;
    dest.y = (delta_y >= 0) ? 0 : -delta_y;
    dest.h = height - ((delta_y >= 0) ? delta_y : -delta_y);
    dest.w = width - ((delta_x >= 0) ? delta_x : -delta_x);

    // scroll image data
    iter_t *src_line = work_buff;
    iter_t *dst_line = work_buff;
    if (delta_x >= 0) {
      src_line += delta_x;
    } else {
      dst_line -= delta_x;
    }
    if (delta_y >= 0) {
      src_line += delta_y * width;
      for (pos_t i = 0; i < dest.h; i++) {
        memmove(dst_line, src_line, sizeof(iter_t) * dest.w);
        src_line += width;
        dst_line += width;
      }
    } else {
      src_line += (dest.h - 1) * width;
      dst_line += (dest.h - 1 - delta_y) * width;
      for (pos_t i = 0; i < dest.h; i++) {
        memmove(dst_line, src_line, sizeof(iter_t) * dest.w);
        src_line -= width;
        dst_line -= width;
      }
    }

    // clear new area
    if (delta_x > 0) {
      FIXBROT_TRY(clear_rect(rect_t{dest.right(), 0, delta_x, height}));
    } else if (delta_x < 0) {
      FIXBROT_TRY(clear_rect(rect_t{0, 0, (pos_t)-delta_x, height}));
    }
    if (delta_y > 0) {
      FIXBROT_TRY(clear_rect(rect_t{dest.x, dest.bottom(), dest.w, delta_y}));
    } else if (delta_y < 0) {
      FIXBROT_TRY(clear_rect(rect_t{dest.x, 0, dest.w, (pos_t)-delta_y}));
    }

    // render new area
    FIXBROT_TRY(start_render(false));
    if (delta_x != 0) {
      pos_t x0 = (delta_x > 0) ? (dest.right() - 1) : dest.x;
      pos_t x1 = (delta_x > 0) ? dest.right() : (dest.x - 1);
      FIXBROT_TRY(scan_vert(x0, x1, dest.y, dest.h));
    }
    if (delta_y != 0) {
      pos_t y0 = (delta_y > 0) ? (dest.bottom() - 1) : dest.y;
      pos_t y1 = (delta_y > 0) ? dest.bottom() : (dest.y - 1);
      FIXBROT_TRY(scan_hori(dest.x, y0, y1, dest.w));
    }

    return result_t::SUCCESS;
  }

  result_t zoom_in() {
    if (is_busy()) return result_t::ERROR_BUSY;

    scale_exp++;
    bool last_is_fixed32 = scene.step.is_fixed32();
    update_pixel_step();
    bool prec_changed = scene.step.is_fixed32() != last_is_fixed32;

    if (prec_changed) {
      // clear all
      FIXBROT_TRY(clear_rect(rect_t{0, 0, width, height}));
    } else {
      // upscale last image and reuse pixels
      for (pos_t i = 0; i < height; i++) {
        pos_t dy = (i < height / 2) ? i : (height * 3 / 2 - 1 - i);
        pos_t sy = height / 4 + (dy / 2);
        for (pos_t j = 0; j < width; j++) {
          pos_t dx = (j < width / 2) ? j : (width * 3 / 2 - 1 - j);
          pos_t sx = width / 4 + (dx / 2);
          if ((dx & 1) == 0 && (dy & 1) == 0) {
            work_buff[dy * width + dx] = work_buff[sy * width + sx];
          } else {
            work_buff[dy * width + dx] = ITER_BLANK;
          }
        }
      }
    }
    FIXBROT_TRY(start_render(true));

    paint_zoom_inprog = true;
    paint_zoom_dir_in = true;
    paint_zoom_end_ms = get_time_ms() + ZOOM_DURATION_MS;
    paint_requested = true;

    return result_t::SUCCESS;
  }

  result_t zoom_out() {
    if (is_busy()) return result_t::ERROR_BUSY;
    if (scale_exp <= MIN_SCALE_EXP) {
      return result_t::SUCCESS;
    }

    scale_exp--;
    bool last_is_fixed32 = scene.step.is_fixed32();
    update_pixel_step();
    bool prec_changed = scene.step.is_fixed32() != last_is_fixed32;

    if (prec_changed) {
      // clear all
      FIXBROT_TRY(clear_rect(rect_t{0, 0, width, height}));
      FIXBROT_TRY(start_render(true));
    } else {
      // downscale last image and reuse pixels
      for (pos_t i = 0; i < height; i++) {
        pos_t dy = (i < height / 2) ? (height / 2 - 1 - i) : i;
        pos_t sy = dy * 2 - height / 2;
        for (pos_t j = 0; j < width; j++) {
          pos_t dx = (j < width / 2) ? (width / 2 - 1 - j) : j;
          pos_t sx = dx * 2 - width / 2;
          if (0 <= sx && sx < width && 0 <= sy && sy < height) {
            work_buff[dy * width + dx] = work_buff[sy * width + sx];
          } else {
            work_buff[dy * width + dx] = ITER_BLANK;
          }
        }
      }
      FIXBROT_TRY(start_render(true));
      rect_t rect{(pos_t)(width / 4), (pos_t)(height / 4), (pos_t)(width / 2),
                  (pos_t)(height / 2)};
      FIXBROT_TRY(scan_vert(rect.x, rect.x - 1, rect.y, rect.h));
      FIXBROT_TRY(scan_vert(rect.right() - 1, rect.right(), rect.y, rect.h));
      FIXBROT_TRY(scan_hori(rect.x, rect.y, rect.y - 1, rect.w));
      FIXBROT_TRY(scan_hori(rect.x, rect.bottom() - 1, rect.bottom(), rect.w));
    }

    paint_zoom_inprog = true;
    paint_zoom_end_ms = get_time_ms() + ZOOM_DURATION_MS;
    paint_zoom_dir_in = false;
    paint_requested = true;

    return result_t::SUCCESS;
  }

  FIXBROT_INLINE formula_t get_formula() const { return scene.formula; }

  result_t set_formula(formula_t f) {
    if (is_busy()) return result_t::ERROR_BUSY;
    scene.formula = f;
    FIXBROT_TRY(clear_rect(rect_t{0, 0, width, height}));
    FIXBROT_TRY(start_render(true));
    paint_requested = true;
    return result_t::SUCCESS;
  }

  FIXBROT_INLINE iter_t get_max_iter() const { return scene.max_iter; }

  result_t set_max_iter(iter_t max_iter) {
    if (is_busy()) return result_t::ERROR_BUSY;

    if (max_iter < 100) {
      max_iter = 100;
    } else if (max_iter > ITER_MAX) {
      max_iter = ITER_MAX;
    }
    if (scene.max_iter == max_iter) {
      return result_t::SUCCESS;
    }
    bool increasing = (max_iter > scene.max_iter);
    scene.max_iter = max_iter;
    if (increasing) {
      FIXBROT_TRY(start_render(true));
      for (pos_t y = 1; y < height - 1; y++) {
        for (pos_t x = 1; x < width - 1; x++) {
          if (work_buff[y * width + x] == ITER_MAX) {
            work_buff[y * width + x] = ITER_BLANK;
          }
        }
      }
      for (pos_t y0 = 8; y0 < height - 1; y0 += 16) {
        pos_t y1 = y0 + 1;
        if (y0 < height / 2) {
          FIXBROT_TRY(scan_hori(0, y0, y1, width));
        } else {
          FIXBROT_TRY(scan_hori(0, y1, y0, width));
        }
      }
      for (pos_t x0 = 8; x0 < width - 1; x0 += 16) {
        pos_t x1 = x0 + 1;
        if (x0 < width / 2) {
          FIXBROT_TRY(scan_vert(x0, x1, 0, height));
        } else {
          FIXBROT_TRY(scan_vert(x1, x0, 0, height));
        }
      }
    }
    paint_requested = true;
    return result_t::SUCCESS;
  }

  FIXBROT_INLINE bool get_vert_flip() const { return vert_flip; }

  result_t set_vert_flip(bool vf) {
    if (is_busy()) return result_t::ERROR_BUSY;
    vert_flip = vf;
    paint_requested = true;
    return result_t::SUCCESS;
  }

  void load_builtin_palette(builtin_palette_t palette, int slope) {
    slope = clamp(0, MAX_PALETTE_SLOPE, slope);
    switch (palette) {
      case builtin_palette_t::RAINBOW:
        palette_load_rainbow(slope);
        break;
      case builtin_palette_t::GRAY:
        palette_load_gray(slope);
        break;
      case builtin_palette_t::STRIPE:
        palette_load_stripe(slope);
        break;
      default:  // builtin_palette_t::HEATMAP
        palette_load_heatmap(slope);
        break;
    }
    paint_requested = true;
  }

  FIXBROT_INLINE int get_palette_size() const { return palette_size; }

  FIXBROT_INLINE int get_palette_phase() const { return palette_phase; }

  void set_palette_phase(int phase) {
    palette_phase = phase % MAX_PALETTE_SIZE;
    paint_requested = true;
  }

  int num_queued() const { return queue.size(); }

  bool dequeue(vec_t *out_loc) { return queue.dequeue(out_loc); }

  FIXBROT_INLINE bool is_busy() const {
    return (busy_items > 0) || (correct_y < height);
  }

  FIXBROT_INLINE bool is_repaint_requested() const { return paint_requested; }

  FIXBROT_INLINE bool is_animating() const {
    return (last_ms < paint_zoom_end_ms);
  }

  result_t paint_start() {
    // cache x coordinates
    for (pos_t x = 0; x < width; x++) {
      pos_t sx = x;
      if (paint_scale != 1.0f) {
        sx = (pos_t)((x - width / 2) * paint_scale + (width / 2));
      }
      paint_x_buff[x] = sx;
    }
    return result_t::SUCCESS;
  }

  result_t paint_line(pos_t x_offset, pos_t y_offset, pos_t w,
                      col_t *line_buff) {
    if (vert_flip) {
      y_offset = height - 1 - y_offset;
    }

    pos_t sy = y_offset;
    if (paint_scale != 1.0f) {
      sy = (pos_t)((y_offset - height / 2) * paint_scale + (height / 2));
    }

    for (pos_t ix = 0; ix < w; ix++) {
      pos_t x = x_offset + ix;
      pos_t sx = paint_x_buff[x];

      if (sx < 0 || sx >= width || sy < 0 || sy >= height) {
        line_buff[ix] = 0x0000;
        continue;
      }

      bool finished = true;
      iter_t iter = work_buff[sy * width + sx];
      if (iter == ITER_BLANK) {
        finished = false;
        iter = work_buff[(sy & 0xFFFE) * width + (sx & 0xFFFE)];
        if (iter == ITER_BLANK || iter == ITER_QUEUED) {
          constexpr pos_t MASK = ~(COARSE_POS_STEP - 1);
          pos_t sx2 = (sx & MASK) + (COARSE_POS_STEP / 2);
          pos_t sy2 = (sy & MASK) + (COARSE_POS_STEP / 2);
          iter = work_buff[sy2 * width + sx2];
          if (iter == ITER_QUEUED) {
            iter = ITER_BLANK;
          }
        }
      }

      if (ITER_BLANK == iter) {
        line_buff[ix] = 0x0000;
      } else if (iter <= ITER_MAX) {
        col_t c = max_iter_color;
        if (iter < scene.max_iter) {
          c = palette[(iter + palette_phase) & (palette_size - 1)];
        }
        if (!finished) {
          c >>= 1;
          c &= 0x7BEF;
        }
        line_buff[ix] = c;
      } else {
        line_buff[ix] = 0xFFE0;
      }
    }
    return result_t::SUCCESS;
  }

  result_t paint_finished() {
    paint_requested = false;
    return result_t::SUCCESS;
  }

 private:
  void update_pixel_step() {
    scene.step = real_exp2(-scale_exp - screen_size_clog2);
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
    iter_t *line_ptr = work_buff + (view.y * width + view.x);
    for (pos_t i = 0; i < view.h; i++) {
      memset(line_ptr, 0, sizeof(iter_t) * view.w);
      line_ptr += width;
    }
    return result_t::SUCCESS;
  }

  scene_t get_worker_args() {
    scene_t s = scene;
    s.real -= scene.step * (width / 2);
    s.imag -= scene.step * (height / 2);
    return s;
  }

  result_t start_render(bool post_correction) {
    if (is_busy()) {
      return result_t::ERROR_BUSY;
    }

    const scene_t s = get_worker_args();
    on_render_start(s);

    queue.clear();
    busy_items = 0;
    correct_x = 0;
    correct_y = post_correction ? 0 : height;
    iter_accum = 0;

    for (pos_t y = COARSE_POS_STEP / 2; y < height; y += COARSE_POS_STEP) {
      for (pos_t x = COARSE_POS_STEP / 2; x < width; x += COARSE_POS_STEP) {
        enqueue(vec_t{x, y});
      }
    }

    for (pos_t x = 0; x < width; x++) {
      enqueue(vec_t{x, 0});
      enqueue(vec_t{x, (pos_t)(height - 1)});
    }
    for (pos_t y = 1; y < height - 1; y++) {
      enqueue(vec_t{0, y});
      enqueue(vec_t{(pos_t)(width - 1), y});
    }

    return result_t::SUCCESS;
  }

  result_t iterate() {
    if (!is_busy()) {
      return result_t::SUCCESS;
    }

    // border-tracing
    for (int i_batch = 0; i_batch < BATCH_SIZE; i_batch++) {
      cell_t c;
      if (!collect(&c)) break;
      if (c.iter == ITER_MAX) {
        iter_accum += scene.max_iter;
      } else {
        iter_accum += c.iter;
      }
      pos_t x = c.loc.x;
      pos_t y = c.loc.y;
      work_buff[y * width + x] = c.iter;
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
    }

    uint32_t iter_thresh = (uint32_t)width * height * 4;
    if (is_animating()) {
      iter_thresh /= 8;
    }
    if (scene.step.is_fixed32()) {
      iter_thresh *= 2;
    }
    if (iter_accum >= iter_thresh) {
      iter_accum = 0;
      paint_requested = true;
    }

    if (busy_items == 0) {
      correct();
    }

    if (!is_busy()) {
      // fill blanks
      iter_t *line_ptr = work_buff;
      for (pos_t y = 0; y < height; y++) {
        iter_t *ptr = line_ptr;
        iter_t last = 1;
        for (pos_t x = 0; x < width; x++) {
          iter_t iter = *ptr;
          if (iter == ITER_BLANK) {
            *ptr = last;
          } else {
            last = iter;
          }
          ptr++;
        }
        line_ptr += width;
      }

      on_render_finished(result_t::SUCCESS);
      paint_requested = true;
    }

    return result_t::SUCCESS;
  }

  void correct() {
    scene_t s = get_worker_args();

    while (correct_y < height) {
      pos_t x0 = -1;
      iter_t iter0 = ITER_BLANK;
      int blank_count = 0;
      int line_ptr = correct_y * width;
      while (correct_x < width) {
        iter_t iter1 = work_buff[line_ptr + correct_x];
        if (iter1 == ITER_BLANK || ITER_MAX < iter1) {
          // count blank pixel
          blank_count++;
          correct_x++;
          continue;
        }

        if (blank_count == 0 || iter0 == iter1) {
          // not broken
          blank_count = 0;
          x0 = correct_x;
          iter0 = iter1;
          correct_x++;
          continue;
        }

        // find boundary between x0 and x1
        pos_t x1 = correct_x;
        while (x0 + 1 < x1) {
          pos_t xm = (x1 + x0) / 2;
          iter_t iter_m = Mandelbrot::compute(s, vec_t{xm, correct_y});
          work_buff[line_ptr + xm] = iter_m;
          if (iter_m == iter0) {
            x0 = xm;
          } else {
            x1 = xm;
            iter1 = iter_m;
          }
        }

        // restart border-tracing
        if (correct_y > 0) {
          enqueue(vec_t{x0, (pos_t)(correct_y - 1)});
          enqueue(vec_t{x1, (pos_t)(correct_y - 1)});
        }
        if (correct_y < height - 1) {
          enqueue(vec_t{x0, (pos_t)(correct_y + 1)});
          enqueue(vec_t{x1, (pos_t)(correct_y + 1)});
        }

        // continue correction from x1
        x0 = x1;
        iter0 = iter1;
        blank_count = 0;
        correct_x = x1;
        return;
      }

      correct_y++;
      correct_x = 0;
    }
  }

  FIXBROT_INLINE cell_t get_cell(pos_t x, pos_t y) {
    vec_t loc{x, y};
    cell_t cell;
    if (0 <= x && x < width && 0 <= y && y < height) {
      cell.loc = loc;
      cell.iter = work_buff[y * width + x];
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
    iter_t &target = work_buff[loc.y * width + loc.x];
    if (target != ITER_BLANK) {
      return result_t::SUCCESS;
    }
    target = ITER_QUEUED;
    FIXBROT_TRY(queue.enqueue(loc));
    busy_items++;
    return result_t::SUCCESS;
  }

  bool collect(cell_t *cell) {
    if (on_collect(cell)) {
      busy_items--;
      return true;
    } else {
      return false;
    }
  }

  void palette_load_heatmap(int slope) {
    palette_size = MAX_PALETTE_SIZE >> slope;
    max_iter_color = 0x0000;
    for (uint16_t i = 0; i < palette_size; i++) {
      int p = i * (32 * 6) / palette_size;
      int c = p / 32;
      int f = p % 32;
      switch (c) {
        case 0:
          palette[i] = pack565(0, f / 2, f);
          break;
        case 1:
          palette[i] = pack565(0, 16 + f, 31);
          break;
        case 2:
          palette[i] = pack565(f, 48 + f / 2, 31);
          break;
        case 3:
          palette[i] = pack565(31, 63 - f / 2, 31 - f);
          break;
        case 4:
          palette[i] = pack565(31, 47 - f, 0);
          break;
        default:
          palette[i] = pack565(31 - f, 15 - f / 2, 0);
          break;
      }
    }
  }

  void palette_load_rainbow(int slope) {
    palette_size = MAX_PALETTE_SIZE >> slope;
    max_iter_color = 0x0000;
    for (uint16_t i = 0; i < palette_size; i++) {
      int p = i * (64 * 6) / palette_size;
      int c = p / 64;
      int f = p % 64;
      switch (c) {
        case 0:
          palette[i] = pack565(31, f, 0);
          break;
        case 1:
          palette[i] = pack565(31 - (f / 2), 63, 0);
          break;
        case 2:
          palette[i] = pack565(0, 63, f / 2);
          break;
        case 3:
          palette[i] = pack565(0, 63 - f, 31);
          break;
        case 4:
          palette[i] = pack565(f / 2, 0, 31);
          break;
        default:
          palette[i] = pack565(31, 0, 31 - f / 2);
          break;
      }
    }
  }

  void palette_load_gray(int slope) {
    palette_size = MAX_PALETTE_SIZE >> slope;
    max_iter_color = 0x0000;
    for (uint16_t i = 0; i < palette_size; i++) {
      uint16_t gray = i * 128 / palette_size;
      if (gray >= 64) {
        gray = 127 - gray;
      }
      palette[i] = pack565(gray >> 1, gray, gray >> 1);
    }
  }

  void palette_load_stripe(int slope) {
    palette_size = 64 >> slope;
    max_iter_color = 0x0000;
    for (uint16_t i = 0; i < palette_size; i++) {
      palette[i] =
          (i < palette_size / 2) ? pack565(28, 56, 28) : pack565(4, 8, 4);
    }
  }
};

}  // namespace fixbrot

#endif
