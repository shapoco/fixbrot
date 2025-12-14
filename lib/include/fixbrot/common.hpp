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

#define FIXBROT_TRY(expr)                                                      \
  do {                                                                         \
    result_t res = (expr);                                                     \
    if (res != result_t::SUCCESS) {                                            \
      return res;                                                              \
    }                                                                          \
  } while (0)

enum class input_t : uint16_t {
  NONE = 0,
  SCROLL_LEFT = (1 << 0),
  SCROLL_RIGHT = (1 << 1),
  SCROLL_UP = (1 << 2),
  SCROLL_DOWN = (1 << 3),
  ZOOM_IN = (1 << 4),
  ZOOM_OUT = (1 << 5),
  ITER_INC = (1 << 6),
  ITER_DEC = (1 << 7),
  PALETTE_CHANGE = (1 << 8),
  SCROLL_MASK = SCROLL_UP | SCROLL_DOWN | SCROLL_LEFT | SCROLL_RIGHT,
};

static inline input_t operator&(input_t a, input_t b) {
  return static_cast<input_t>(static_cast<uint16_t>(a) &
                              static_cast<uint16_t>(b));
}
static inline input_t operator|(input_t a, input_t b) {
  return static_cast<input_t>(static_cast<uint16_t>(a) |
                              static_cast<uint16_t>(b));
}
static inline input_t operator~(input_t a) {
  return static_cast<input_t>(~static_cast<uint16_t>(a));
}
static inline input_t &operator|=(input_t &a, input_t b) { return (a = a | b); }
static inline bool operator!(input_t a) {
  return static_cast<uint16_t>(a) == 0;
}

enum class precision_t : uint8_t {
  FIXED32,
  FIXED64,
};

using iter_t = uint16_t;
using pos_t = int16_t;
using col_t = uint16_t;

#ifdef FIXBROT_USE_FLOAT

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

#else

using real_t = fixed64_t;

static inline real_t real_exp2(int exp) {
  return fixed64_t::from_raw((int64_t)1 << (fixed64_t::FRAC_BITS + exp));
}

#endif

struct vec_t {
  pos_t x;
  pos_t y;
};

struct rect_t {
  pos_t x;
  pos_t y;
  pos_t w;
  pos_t h;

  inline bool contains(vec_t loc) const {
    return (x <= loc.x) && (loc.x < x + w) && (y <= loc.y) && (loc.y < y + h);
  }

  inline pos_t right() const { return x + w; }
  inline pos_t bottom() const { return y + h; }
};

static constexpr iter_t ITER_BLANK = 0;
static constexpr iter_t ITER_MAX = (1 << (sizeof(iter_t) * 8)) - 3;
static constexpr iter_t ITER_QUEUED = (1 << (sizeof(iter_t) * 8)) - 2;
static constexpr iter_t ITER_WALL = (1 << (sizeof(iter_t) * 8)) - 1;

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
  real_t real;
  real_t imag;
  real_t step;
  iter_t max_iter;
};

} // namespace fixbrot

#endif
