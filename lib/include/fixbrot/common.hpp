#ifndef FIXBROT_COMMON_HPP
#define FIXBROT_COMMON_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#endif

#include "fixbrot/fixed32.hpp"
#include "fixbrot/fixed64.hpp"

namespace fixbrot {

enum class result_t : uint16_t {
  SUCCESS,
  ERROR_QUEUE_OVERFLOW,
  ERROR_BUSY,
};

#define FIXBROT_TRY(expr)                    \
  do {                                       \
    fixbrot::result_t res = (expr);          \
    if (res != fixbrot::result_t::SUCCESS) { \
      return res;                            \
    }                                        \
  } while (0)

using iter_t = uint16_t;
using pos_t = int16_t;
using col_t = uint16_t;

using real_t = fixed64_t;

static FIXBROT_INLINE real_t real_exp2(int exp) {
  return fixed64_t::from_raw((int64_t)1 << (fixed64_t::FRAC_BITS + exp));
}

struct vec_t {
  pos_t x;
  pos_t y;
};

struct rect_t {
  pos_t x;
  pos_t y;
  pos_t w;
  pos_t h;

  FIXBROT_INLINE bool contains(vec_t loc) const {
    return (x <= loc.x) && (loc.x < x + w) && (y <= loc.y) && (loc.y < y + h);
  }

  FIXBROT_INLINE pos_t right() const { return x + w; }
  FIXBROT_INLINE pos_t bottom() const { return y + h; }
};

enum class formula_t {
  MANDELBROT,
  BURNING_SHIP,
  CELTIC,
  BUFFALO,
  PERP_BURNING_SHIP,
  AIRSHIP,
  SHARK_FIN,
  POWER_DRILL,
  CROWN,
  SUPER,
  CUBIC_MANDELBROT,
  CUBIC_01344,
  CUBIC_01417,
  CUBIC_01479,
  CUBIC_01856,
  CUBIC_09601,
  CUBIC_09743,
  FEATHER,
  LAST,
};

static constexpr iter_t ITER_BLANK = 0;
static constexpr iter_t ITER_MAX = (1 << (sizeof(iter_t) * 8)) - 3;
static constexpr iter_t ITER_QUEUED = (1 << (sizeof(iter_t) * 8)) - 2;
static constexpr iter_t ITER_WALL = (1 << (sizeof(iter_t) * 8)) - 1;

static constexpr int COARSE_POS_BITS = 4;
static constexpr pos_t COARSE_POS_STEP = 1 << COARSE_POS_BITS;

struct cell_t {
  vec_t loc;
  iter_t iter;
  inline bool is_blank() const { return iter == ITER_BLANK; }
  inline bool is_queued() const { return iter == ITER_QUEUED; }
  inline bool is_wall() const { return iter == ITER_WALL; }
  inline bool is_finished() const {
    return !(is_blank() || is_queued() || is_wall());
  }
};

struct scene_t {
  formula_t formula;
  real_t real;
  real_t imag;
  real_t step;
  iter_t max_iter;
};

enum class builtin_palette_t {
  HEATMAP,
  RAINBOW,
  GRAY,
  STRIPE,
  LAST,
};

static FIXBROT_INLINE builtin_palette_t
next_palette_of(builtin_palette_t palette) {
  return (builtin_palette_t)(((int)palette + 1) % (int)builtin_palette_t::LAST);
}
static FIXBROT_INLINE builtin_palette_t
prev_palette_of(builtin_palette_t palette) {
  return (builtin_palette_t)(((int)palette + (int)builtin_palette_t::LAST - 1) %
                             (int)builtin_palette_t::LAST);
}

static FIXBROT_INLINE col_t pack565(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint16_t)r << 11) | ((uint16_t)g << 5) | b;
}

static FIXBROT_INLINE void unpack565(col_t cpl, uint8_t *r, uint8_t *g,
                                     uint8_t *b) {
  *r = (cpl >> 11) & 0x1F;
  *g = (cpl >> 5) & 0x3F;
  *b = cpl & 0x1F;
}

template <typename T>
static FIXBROT_INLINE T clamp(T min, T max, T val) {
  if (val < min) return min;
  if (val > max) return max;
  return val;
}

static constexpr int BATCH_SIZE_CLOG2 = 8;
static constexpr int BATCH_SIZE = 1 << BATCH_SIZE_CLOG2;

static constexpr int MAX_PALETTE_SIZE = (1 << 9);
static constexpr int MAX_PALETTE_SLOPE = 4;
static constexpr int DEFAULT_PALETTE_SLOPE = 2;

static constexpr int MIN_SCALE_EXP = -3;

}  // namespace fixbrot

#endif
