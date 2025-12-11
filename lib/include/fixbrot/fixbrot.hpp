#ifndef FIXBROT_HPP
#define FIXBROT_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#include <stdlib.h>
#endif

#include "fixbrot/fixed32.hpp"
#include "fixbrot/fixed64.hpp"

#define FIXBROT_TRY(expr)                                                      \
  do {                                                                         \
    result_t res = (expr);                                                     \
    if (res != result_t::SUCCESS) {                                            \
      return res;                                                              \
    }                                                                          \
  } while (0)

namespace fixbrot {

enum class result_t : uint16_t {
  SUCCESS,
  ERROR_QUEUE_OVERFLOW,
};

enum class pad_t : uint16_t {
  NONE = 0,
  UP = (1 << 0),
  DOWN = (1 << 1),
  LEFT = (1 << 2),
  RIGHT = (1 << 3),
  ZOOM_IN = (1 << 4),
  ZOOM_OUT = (1 << 5),
  X = (1 << 6),
  Y = (1 << 7),
};

static inline bool operator&(pad_t a, pad_t b) {
  return (static_cast<uint16_t>(a) & static_cast<uint16_t>(b)) != 0;
}
static inline pad_t operator|(pad_t a, pad_t b) {
  return static_cast<pad_t>(static_cast<uint16_t>(a) |
                            static_cast<uint16_t>(b));
}
static inline pad_t &operator|=(pad_t &a, pad_t b) { return (a = a | b); }

using iter_t = uint8_t;
using pos_t = int16_t;
using col_t = uint16_t;

#if 1

using real_t = fixed64_t;

static inline real_t real_exp2(int exp) {
  return fixed64_t::from_raw((int64_t)1 << (fixed64_t::FRAC_BITS + exp));
}

#else

using real_t = float;
static inline real_t real_exp2(int exp) {
  real_t result = 1.0f;
  bool neg = (exp < 0);
  if (neg) {
    exp = -exp;
  }
  for (int i = 0; i < exp; i++) {
    result *= 2;
  }
  return neg ? (1.0f / result) : result;
}

#endif

struct loc_t {
  pos_t x;
  pos_t y;
};

struct rect_t {
  pos_t x;
  pos_t y;
  pos_t w;
  pos_t h;

  inline bool contains(loc_t loc) const {
    return (x <= loc.x) && (loc.x < x + w) && (y <= loc.y) && (loc.y < y + h);
  }
};

static constexpr iter_t ITER_BLANK = 0;
static constexpr iter_t ITER_MAX = (1 << (sizeof(iter_t) * 8)) - 3;
static constexpr iter_t ITER_QUEUED = (1 << (sizeof(iter_t) * 8)) - 2;
static constexpr iter_t ITER_WALL = (1 << (sizeof(iter_t) * 8)) - 1;

struct cell_t {
  loc_t loc;
  iter_t iter;
  inline bool is_blank() const { return iter == ITER_BLANK; }
  inline bool is_queued() const { return iter == ITER_QUEUED; }
  inline bool is_wall() const { return iter == ITER_WALL; }
  inline bool is_finished() const {
    return !(is_blank() || is_queued() || is_wall());
  }
};

void compute_started();
void compute_finished(result_t res);
result_t send_line(pos_t x, pos_t y, pos_t width, const col_t *data);

template <uint16_t prm_WIDTH, uint16_t prm_HEIGHT> class App {
public:
  static constexpr pos_t SCREEN_W = prm_WIDTH;
  static constexpr pos_t SCREEN_H = prm_HEIGHT;
  col_t line_buff[SCREEN_W];
  iter_t work_buff[SCREEN_W * SCREEN_H];

  static constexpr uint16_t QUEUE_DEPTH = (SCREEN_W + SCREEN_H) * 4;
  loc_t queue_array[QUEUE_DEPTH];
  uint16_t queue_wr_ptr = 0;
  uint16_t queue_rd_ptr = 0;

  real_t center_re = 0;
  real_t center_im = 0;
  int scale_exp = -3;
  real_t step = 0;

  float scroll_accum_x = 0;
  float scroll_accum_y = 0;

  pad_t last_keys = pad_t::NONE;

  App() {}

  result_t init() {
    center_re = -0.5f;
    center_im = 0;

    pos_t p = SCREEN_W > SCREEN_H ? SCREEN_W : SCREEN_H;
    while (p > 0) {
      scale_exp++;
      p /= 2;
    }
    step = real_exp2(-scale_exp);

    FIXBROT_TRY(render(rect_t{0, 0, SCREEN_W, SCREEN_H}));
    return result_t::SUCCESS;
  }

  result_t service(uint32_t delta_us, pad_t keys) {
    if ((keys & pad_t::ZOOM_IN) && !(last_keys & pad_t::ZOOM_IN)) {
      scale_exp++;
      step = real_exp2(-scale_exp);
      FIXBROT_TRY(render(rect_t{0, 0, SCREEN_W, SCREEN_H}));
    } else if ((keys & pad_t::ZOOM_OUT) && !(last_keys & pad_t::ZOOM_OUT)) {
      scale_exp--;
      step = real_exp2(-scale_exp);
      FIXBROT_TRY(render(rect_t{0, 0, SCREEN_W, SCREEN_H}));
    } else {
      if (keys & pad_t::UP) {
        scroll_accum_y -= (float)delta_us * 0.0001f;
      }
      if (keys & pad_t::DOWN) {
        scroll_accum_y += (float)delta_us * 0.0001f;
      }
      if (keys & pad_t::LEFT) {
        scroll_accum_x -= (float)delta_us * 0.0001f;
      }
      if (keys & pad_t::RIGHT) {
        scroll_accum_x += (float)delta_us * 0.0001f;
      }

      loc_t delta{0, 0};
      if (scroll_accum_x >= 1.0f) {
        delta.x = (pos_t)scroll_accum_x;
        scroll_accum_x -= (float)delta.x;
      } else if (scroll_accum_x <= -1.0f) {
        delta.x = (pos_t)scroll_accum_x;
        scroll_accum_x -= (float)delta.x;
      }
      if (scroll_accum_y >= 1.0f) {
        delta.y = (pos_t)scroll_accum_y;
        scroll_accum_y -= (float)delta.y;
      } else if (scroll_accum_y <= -1.0f) {
        delta.y = (pos_t)scroll_accum_y;
        scroll_accum_y -= (float)delta.y;
      }
      if (delta.x != 0 || delta.y != 0) {
        FIXBROT_TRY(scroll(delta));
      }
    }

    for (pos_t y = 0; y < SCREEN_H; y++) {
      for (pos_t x = 0; x < SCREEN_W; x++) {
        iter_t iter = work_buff[y * SCREEN_W + x];
        if (iter < ITER_MAX) {
          uint16_t gray = iter % 64;
          if (gray >= 32) {
            gray = 63 - gray;
          }
          line_buff[x] = (gray << 11) | (gray << 6) | gray;
        } else {
          line_buff[x] = 0x0000;
        }
      }
      FIXBROT_TRY(send_line(0, y, SCREEN_W, line_buff));
    }

    last_keys = keys;
    return result_t::SUCCESS;
  }

private:
  result_t scroll(loc_t delta) {
    pos_t h = SCREEN_H - (delta.y >= 0 ? delta.y : -delta.y);
    pos_t w = SCREEN_W - (delta.x >= 0 ? delta.x : -delta.x);

    iter_t *src_line = work_buff;
    iter_t *dst_line = work_buff;
    if (delta.x >= 0) {
      src_line += delta.x;
    } else {
      dst_line -= delta.x;
    }
    if (delta.y >= 0) {
      src_line += delta.y * SCREEN_W;
      for (pos_t i = 0; i < h; i++) {
        memmove(dst_line, src_line, sizeof(iter_t) * w);
        src_line += SCREEN_W;
        dst_line += SCREEN_W;
      }
    } else {
      src_line += (h - 1) * SCREEN_W;
      dst_line += (h - 1 - delta.y) * SCREEN_W;
      for (pos_t i = 0; i < h; i++) {
        memmove(dst_line, src_line, sizeof(iter_t) * w);
        src_line -= SCREEN_W;
        dst_line -= SCREEN_W;
      }
    }

    center_re += step * delta.x;
    center_im += step * delta.y;
    rect_t rect_v, rect_h;
    rect_v.y = 0;
    rect_v.h = SCREEN_H;
    rect_h.x = 0;
    rect_h.w = w;
    if (delta.x > 0) {
      rect_v.x = SCREEN_W - delta.x;
      rect_v.w = delta.x;
      FIXBROT_TRY(render(rect_v));
    } else if (delta.x < 0) {
      rect_v.x = 0;
      rect_v.w = -delta.x;
      FIXBROT_TRY(render(rect_v));
      rect_h.x = -delta.x;
    }
    if (delta.y > 0) {
      rect_h.y = SCREEN_H - delta.y;
      rect_h.h = delta.y;
      FIXBROT_TRY(render(rect_h));
    } else if (delta.y < 0) {
      rect_h.y = 0;
      rect_h.h = -delta.y;
      FIXBROT_TRY(render(rect_h));
    }

    return result_t::SUCCESS;
  }

  result_t render(rect_t view) {
    compute_started();
    result_t res = render_inner(view);
    compute_finished(res);
    return res;
  }

  result_t render_inner(rect_t view) {
    queue_wr_ptr = 0;
    queue_rd_ptr = 0;
    for (pos_t y = view.y; y < view.y + view.h; y++) {
      memset(work_buff + (y * SCREEN_W + view.x), 0, sizeof(iter_t) * view.w);
    }
    for (pos_t x = view.x; x < view.x + view.w; x++) {
      enqueue(loc_t{x, view.y});
      if (view.h >= 2) {
        enqueue(loc_t{x, (pos_t)(view.y + view.h - 1)});
      }
    }
    for (pos_t y = view.y + 1; y < view.y + view.h - 1; y++) {
      enqueue(loc_t{view.x, y});
      if (view.w >= 2) {
        enqueue(loc_t{(pos_t)(view.x + view.w - 1), y});
      }
    }

    loc_t loc;
    while (dequeue(&loc)) {
      cell_t c;
      c.loc = loc;
      c.iter = mandelbrot(loc);
      work_buff[loc.y * SCREEN_W + loc.x] = c.iter;
      cell_t l = get_cell(view, loc.x - 1, loc.y);
      cell_t r = get_cell(view, loc.x + 1, loc.y);
      cell_t u = get_cell(view, loc.x, loc.y - 1);
      cell_t d = get_cell(view, loc.x, loc.y + 1);
      cell_t lu = get_cell(view, loc.x - 1, loc.y - 1);
      cell_t ru = get_cell(view, loc.x + 1, loc.y - 1);
      cell_t ld = get_cell(view, loc.x - 1, loc.y + 1);
      cell_t rd = get_cell(view, loc.x + 1, loc.y + 1);
      FIXBROT_TRY(compare(c, u, l, lu));
      FIXBROT_TRY(compare(c, d, l, ld));
      FIXBROT_TRY(compare(c, u, r, ru));
      FIXBROT_TRY(compare(c, d, r, rd));
      FIXBROT_TRY(compare(c, l, u, lu));
      FIXBROT_TRY(compare(c, r, u, ru));
      FIXBROT_TRY(compare(c, l, d, ld));
      FIXBROT_TRY(compare(c, r, d, rd));
    }

    iter_t *line_ptr = work_buff + (view.y * SCREEN_W + view.x);
    for (pos_t y = view.y; y < view.y + view.h; y++) {
      iter_t *ptr = line_ptr;
      iter_t last = 1;
      for (pos_t x = view.x; x < view.x + view.w; x++) {
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

    return result_t::SUCCESS;
  }

  iter_t mandelbrot(loc_t loc) {
    if (step.is_fixed32()) {
      return mandelbrot32(loc);
    } else {
      return mandelbrot64(loc);
    }
  }

  iter_t mandelbrot64(loc_t loc) {
    fixed64_t a = center_re + step * (loc.x - SCREEN_W / 2);
    fixed64_t b = center_im + step * (loc.y - SCREEN_H / 2);
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 1;
    fixed64_t four = fixed64_t(4);
    while (iter < ITER_MAX && (xx + yy) <= four) {
      y = x * y * 2 + b;
      x = xx - yy + a;
      xx = x * x;
      yy = y * y;
      iter++;
    }
    return iter;
  }

  iter_t mandelbrot32(loc_t loc) {
    fixed32_t step32 = (fixed32_t)step;
    fixed32_t a = (fixed32_t)center_re + step32 * (loc.x - SCREEN_W / 2);
    fixed32_t b = (fixed32_t)center_im + step32 * (loc.y - SCREEN_H / 2);
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 1;
    fixed32_t four = fixed32_t(4);
    while (iter < ITER_MAX && (xx + yy) <= four) {
      y = x * y * 2 + b;
      x = xx - yy + a;
      xx = x * x;
      yy = y * y;
      iter++;
    }
    return iter;
  }

  cell_t get_cell(rect_t view, pos_t x, pos_t y) {
    loc_t loc{x, y};
    cell_t cell;
    if (view.contains(loc)) {
      cell.loc = loc;
      cell.iter = work_buff[loc.y * SCREEN_W + loc.x];
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
  inline result_t compare(const cell_t &a, const cell_t &b, const cell_t &c,
                          const cell_t &d) {
    if (b.is_finished() && b.iter != a.iter) {
      // b ピクセルが処理完了済みかつ値が a と異なる
      // c, d が未処理であればエンキュー
      if (c.is_blank()) {
        FIXBROT_TRY(enqueue(c.loc));
      }
      if (d.is_blank()) {
        FIXBROT_TRY(enqueue(d.loc));
      }
    }
    return result_t::SUCCESS;
  }

  result_t enqueue(loc_t loc) {
    uint16_t next_wr_ptr = queue_wr_ptr + 1;
    if (next_wr_ptr >= QUEUE_DEPTH) {
      next_wr_ptr = 0;
    }
    if (next_wr_ptr == queue_rd_ptr) {
      return result_t::ERROR_QUEUE_OVERFLOW;
    }

    work_buff[loc.y * SCREEN_W + loc.x] = ITER_QUEUED;
    queue_array[queue_wr_ptr] = loc;
    queue_wr_ptr = next_wr_ptr;
    return result_t::SUCCESS;
  }

  bool dequeue(loc_t *entry) {
    if (queue_rd_ptr == queue_wr_ptr) {
      return false;
    }
    *entry = queue_array[queue_rd_ptr];
    queue_rd_ptr++;
    if (queue_rd_ptr >= QUEUE_DEPTH) {
      queue_rd_ptr = 0;
    }
    return true;
  }
};

} // namespace fixbrot

#endif
