#ifndef FIXBROT_H
#define FIXBROT_H

#ifndef FIXBROT_HPP
#define FIXBROT_HPP

// #include "fixbrot/array_queue.hpp"

#ifndef FIXBROT_QUEUE_HPP
#define FIXBROT_QUEUE_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#include <stdlib.h>
#endif

// #include "fixbrot/common.hpp"

#ifndef FIXBROT_COMMON_HPP
#define FIXBROT_COMMON_HPP

#ifndef FIXBROT_ITER_12BIT
#define FIXBROT_ITER_12BIT (0)
#endif

#ifndef FIXBROT_ARGB4444_BSWAP
#define FIXBROT_ARGB4444_BSWAP (0)
#endif

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#endif

// #include "fixbrot/fixed32.hpp"

#ifndef FIXBROT_FIXED32_HPP
#define FIXBROT_FIXED32_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#endif

// #include "fixbrot/fixed_common.hpp"

#ifndef FIXBROT_FIXED_COMMON_HPP
#define FIXBROT_FIXED_COMMON_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#endif

#define FIXBROT_INLINE __attribute__((always_inline)) inline

namespace fixbrot {

static constexpr int FIXED_INT_BITS = 8;

}

#endif

namespace fixbrot {

struct fixed32_t {
  static constexpr int FRAC_BITS = 32 - FIXED_INT_BITS;
  int32_t raw;

  FIXBROT_INLINE fixed32_t() : raw(0) {}
  FIXBROT_INLINE fixed32_t(int integer)
      : raw(static_cast<int32_t>(integer) << FRAC_BITS) {}
  FIXBROT_INLINE fixed32_t(float f)
      : raw(static_cast<int32_t>(f * (1ull << FRAC_BITS))) {}

  static FIXBROT_INLINE fixed32_t from_raw(int32_t r) {
    fixed32_t f;
    f.raw = r;
    return f;
  }

  FIXBROT_INLINE int int_part() const { return raw >> FRAC_BITS; }

  FIXBROT_INLINE fixed32_t abs() const {
    return (raw < 0) ? fixed32_t::from_raw(-raw) : *this;
  }

  FIXBROT_INLINE fixed32_t square() const {
    int32_t a = raw << (FIXED_INT_BITS / 2);
    int64_t result = a;
    result *= a;
    return fixed32_t::from_raw((int32_t)(result >> 32));
  }

  FIXBROT_INLINE fixed32_t inverse() const {
    int64_t dividend = (int64_t)1ull << (FRAC_BITS * 2);
    return fixed32_t::from_raw((int32_t)(dividend / raw));
  }

  FIXBROT_INLINE fixed32_t operator-() const {
    return fixed32_t::from_raw(-raw);
  }

  FIXBROT_INLINE fixed32_t operator+(const fixed32_t &other) const {
    return fixed32_t::from_raw(raw + other.raw);
  }

  FIXBROT_INLINE fixed32_t operator-(const fixed32_t &other) const {
    return fixed32_t::from_raw(raw - other.raw);
  }

  FIXBROT_INLINE fixed32_t operator*(const int &other) const {
    return fixed32_t::from_raw(raw * other);
  }

  FIXBROT_INLINE fixed32_t operator*(const short &other) const {
    return fixed32_t::from_raw(raw * other);
  }

  FIXBROT_INLINE fixed32_t operator*(const fixed32_t &other) const {
    int64_t result = (int64_t)raw * (int64_t)other.raw;
    return fixed32_t::from_raw((int32_t)(result >> FRAC_BITS));
  }

  FIXBROT_INLINE fixed32_t operator/(const fixed32_t &other) const {
    int64_t dividend = ((int64_t)raw << FRAC_BITS);
    return fixed32_t::from_raw((int32_t)(dividend / other.raw));
  }
};

static FIXBROT_INLINE fixed32_t operator+=(fixed32_t &a, const fixed32_t &b) {
  a = a + b;
  return a;
}

static FIXBROT_INLINE fixed32_t operator-=(fixed32_t &a, const fixed32_t &b) {
  a = a - b;
  return a;
}

static FIXBROT_INLINE fixed32_t operator*=(fixed32_t &a, const fixed32_t &b) {
  a = a * b;
  return a;
}

static FIXBROT_INLINE bool operator>(const fixed32_t &a, const fixed32_t &b) {
  return a.raw > b.raw;
}

static FIXBROT_INLINE bool operator<=(const fixed32_t &a, const fixed32_t &b) {
  return a.raw <= b.raw;
}

static FIXBROT_INLINE bool operator<(const fixed32_t &a, const fixed32_t &b) {
  return a.raw < b.raw;
}

static FIXBROT_INLINE bool operator>=(const fixed32_t &a, const fixed32_t &b) {
  return a.raw >= b.raw;
}

static FIXBROT_INLINE bool operator==(const fixed32_t &a, const fixed32_t &b) {
  return a.raw == b.raw;
}

static FIXBROT_INLINE bool operator!=(const fixed32_t &a, const fixed32_t &b) {
  return a.raw != b.raw;
}

}  // namespace fixbrot

#endif
// #include "fixbrot/fixed64.hpp"

#ifndef FIXBROT_FIXED64_HPP
#define FIXBROT_FIXED64_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#endif

// #include "fixbrot/fixed_common.hpp"


namespace fixbrot {

struct fixed64_t {
  static constexpr int FRAC_BITS = 64 - FIXED_INT_BITS;
  int64_t raw;

  FIXBROT_INLINE fixed64_t() : raw(0) {}
  FIXBROT_INLINE fixed64_t(int integer)
      : raw(static_cast<int64_t>(integer) << FRAC_BITS) {}
  FIXBROT_INLINE fixed64_t(float f)
      : raw(static_cast<int64_t>(f * (1ull << FRAC_BITS))) {}

  static FIXBROT_INLINE fixed64_t from_raw(int64_t r) {
    fixed64_t f;
    f.raw = r;
    return f;
  }

  FIXBROT_INLINE int int_part() const { return raw >> FRAC_BITS; }

  FIXBROT_INLINE fixed64_t abs() const {
    return (raw < 0) ? fixed64_t::from_raw(-raw) : *this;
  }

  FIXBROT_INLINE fixed64_t square() const {
    uint64_t a = (raw < 0) ? -raw : raw;
    a <<= (FIXED_INT_BITS / 2);
    uint32_t al = (uint32_t)(a & 0xFFFFFFFF);
    uint32_t ah = (uint32_t)(a >> 32);
    uint64_t r0 = (uint64_t)al * al;
    uint64_t r1 = (uint64_t)al * ah;
    uint64_t r2 = (uint64_t)ah * ah;
    uint64_t result = r2 + (((r1 << 1) + (r0 >> 32)) >> 32);
    return fixed64_t::from_raw((int64_t)result);
  }

  FIXBROT_INLINE fixed64_t operator-() const {
    return fixed64_t::from_raw(-raw);
  }

  FIXBROT_INLINE fixed64_t operator+(const fixed64_t &other) const {
    return fixed64_t::from_raw(raw + other.raw);
  }

  FIXBROT_INLINE fixed64_t operator-(const fixed64_t &other) const {
    return fixed64_t::from_raw(raw - other.raw);
  }

  FIXBROT_INLINE fixed64_t operator*(const int &other) const {
    return fixed64_t::from_raw(raw * other);
  }

  FIXBROT_INLINE fixed64_t operator*(const short &other) const {
    return fixed64_t::from_raw(raw * other);
  }

  FIXBROT_INLINE fixed64_t operator*(const fixed64_t &other) const {
    bool a_neg = (raw < 0);
    bool b_neg = (other.raw < 0);
    uint64_t a = a_neg ? -raw : raw;
    uint64_t b = b_neg ? -other.raw : other.raw;
    a <<= (FIXED_INT_BITS / 2);
    b <<= (FIXED_INT_BITS / 2);
    uint32_t ah = (uint32_t)(a >> 32);
    uint32_t al = (uint32_t)(a & 0xFFFFFFFF);
    uint32_t bh = (uint32_t)(b >> 32);
    uint32_t bl = (uint32_t)(b & 0xFFFFFFFF);
    uint64_t rh = (uint64_t)ah * (uint64_t)bh;
    uint64_t rm1 = (uint64_t)ah * (uint64_t)bl;
    uint64_t rm2 = (uint64_t)al * (uint64_t)bh;
    uint64_t rl = (uint64_t)al * (uint64_t)bl;
    uint64_t mid = rm1 + rm2 + (rl >> 32);
    uint64_t result = rh + (mid >> 32);
    if (a_neg ^ b_neg) {
      return fixed64_t::from_raw(-((int64_t)result));
    } else {
      return fixed64_t::from_raw((int64_t)result);
    }
  }

  FIXBROT_INLINE bool is_fixed32() const { return (raw & 0xFFFFFFFF) == 0; }
  FIXBROT_INLINE explicit operator fixed32_t() {
    return fixed32_t::from_raw(raw >> 32);
  }

  void to_decimal_string(char *buf, int buff_size, int frac_digits = 20) {
    int64_t tmp = raw;
    int pos = 0;
    if (tmp < 0) {
      if (pos < buff_size - 1) {
        buf[pos++] = '-';
      } else {
        buf[0] = '\0';
        return;
      }
      tmp = -tmp;
    }

    int int_val = (int)(tmp >> FRAC_BITS);
    tmp -= (int64_t)int_val << FRAC_BITS;
    int int_digits = (int_val >= 100) ? 3 : (int_val >= 10) ? 2 : 1;
    uint8_t int_buf[20];
    for (int i = int_digits - 1; i >= 0; i--) {
      int_buf[i] = (uint8_t)(int_val % 10);
      int_val /= 10;
    }
    for (int i = 0; i < int_digits; i++) {
      if (pos < buff_size - 1) {
        buf[pos++] = '0' + int_buf[i];
      } else {
        buf[0] = '\0';
        return;
      }
    }

    if (frac_digits <= 0) {
      buf[pos] = '\0';
      return;
    }

    if (pos < buff_size - 1) {
      buf[pos++] = '.';
    } else {
      buf[0] = '\0';
      return;
    }

    for (int i = 0; i < frac_digits; i++) {
      tmp *= 10;
      int digit = (int)(tmp >> FRAC_BITS);
      if (pos < buff_size - 1) {
        buf[pos++] = '0' + digit;
      } else {
        buf[0] = '\0';
        return;
      }
      tmp -= (int64_t)digit << FRAC_BITS;
      // if (tmp == 0) {
      //   break;
      // }
    }

    buf[pos] = '\0';
  }
};

static FIXBROT_INLINE fixed64_t operator+=(fixed64_t &a, const fixed64_t &b) {
  a = a + b;
  return a;
}

static FIXBROT_INLINE fixed64_t operator-=(fixed64_t &a, const fixed64_t &b) {
  a = a - b;
  return a;
}

static FIXBROT_INLINE fixed64_t operator*=(fixed64_t &a, const fixed64_t &b) {
  a = a * b;
  return a;
}

static FIXBROT_INLINE bool operator>(const fixed64_t &a, const fixed64_t &b) {
  return a.raw > b.raw;
}

static FIXBROT_INLINE bool operator<=(const fixed64_t &a, const fixed64_t &b) {
  return a.raw <= b.raw;
}

static FIXBROT_INLINE bool operator<(const fixed64_t &a, const fixed64_t &b) {
  return a.raw < b.raw;
}

static FIXBROT_INLINE bool operator>=(const fixed64_t &a, const fixed64_t &b) {
  return a.raw >= b.raw;
}

static FIXBROT_INLINE bool operator==(const fixed64_t &a, const fixed64_t &b) {
  return a.raw == b.raw;
}

static FIXBROT_INLINE bool operator!=(const fixed64_t &a, const fixed64_t &b) {
  return a.raw != b.raw;
}

}  // namespace fixbrot

#endif

namespace fixbrot {

enum class result_t : uint16_t {
  SUCCESS,
  ERROR_QUEUE_OVERFLOW,
  ERROR_BUSY,
  ERROR_TOO_MANY_TOUCHES,
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

#if FIXBROT_ITER_12BIT
static constexpr int ITER_BITS = 12;
#else
static constexpr int ITER_BITS = 16;
#endif

static constexpr iter_t ITER_BLANK = 0;
static constexpr iter_t ITER_MAX = (1 << ITER_BITS) - 3;
static constexpr iter_t ITER_QUEUED = (1 << ITER_BITS) - 2;
static constexpr iter_t ITER_WALL = (1 << ITER_BITS) - 1;

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

static FIXBROT_INLINE constexpr col_t color_pack_from_888(uint8_t r, uint8_t g,
                                                          uint8_t b) {
#if FIXBROT_ARGB4444_BSWAP
  r >>= 4;
  g >>= 4;
  b >>= 4;
  return ((uint16_t)b << 8) | ((uint16_t)g << 12) | r | 0x00F0;
#else
  r >>= 3;
  g >>= 2;
  b >>= 3;
  return ((uint16_t)r << 11) | ((uint16_t)g << 5) | b;
#endif
}

static FIXBROT_INLINE constexpr col_t color_pack(uint8_t r, uint8_t g,
                                                 uint8_t b) {
#if FIXBROT_ARGB4444_BSWAP
  return ((uint16_t)b << 8) | ((uint16_t)g << 12) | r | 0x00F0;
#else
  return ((uint16_t)r << 11) | ((uint16_t)g << 5) | b;
#endif
}

static FIXBROT_INLINE constexpr void color_unpack(col_t col, uint8_t *r,
                                                  uint8_t *g, uint8_t *b) {
#if FIXBROT_ARGB4444_BSWAP
  *r = col & 0x0F;
  *g = (col >> 12) & 0x0F;
  *b = (col >> 8) & 0x0F;
#else
  *r = (col >> 11) & 0x1F;
  *g = (col >> 5) & 0x3F;
  *b = col & 0x1F;
#endif
}

static FIXBROT_INLINE constexpr col_t color_div2(col_t c) {
  c >>= 1;
#if FIXBROT_ARGB4444_BSWAP
  return (c & 0x7707) | 0x00F0;
#else
  return c & 0x7BEF;
#endif
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

namespace fixbrot {

template <typename prm_TData>
class ArrayQueue {
 public:
  using TData = prm_TData;
  using index_t = uint32_t;
  const index_t depth;

  volatile index_t wr_ptr = 0;
  volatile index_t rd_ptr = 0;

  TData *array;

  ArrayQueue(index_t depth) : depth(depth) { array = new TData[depth]; }

  ~ArrayQueue() { delete[] array; }

  FIXBROT_INLINE index_t size() const {
    index_t rp = rd_ptr;
    index_t wp = wr_ptr;
    if (wp >= rp) {
      return wp - rp;
    } else {
      return (depth - rp) + wp;
    }
  }

  FIXBROT_INLINE bool empty() const { return rd_ptr == wr_ptr; }
  FIXBROT_INLINE bool full() const { return size() >= (depth - 1); }

  FIXBROT_INLINE void clear() {
    rd_ptr = 0;
    wr_ptr = 0;
  }

  result_t enqueue(const TData &data) {
    index_t rp = rd_ptr;
    index_t wp = wr_ptr;
    index_t wp_next = wp + 1;
    if (wp_next >= depth) {
      wp_next = 0;
    }
    if (wp_next == rp) {
      return result_t::ERROR_QUEUE_OVERFLOW;
    }
    array[wp] = data;
    wr_ptr = wp_next;
    return result_t::SUCCESS;
  }

  bool dequeue(TData *entry) {
    index_t rp = rd_ptr;
    index_t wp = wr_ptr;
    if (rp == wp) {
      return false;
    }
    *entry = array[rp];
    rp++;
    if (rp >= depth) {
      rp = 0;
    }
    rd_ptr = rp;
    return true;
  }
};

}  // namespace fixbrot

#endif// #include "fixbrot/common.hpp"

// #include "fixbrot/gui.hpp"

#ifndef FIXBROT_GBUI_HPP
#define FIXBROT_GBUI_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#endif

// #include "fixbrot/common.hpp"

// #include "fixbrot/packed_bitmap.hpp"

#ifndef PACKED_BITMAP_HPP
#define PACKED_BITMAP_HPP

#include <gfxfont.h>

// #include "fixbrot/common.hpp"


#define FIXBROT_INLINE __attribute__((always_inline)) inline

namespace fixbrot {

template <int prm_BPP>
class PackedBitmap {
 public:
  static constexpr int BPP = prm_BPP;
  static constexpr int PIXS_PER_BYTE = 8 / BPP;
  const pos_t width;
  const pos_t height;
  const pos_t stride;

 private:
  uint8_t *buff;

 public:
  PackedBitmap(pos_t width, pos_t height)
      : width(width),
        height(height),
        stride((width * BPP + 7) / 8),
        buff(new uint8_t[stride * height]) {}

  ~PackedBitmap() { delete[] buff; }

  FIXBROT_INLINE uint8_t *pixel_pointer(pos_t x, pos_t y) {
    return buff + (y * stride) + (x / PIXS_PER_BYTE);
  }

  void render_to(pos_t sx, pos_t sy, pos_t w, pos_t h, col_t *dest_buff,
                 int dest_stride, const col_t *palette) {
    uint8_t *rd_line_ptr = pixel_pointer(sx, sy);
    for (pos_t iy = 0; iy < h; iy++) {
      uint8_t *rd_ptr = rd_line_ptr;
      uint8_t rd_byte = *(rd_ptr++);
      rd_byte <<= (sx % PIXS_PER_BYTE) * BPP;
      col_t *wr_ptr = dest_buff;
      for (pos_t ix = 0; ix < w; ix++) {
        uint8_t color = (rd_byte >> 6) & ((1 << BPP) - 1);
        *(wr_ptr++) = palette[color];
        rd_byte <<= BPP;
        if (((sx + ix + 1) % PIXS_PER_BYTE) == 0) {
          rd_byte = *(rd_ptr++);
        }
      }
      rd_line_ptr += stride;
      dest_buff += dest_stride;
    }
  }

  void clear(uint8_t color = 0) {
    color = extend_pixel(color);
    uint8_t *line_ptr = buff;
    for (pos_t iy = 0; iy < height; iy++) {
      uint8_t *wr_ptr = line_ptr;
      for (pos_t ix = 0; ix < stride; ix++) {
        *(wr_ptr++) = color;
      }
      line_ptr += stride;
    }
  }

  void fill_rect(pos_t x, pos_t y, pos_t w, pos_t h, uint8_t color) {
    color = extend_pixel(color);

    pos_t ix_start = x / PIXS_PER_BYTE;
    pos_t ix_end = (x + w - 1) / PIXS_PER_BYTE;
    uint8_t start_mask = 0xFF >> ((x * BPP) % 8);
    uint8_t end_mask = 0xFF << (8 - ((x + w) * BPP) % 8);
    if (ix_start == ix_end) {
      start_mask &= end_mask;
    }

    uint8_t *line_ptr = pixel_pointer(x, y);
    for (pos_t iy = 0; iy < h; iy++) {
      uint8_t *p = line_ptr;
      *(p++) = (*(p) & ~start_mask) | (color & start_mask);
      for (pos_t ix = ix_start + 1; ix < ix_end; ix++) {
        *(p++) = color;
      }
      if (ix_start != ix_end) {
        *(p++) = (*(p) & ~end_mask) | (color & end_mask);
      }
      line_ptr += stride;
    }
  }

  pos_t draw_char(pos_t x, pos_t y, char c, const GFXfont &font,
                  uint8_t color) {
    color = extend_pixel(color);

    if (c < font.first || font.last < c) {
      return x;
    }
    GFXglyph glyph = font.glyph[c - font.first];

    x += glyph.xOffset;
    y += glyph.yOffset;

    const uint8_t *rd_ptr = font.bitmap + glyph.bitmapOffset;
    int rd_bits = 0;

    uint8_t rd_byte = 0;
    for (pos_t iy = 0; iy < glyph.height; iy++) {
      pos_t dy = y + iy;
      // uint8_t *wr_ptr = line_ptr;
      // uint8_t wr_byte = 0;
      // uint8_t mask = ((1 << BPP) - 1) << ((PIXS_PER_BYTE - 1) * BPP);
      // mask >>= (x % PIXS_PER_BYTE) * BPP;
      uint8_t *line_ptr = nullptr;
      uint8_t *wr_ptr = nullptr;
      uint8_t wr_byte = 0;
      if (0 <= dy && dy < height) {
        line_ptr = pixel_pointer(0, dy);
      }
      for (pos_t ix = 0; ix < glyph.width; ix++) {
        pos_t dx = x + ix;

        if (rd_bits == 0) rd_byte = *(rd_ptr++);
        rd_bits = (rd_bits + 1) % 8;

        bool set = (rd_byte & 0x80) != 0;
        rd_byte <<= 1;

        if (!line_ptr) continue;
        if (dx < 0 || width <= dx) continue;

        if (dx % PIXS_PER_BYTE == 0 || ix == 0) {
          wr_ptr = line_ptr + (dx / PIXS_PER_BYTE);
          wr_byte = *wr_ptr;
        }

        uint8_t shift = (PIXS_PER_BYTE - 1 - (dx % PIXS_PER_BYTE)) * BPP;
        uint8_t pixel_mask = ((1 << BPP) - 1) << shift;
        if (set) {
          wr_byte &= ~pixel_mask;
          wr_byte |= (color & pixel_mask);
        }

        if ((dx + 1) % PIXS_PER_BYTE == 0 || ix + 1 == glyph.width) {
          *(wr_ptr++) = wr_byte;
        }
      }
      line_ptr += stride;
    }

    return x + glyph.xAdvance;
  }

  pos_t draw_text(pos_t x, pos_t y, pos_t w, const char *text,
                  const GFXfont &font, uint8_t color) {
    pos_t r = x + w;
    pos_t px = x;
    while (*text) {
      char c = *text++;
      if (c < font.first || font.last < c) continue;
      const GFXglyph &glyph = font.glyph[c - font.first];

      if (px + glyph.xOffset + glyph.width > r) {
        y += font.yAdvance;
        px = x;
      }
      px = draw_char(px, y, c, font, color);
    }

    return y + font.yAdvance;
  }

  static FIXBROT_INLINE uint8_t extend_pixel(uint8_t color) {
    if (BPP <= 1) {
      color &= 0x1;
      color |= (color << 1);
      color |= (color << 2);
      color |= (color << 4);
      return color;
    } else if (BPP <= 2) {
      color &= 0x3;
      color |= (color << 2);
      color |= (color << 4);
      return color;
    } else {
      color &= 0xF;
      color |= (color << 4);
      return color;
    }
  }
};

class MonoBitmap : public PackedBitmap<1> {
 public:
  MonoBitmap(pos_t width, pos_t height) : PackedBitmap<1>(width, height) {}
};

class Gray2Bitmap : public PackedBitmap<2> {
 public:
  Gray2Bitmap(pos_t width, pos_t height) : PackedBitmap<2>(width, height) {}
};

class Gray4Bitmap : public PackedBitmap<4> {
 public:
  Gray4Bitmap(pos_t width, pos_t height) : PackedBitmap<4>(width, height) {}
};

}  // namespace fixbrot

#endif
// #include "fixbrot/renderer.hpp"

#ifndef FIXBROT_RENDERER_HPP
#define FIXBROT_RENDERER_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#endif

// #include "fixbrot/array_queue.hpp"

// #include "fixbrot/common.hpp"

// #include "fixbrot/mandelbrot.hpp"

#ifndef FIXBROT_MANDELBROT_HPP
#define FIXBROT_MANDELBROT_HPP

// #include "fixbrot/common.hpp"


namespace fixbrot {

class Mandelbrot {
 public:
  static iter_t compute(const scene_t &scene, vec_t loc) {
    real_t re64 = scene.real + scene.step * loc.x;
    real_t im64 = scene.imag + scene.step * loc.y;
    iter_t max_iter = scene.max_iter;
    if (scene.step.is_fixed32()) {
      fixed32_t re32 = (fixed32_t)re64;
      fixed32_t im32 = (fixed32_t)im64;
      switch (scene.formula) {
        case formula_t::BURNING_SHIP:
          return burning_ship32(re32, im32, max_iter);
        case formula_t::CELTIC:
          return celtic32(re32, im32, max_iter);
        case formula_t::BUFFALO:
          return buffalo32(re32, im32, max_iter);
        case formula_t::PERP_BURNING_SHIP:
          return perp_burning_ship32(re32, im32, max_iter);
        case formula_t::AIRSHIP:
          return airship32(re32, im32, max_iter);
        case formula_t::SHARK_FIN:
          return shark_fin32(re32, im32, max_iter);
        case formula_t::POWER_DRILL:
          return power_drill32(re32, im32, max_iter);
        case formula_t::CROWN:
          return crown32(re32, im32, max_iter);
        case formula_t::SUPER:
          return super32(re32, im32, max_iter);
        case formula_t::CUBIC_MANDELBROT:
          return cubic_mandelbrot_32(re32, im32, max_iter);
        case formula_t::CUBIC_01344:
          return cubic_01344_32(re32, im32, max_iter);
        case formula_t::CUBIC_01417:
          return cubic_01417_32(re32, im32, max_iter);
        case formula_t::CUBIC_01479:
          return cubic_01479_32(re32, im32, max_iter);
        case formula_t::CUBIC_01856:
          return cubic_01856_32(re32, im32, max_iter);
        case formula_t::CUBIC_09601:
          return cubic_09601_32(re32, im32, max_iter);
        case formula_t::CUBIC_09743:
          return cubic_09743_32(re32, im32, max_iter);
        case formula_t::FEATHER:
          return feather32(re32, im32, max_iter);
        default:  // formula_t::MANDELBROT:
          return mandelbrot32(re32, im32, max_iter);
      }
    } else {
      switch (scene.formula) {
        case formula_t::BURNING_SHIP:
          return burning_ship64(re64, im64, max_iter);
        case formula_t::CELTIC:
          return celtic64(re64, im64, max_iter);
        case formula_t::BUFFALO:
          return buffalo64(re64, im64, max_iter);
        case formula_t::PERP_BURNING_SHIP:
          return perp_burning_ship64(re64, im64, max_iter);
        case formula_t::AIRSHIP:
          return airship64(re64, im64, max_iter);
        case formula_t::SHARK_FIN:
          return shark_fin64(re64, im64, max_iter);
        case formula_t::POWER_DRILL:
          return power_drill64(re64, im64, max_iter);
        case formula_t::CROWN:
          return crown64(re64, im64, max_iter);
        case formula_t::SUPER:
          return super64(re64, im64, max_iter);
        case formula_t::CUBIC_MANDELBROT:
          return cubic_mandelbrot_64(re64, im64, max_iter);
        case formula_t::CUBIC_01344:
          return cubic_01344_64(re64, im64, max_iter);
        case formula_t::CUBIC_01417:
          return cubic_01417_64(re64, im64, max_iter);
        case formula_t::CUBIC_01479:
          return cubic_01479_64(re64, im64, max_iter);
        case formula_t::CUBIC_01856:
          return cubic_01856_64(re64, im64, max_iter);
        case formula_t::CUBIC_09601:
          return cubic_09601_64(re64, im64, max_iter);
        case formula_t::CUBIC_09743:
          return cubic_09743_64(re64, im64, max_iter);
        case formula_t::FEATHER:  // not implemented yet
          return max_iter;
        default:  // formula_t::MANDELBROT:
          return mandelbrot64(re64, im64, max_iter);
      }
    }
  }

  static const char *get_name(formula_t f) {
    switch (f) {
      case formula_t::MANDELBROT:
        return "Mandelbrot";
      case formula_t::BURNING_SHIP:
        return "Burning Ship";
      case formula_t::CELTIC:
        return "Celtic";
      case formula_t::BUFFALO:
        return "Buffalo";
      case formula_t::PERP_BURNING_SHIP:
        return "Perp. Burning Ship";
      case formula_t::AIRSHIP:
        return "Airship";
      case formula_t::SHARK_FIN:
        return "Shark Fin";
      case formula_t::POWER_DRILL:
        return "Power Drill";
      case formula_t::CROWN:
        return "Crown";
      case formula_t::SUPER:
        return "Super";
      case formula_t::CUBIC_MANDELBROT:
        return "Cubic Mandelbrot";
      case formula_t::CUBIC_01344:
        return "Cubic #01344";
      case formula_t::CUBIC_01417:
        return "Cubic #01417";
      case formula_t::CUBIC_01479:
        return "Cubic #01479";
      case formula_t::CUBIC_01856:
        return "Cubic #01856";
      case formula_t::CUBIC_09601:
        return "Cubic #09601";
      case formula_t::CUBIC_09743:
        return "Cubic #09743";
      case formula_t::FEATHER:
        return "Feather";
      default:
        return "(Unknown)";
    }
  }

 private:
  static iter_t mandelbrot64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      y = x * y * 2 + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t mandelbrot32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      y = x * y * 2 + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t burning_ship64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      y = (x * y * 2).abs() + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t burning_ship32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      y = (x * y * 2).abs() + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t celtic64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      y = x * y * 2 + b;
      x = (xx - yy).abs() + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t celtic32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      y = x * y * 2 + b;
      x = (xx - yy).abs() + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t buffalo64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      y = (x * y).abs() * -2 + b;
      x = (xx - yy).abs() + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t buffalo32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      y = (x * y).abs() * -2 + b;
      x = (xx - yy).abs() + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t perp_burning_ship64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      y = x * y.abs() * 2 + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t perp_burning_ship32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      y = x * y.abs() * 2 + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t airship64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (y >= 0) {
        y = x * y * 2 + b;
        x = xx - yy + a;
      } else {
        y = x * y * -2 + b;
        x = xx + yy + a;
      }
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t airship32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (y >= 0) {
        y = x * y * 2 + b;
        x = xx - yy + a;
      } else {
        y = x * y * -2 + b;
        x = xx + yy + a;
      }
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t shark_fin64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (y < 0) yy = -yy;
      y = x * y * 2 + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t shark_fin32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (y < 0) yy = -yy;
      y = x * y * 2 + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t power_drill64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (y < 0) yy = -yy;
      y = x * y * -2 + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t power_drill32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (y < 0) yy = -yy;
      y = x * y * -2 + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t super64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (y < 0) {
        y = -y;
        yy = -yy;
      }
      y = x * y * 2 + b;
      if (x >= 0) xx = -xx;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t super32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (y < 0) {
        y = -y;
        yy = -yy;
      }
      y = x * y * 2 + b;
      if (x >= 0) xx = -xx;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t crown64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (x < 0) {
        x = -x;
        xx = -xx;
      }
      y = x * y * -2 + b;
      x = (xx - yy).abs() + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t crown32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (x < 0) {
        x = -x;
        xx = -xx;
      }
      y = x * y * -2 + b;
      x = (xx - yy).abs() + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_mandelbrot_64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      fixed64_t xxx = xx * x;
      fixed64_t xxy = xx * y;
      fixed64_t xyy = yy * x;
      fixed64_t yyy = yy * y;
      y = xxy * 3 - yyy + b;
      x = xxx - xyy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_mandelbrot_32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      fixed32_t xxx = xx * x;
      fixed32_t xxy = xx * y;
      fixed32_t xyy = yy * x;
      fixed32_t yyy = yy * y;
      y = xxy * 3 - yyy + b;
      x = xxx - xyy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_01344_64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (x < 0) xx = -xx;
      fixed64_t xxx = xx * x;
      fixed64_t xxy = xx * y;
      fixed64_t xyy = yy * x.abs();
      fixed64_t yyy = yy * y;
      y = xxy * 3 - yyy + b;
      x = xxx - xyy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_01344_32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (x < 0) xx = -xx;
      fixed32_t xxx = xx * x;
      fixed32_t xxy = xx * y;
      fixed32_t xyy = yy * x.abs();
      fixed32_t yyy = yy * y;
      y = xxy * 3 - yyy + b;
      x = xxx - xyy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_01417_64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      fixed64_t xy = x.abs() * y;
      fixed64_t xxx = xx * x;
      fixed64_t xxy = xy * x;
      fixed64_t xyy = xy * y.abs();
      fixed64_t yyy = yy * y;
      y = xxy * 3 + yyy + b;
      x = -xxx - xyy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_01417_32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      fixed32_t xy = x.abs() * y;
      fixed32_t xxx = xx * x;
      fixed32_t xxy = xy * x;
      fixed32_t xyy = xy * y.abs();
      fixed32_t yyy = yy * y;
      y = xxy * 3 + yyy + b;
      x = -xxx - xyy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_01479_64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (x < 0) xx = -xx;
      fixed64_t xxx = xx * x;
      fixed64_t xxy = xx * y;
      fixed64_t xyy = x.abs() * y.abs() * y;
      fixed64_t yyy = yy * y;
      y = xxy * -3 - yyy + b;
      x = -xxx + xyy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_01479_32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (x < 0) xx = -xx;
      fixed32_t xxx = xx * x;
      fixed32_t xxy = xx * y;
      fixed32_t xyy = x.abs() * y.abs() * y;
      fixed32_t yyy = yy * y;
      y = xxy * -3 - yyy + b;
      x = -xxx + xyy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_01856_64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      // temp = ((abs(zr) * zr * zr) - (3 * abs(zr) * zi * zi)) + cr;
      // zi = ((3 * abs(zr) * zr * abs(zi)) - (zi * zi * zi)) + ci;
      // zr = temp;
      if (x < 0) xx = -xx;
      fixed64_t xxx = xx * x;
      fixed64_t xxy = xx * y.abs();
      fixed64_t xyy = yy * x.abs();
      fixed64_t yyy = yy * y;
      y = xxy * 3 - yyy + b;
      x = xxx - xyy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_01856_32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (x < 0) xx = -xx;
      fixed32_t xxx = xx * x;
      fixed32_t xxy = xx * y.abs();
      fixed32_t xyy = yy * x.abs();
      fixed32_t yyy = yy * y;
      y = xxy * 3 - yyy + b;
      x = xxx - xyy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_09601_64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      fixed64_t xxx = x * xx;
      fixed64_t yyy = y * yy;
      fixed64_t xy = x.abs() * y;
      fixed64_t y_abs = y.abs();
      y = ((x * xy * 3) - yyy).abs() + b;
      x = -xxx - xy * y_abs * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_09601_32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      fixed32_t xxx = x * xx;
      fixed32_t yyy = y * yy;
      fixed32_t xy = x.abs() * y;
      fixed32_t y_abs = y.abs();
      y = ((x * xy * 3) - yyy).abs() + b;
      x = -xxx - xy * y_abs * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_09743_64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      fixed64_t xxx = x * xx;
      fixed64_t yyy = y * yy;
      if (x < 0) xx = -xx;
      y = ((xx * y.abs() * -3) + yyy).abs() + b;
      x = -xxx + x * yy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_09743_32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      fixed32_t xxx = x * xx;
      fixed32_t yyy = y * yy;
      if (x < 0) xx = -xx;
      y = ((xx * y.abs() * -3) + yyy).abs() + b;
      x = -xxx + x * yy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t feather32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      fixed32_t xxx = xx * x;
      fixed32_t yyy = yy * y;
      fixed32_t xyy = x * yy;
      fixed32_t yxx = y * xx;
      fixed32_t p = xxx - xyy * 3;
      fixed32_t q = yxx * 3 - yyy;
      fixed32_t r = xx + 1;
      fixed32_t s = yy;
      fixed32_t dsor = (r.square() + s.square()).inverse();
      x = (p * r + q * s) * dsor + a;
      y = (q * r - p * s) * dsor + b;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }
};

}  // namespace fixbrot

#endif

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
#if FIXBROT_ITER_12BIT
  const pos_t stride;
  uint8_t *work_buff;
  uint8_t *copy_buff;
#else
  iter_t *work_buff;
#endif

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
#if FIXBROT_ITER_12BIT
        stride(width * 3 / 2),
        work_buff(new uint8_t[stride * height]),
        copy_buff(new uint8_t[stride]),
#else
        work_buff(new iter_t[width * height]),
#endif
        paint_x_buff(new pos_t[width]) {
  }

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

    pos_t dh = height - ((delta_y >= 0) ? delta_y : -delta_y);
    pos_t dw = width - ((delta_x >= 0) ? delta_x : -delta_x);

    pos_t dx0 = (delta_x >= 0) ? 0 : -delta_x;
    pos_t dy0 = (delta_y >= 0) ? 0 : -delta_y;
    pos_t dx1 = dx0 + dw;
    pos_t dy1 = dy0 + dh;

    pos_t sx0 = dx0 + delta_x;
    pos_t sy0 = dy0 + delta_y;
    pos_t sx1 = sx0 + dw;
    pos_t sy1 = sy0 + dh;

// scroll image data
#if FIXBROT_ITER_12BIT
    {
      int isx0 = (sx0 * 3) / 2;
      int isx1 = (sx1 * 3 + 1) / 2;
      int idx0 = (dx0 * 3) / 2;
      int idx1 = (dx1 * 3 + 1) / 2;
      int nb = isx1 - isx0;
      for (pos_t iy = 0; iy < dh; iy++) {
        pos_t sy, dy;
        if (delta_y * stride + delta_x >= 0) {
          sy = sy0 + iy;
          dy = dy0 + iy;
        } else {
          sy = sy0 + (dh - 1 - iy);
          dy = dy0 + (dh - 1 - iy);
        }

        uint8_t *cp_src_ptr = work_buff + sy * stride + isx0;
        if ((sx0 & 1) && !(dx0 & 1)) {
          uint8_t b = cp_src_ptr[0];
          for (int i = 0; i < nb - 1; i++) {
            uint8_t b_next = cp_src_ptr[i + 1];
            copy_buff[i] = ((b_next << 4) & 0xF0) | ((b >> 4) & 0x0F);
            b = b_next;
          }
          cp_src_ptr = copy_buff;
        } else if (!(sx0 & 1) && (dx0 & 1)) {
          uint8_t b = cp_src_ptr[nb - 1];
          for (int i = nb - 1; i >= 1; i--) {
            uint8_t b_next = cp_src_ptr[i - 1];
            copy_buff[i] = ((b << 4) & 0xF0) | ((b_next >> 4) & 0x0F);
            b = b_next;
          }
          cp_src_ptr = copy_buff;
        } else if (sy == dy) {
          memcpy(copy_buff, cp_src_ptr, nb);
          cp_src_ptr = copy_buff;
        }

        int cp_isrc = 0;
        int cp_idst = dy * stride + idx0;
        int cp_len = nb;
        if (dx0 & 1) {
          int i = dy * stride + idx0;
          work_buff[i] = (work_buff[i] & 0xF0) | (cp_src_ptr[0] & 0x0F);
          cp_isrc += 1;
          cp_idst += 1;
          cp_len -= 1;
        }
        if (dx1 & 1) {
          int i = dy * stride + idx1 - 1;
          work_buff[i] = (work_buff[i] & 0x0F) | (cp_src_ptr[nb - 1] & 0xF0);
          cp_len -= 1;
        }
        memcpy(work_buff + cp_idst, cp_src_ptr + cp_isrc, cp_len);
      }
    }
#else
    iter_t *src_line = work_buff;
    iter_t *dst_line = work_buff;
    if (delta_x >= 0) {
      src_line += delta_x;
    } else {
      dst_line -= delta_x;
    }
    if (delta_y >= 0) {
      src_line += delta_y * width;
      for (pos_t i = 0; i < dh; i++) {
        memmove(dst_line, src_line, sizeof(iter_t) * dw);
        src_line += width;
        dst_line += width;
      }
    } else {
      src_line += (dh - 1) * width;
      dst_line += (dh - 1 - delta_y) * width;
      for (pos_t i = 0; i < dh; i++) {
        memmove(dst_line, src_line, sizeof(iter_t) * dw);
        src_line -= width;
        dst_line -= width;
      }
    }
#endif

    // clear new area
    if (delta_x > 0) {
      FIXBROT_TRY(clear_rect(rect_t{dx1, 0, delta_x, height}));
    } else if (delta_x < 0) {
      FIXBROT_TRY(clear_rect(rect_t{0, 0, (pos_t)-delta_x, height}));
    }
    if (delta_y > 0) {
      FIXBROT_TRY(clear_rect(rect_t{dx0, dy1, dw, delta_y}));
    } else if (delta_y < 0) {
      FIXBROT_TRY(clear_rect(rect_t{dx0, 0, dw, (pos_t)-delta_y}));
    }

    // render new area
    FIXBROT_TRY(start_render(false));
    if (delta_x != 0) {
      pos_t x0 = (delta_x > 0) ? (dx1 - 1) : dx0;
      pos_t x1 = (delta_x > 0) ? dx1 : (dx0 - 1);
      FIXBROT_TRY(scan_vert(x0, x1, dy0, dh));
    }
    if (delta_y != 0) {
      pos_t y0 = (delta_y > 0) ? (dy1 - 1) : dy0;
      pos_t y1 = (delta_y > 0) ? dy1 : (dy0 - 1);
      FIXBROT_TRY(scan_hori(dx0, y0, y1, dw));
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
            work_buff_write(dx, dy, work_buff_read(sx, sy));
          } else {
            work_buff_write(dx, dy, ITER_BLANK);
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
            work_buff_write(dx, dy, work_buff_read(sx, sy));
          } else {
            work_buff_write(dx, dy, ITER_BLANK);
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
          if (work_buff_read(x, y) == ITER_MAX) {
            work_buff_write(x, y, ITER_BLANK);
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
      iter_t iter = work_buff_read(sx, sy);
      if (iter == ITER_BLANK) {
        finished = false;
        iter = work_buff_read(sx & 0xFFFE, sy & 0xFFFE);
        if (iter == ITER_BLANK || iter == ITER_QUEUED) {
          constexpr pos_t MASK = ~(COARSE_POS_STEP - 1);
          pos_t sx2 = (sx & MASK) + (COARSE_POS_STEP / 2);
          pos_t sy2 = (sy & MASK) + (COARSE_POS_STEP / 2);
          iter = work_buff_read(sx2, sy2);
          if (iter == ITER_QUEUED) {
            iter = ITER_BLANK;
          }
        }
      }

      col_t c;
      if (ITER_BLANK == iter) {
        c = 0x0000;
      } else if (iter <= ITER_MAX) {
        c = max_iter_color;
        if (iter < scene.max_iter) {
          c = palette[(iter + palette_phase) & (palette_size - 1)];
        }
        if (!finished) {
          c = color_div2(c);
        }
      } else {
        c = color_pack_from_888(0xFF, 0xFF, 0x00);
      }
      line_buff[ix] = c;
    }

    return result_t::SUCCESS;
  }

  result_t paint_finished() {
    paint_requested = false;
    return result_t::SUCCESS;
  }

 private:
  FIXBROT_INLINE iter_t work_buff_read(pos_t x, pos_t y) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
      return 0;
    }
#if FIXBROT_ITER_12BIT
    int i = y * stride + x * 3 / 2;
    if (x % 2 == 0) {
      return work_buff[i] | ((work_buff[i + 1] & 0x0F) << 8);
    } else {
      return ((work_buff[i] & 0xF0) >> 4) | (work_buff[i + 1] << 4);
    }
#else
    return work_buff[y * width + x];
#endif
  }

  FIXBROT_INLINE void work_buff_write(pos_t x, pos_t y, iter_t val) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
      return;
    }
#if FIXBROT_ITER_12BIT
    int i = y * stride + x * 3 / 2;
    if (x % 2 == 0) {
      work_buff[i] = val & 0xFF;
      work_buff[i + 1] = (work_buff[i + 1] & 0xF0) | ((val >> 8) & 0x0F);
    } else {
      work_buff[i] = (work_buff[i] & 0x0F) | ((val << 4) & 0xF0);
      work_buff[i + 1] = (val >> 4) & 0xFF;
    }
#else
    work_buff[y * width + x] = val;
#endif
  }

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
#if FIXBROT_ITER_12BIT
    for (pos_t y = 0; y < view.h; y++) {
      pos_t x0 = view.x;
      pos_t x1 = view.x + view.w;
      if (x0 % 2 == 1) {
        work_buff_write(x0, view.y + y, 0);
      }
      int ix0 = (x0 + 1) / 2 * 3;
      int ix1 = x1 / 2 * 3;
      int n = ix1 - ix0;
      if (n > 0) {
        memset(work_buff + (view.y + y) * stride + ix0, 0, n);
      }
      if (x1 % 2 == 1) {
        work_buff_write(x1 - 1, view.y + y, 0);
      }
    }
#else
    iter_t *line_ptr = work_buff + (view.y * width + view.x);
    for (pos_t i = 0; i < view.h; i++) {
      memset(line_ptr, 0, sizeof(iter_t) * view.w);
      line_ptr += width;
    }
#endif
    return result_t::SUCCESS;
  }

  result_t fill_blank() {
#if FIXBROT_ITER_12BIT
    for (pos_t y = 0; y < height; y++) {
      iter_t last = 1;
      for (pos_t x = 0; x < width; x++) {
        iter_t iter = work_buff_read(x, y);
        if (iter == ITER_BLANK) {
          work_buff_write(x, y, last);
        } else {
          last = iter;
        }
      }
    }
#else
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
#endif
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
      work_buff_write(x, y, c.iter);
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
      fill_blank();
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
#if FIXBROT_ITER_12BIT
        iter_t iter1 = work_buff_read(correct_x, correct_y);
#else
        iter_t iter1 = work_buff[line_ptr + correct_x];
#endif
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
#if FIXBROT_ITER_12BIT
          work_buff_write(xm, correct_y, iter_m);
#else
          work_buff[line_ptr + xm] = iter_m;
#endif
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
      cell.iter = work_buff_read(x, y);
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
#if FIXBROT_ITER_12BIT
    iter_t iter = work_buff_read(loc.x, loc.y);
#else
    int i = loc.y * width + loc.x;
    iter_t iter = work_buff[i];
#endif
    if (iter != ITER_BLANK) {
      return result_t::SUCCESS;
    }
#if FIXBROT_ITER_12BIT
    work_buff_write(loc.x, loc.y, ITER_QUEUED);
#else
    work_buff[i] = ITER_QUEUED;
#endif
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
      int p = i * (256 * 6) / palette_size;
      int c = p / 256;
      int f = p % 256;
      switch (c) {
        case 0:
          palette[i] = color_pack_from_888(0, f / 4, f);
          break;
        case 1:
          palette[i] = color_pack_from_888(0, 64 + f / 2, 255);
          break;
        case 2:
          palette[i] = color_pack_from_888(f, 192 + f / 4, 255);
          break;
        case 3:
          palette[i] = color_pack_from_888(255, 255 - f / 4, 255 - f);
          break;
        case 4:
          palette[i] = color_pack_from_888(255, 191 - f / 2, 0);
          break;
        default:
          palette[i] = color_pack_from_888(255 - f, 63 - f / 4, 0);
          break;
      }
    }
  }

  void palette_load_rainbow(int slope) {
    palette_size = MAX_PALETTE_SIZE >> slope;
    max_iter_color = 0x0000;
    for (uint16_t i = 0; i < palette_size; i++) {
      int p = i * (256 * 6) / palette_size;
      int c = p / 256;
      int f = p % 256;
      switch (c) {
        case 0:
          palette[i] = color_pack(255, f, 0);
          break;
        case 1:
          palette[i] = color_pack(255 - f, 255, 0);
          break;
        case 2:
          palette[i] = color_pack(0, 255, f);
          break;
        case 3:
          palette[i] = color_pack(0, 255 - f, 255);
          break;
        case 4:
          palette[i] = color_pack(f, 0, 255);
          break;
        default:
          palette[i] = color_pack(255, 0, 255 - f);
          break;
      }
    }
  }

  void palette_load_gray(int slope) {
    palette_size = MAX_PALETTE_SIZE >> slope;
    max_iter_color = 0x0000;
    for (uint16_t i = 0; i < palette_size; i++) {
      uint16_t gray = i * 512 / palette_size;
      if (gray >= 256) {
        gray = 511 - gray;
      }
      palette[i] = color_pack(gray, gray, gray);
    }
  }

  void palette_load_stripe(int slope) {
    palette_size = 64 >> slope;
    max_iter_color = 0x0000;
    for (uint16_t i = 0; i < palette_size; i++) {
      if (i < palette_size / 2) {
        palette[i] = color_pack_from_888(224, 224, 224);
      } else {
        palette[i] = color_pack_from_888(32, 32, 32);
      }
    }
  }
};

}  // namespace fixbrot

#endif
// #include "fixbrot/worker.hpp"

#ifndef FIXBROT_WORKER_HPP
#define FIXBROT_WORKER_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#include <stdlib.h>
#endif

// #include "fixbrot/common.hpp"

// #include "fixbrot/mandelbrot.hpp"


namespace fixbrot {

class Worker {
 public:
  using index_t = uint32_t;
  static constexpr index_t DEPTH = BATCH_SIZE;

 private:
  cell_t queue[DEPTH];
  volatile index_t wr_ptr = 0;
  volatile index_t proc_ptr = 0;
  volatile index_t rd_ptr = 0;
  scene_t scene;

 public:
  result_t init(const scene_t &scene) {
    this->scene = scene;
    wr_ptr = 0;
    proc_ptr = 0;
    rd_ptr = 0;
    return result_t::SUCCESS;
  }

  FIXBROT_INLINE bool full() const {
    return ((wr_ptr + 1) & (DEPTH - 1)) == rd_ptr;
  }

  FIXBROT_INLINE bool empty() const { return rd_ptr == wr_ptr; }

  FIXBROT_INLINE index_t num_queued() const {
    return (wr_ptr - proc_ptr) & (DEPTH - 1);
  }

  FIXBROT_INLINE index_t num_processed() const {
    return (proc_ptr - rd_ptr) & (DEPTH - 1);
  }

  FIXBROT_INLINE result_t dispatch(vec_t loc) {
    index_t wp = wr_ptr;
    index_t next_wp = (wp + 1) & (DEPTH - 1);
    if (next_wp == rd_ptr) {
      return result_t::ERROR_QUEUE_OVERFLOW;
    }
    queue[wp].loc = loc;
    wr_ptr = next_wp;
    return result_t::SUCCESS;
  }

  FIXBROT_INLINE bool collect(cell_t *resp) {
    index_t rp = rd_ptr;
    if (rp == proc_ptr) return false;
    *resp = queue[rp];
    rd_ptr = (rp + 1) & (DEPTH - 1);
    return true;
  }

  result_t service() {
    int n = num_queued();
    vec_t loc;
    while (n-- > 0 && fetch(&loc)) {
      cell_t resp;
      resp.loc = loc;
      resp.iter = Mandelbrot::compute(scene, loc);
      if (resp.iter == scene.max_iter) {
        resp.iter = ITER_MAX;
      }
      advance(resp.iter);
    }
    return result_t::SUCCESS;
  }

 private:
  FIXBROT_INLINE bool fetch(vec_t *loc) {
    index_t pp = proc_ptr;
    if (pp == wr_ptr) return false;
    *loc = queue[pp].loc;
    return true;
  }

  FIXBROT_INLINE void advance(iter_t iter) {
    index_t pp = proc_ptr;
    queue[pp].iter = iter;
    proc_ptr = (pp + 1) & (DEPTH - 1);
  }

};

}  // namespace fixbrot
#endif

#define SHAPOFONT_INCLUDE_GFXFONT
// #include "fixbrot/ShapoSansP_s12c09a01w02.h"

#pragma once

// Generated from ShapoFont
//   Pixel Count:
//     Effective:  6804 px
//     Shrinked :  4516 px
//   Estimated Foot Print:
//     Bitmap Data    :   583 Bytes (  6.14 Bytes/glyph)
//     Glyph Table    :   760 Bytes (  8.00 Bytes/glyph)
//     GFXfont Struct :    10 Bytes
//     Total          :  1353 Bytes ( 14.24 Bytes/glyph)
//   Memory Efficiency:  5.029 px/Byte

#ifndef SHAPOFONT_NO_STDLIB
#include <stdint.h>
#endif

#ifdef SHAPOFONT_INCLUDE_AVR_PGMSPACE
#include <avr/pgmspace.h>
#endif

#ifdef SHAPOFONT_INCLUDE_GFXFONT
#include <gfxfont.h>
#endif

#ifndef SHAPOFONT_PROGMEM
#ifdef PROGMEM
#define SHAPOFONT_PROGMEM PROGMEM
#else
#define SHAPOFONT_PROGMEM
#endif
#define SHAPOFONT_PROGMEM_SELF_DEFINED
#endif

#ifndef SHAPOFONT_GFXFONT_NAMESPACE
#define SHAPOFONT_GFXFONT_NAMESPACE
#define SHAPOFONT_GFXFONT_NAMESPACE_SELF_DEFINED
#endif

const uint8_t ShapoSansP_s12c09a01w02Bitmaps[] SHAPOFONT_PROGMEM = {
  0xFF, 0xF3, 0xC0, 0xDE, 0xD3, 0x20, 0x6C, 0xDB, 0xFB, 0x66, 0xCD, 0xBF, 0xB6, 0x6C, 0x18, 0x7E,
  0xDB, 0xD8, 0x7E, 0x1B, 0xDB, 0x7E, 0x18, 0x73, 0x36, 0xCD, 0xE1, 0xD8, 0x0C, 0x06, 0xE1, 0xEC,
  0xDB, 0x33, 0x80, 0x38, 0x6C, 0x6C, 0x38, 0x7B, 0xDE, 0xCC, 0xDE, 0x7B, 0x36, 0xEC, 0xCC, 0xCC,
  0xE6, 0x30, 0xC6, 0x73, 0x33, 0x33, 0x76, 0xC0, 0x18, 0xDB, 0x7E, 0x3C, 0x7E, 0xDB, 0x18, 0x30,
  0xCF, 0xCC, 0x30, 0x18, 0xCC, 0x63, 0x31, 0x8C, 0x66, 0x30, 0x7B, 0xFC, 0xF3, 0xCF, 0x3C, 0xFF,
  0x78, 0x37, 0xF3, 0x33, 0x33, 0x30, 0x7B, 0xFC, 0xC7, 0x39, 0xCE, 0x3F, 0xFC, 0xFF, 0xE3, 0x1E,
  0xFC, 0x3C, 0xFF, 0x78, 0x30, 0xC6, 0x18, 0xDB, 0xFF, 0xC6, 0x18, 0xFF, 0xFC, 0x3E, 0xFC, 0x3C,
  0xFF, 0x78, 0x30, 0xC6, 0x1E, 0xFF, 0x3C, 0xFF, 0x78, 0xFF, 0xFC, 0xC6, 0x18, 0x63, 0x0C, 0x30,
  0x7B, 0xFC, 0xFF, 0x7B, 0x3C, 0xFF, 0x78, 0x7B, 0xFC, 0xF3, 0xFD, 0xE1, 0x8C, 0x30, 0xFC, 0x01,
  0xF8, 0xFC, 0x01, 0xFB, 0xC0, 0x19, 0x99, 0x86, 0x18, 0x60, 0xF8, 0x01, 0xF0, 0xC3, 0x0C, 0x33,
  0x33, 0x00, 0x7B, 0xFC, 0xC3, 0x38, 0xC0, 0x0C, 0x30, 0x3E, 0x63, 0xCF, 0xDB, 0xDB, 0xDB, 0xCF,
  0x60, 0x3E, 0x38, 0x71, 0xB3, 0x6C, 0x7F, 0xFF, 0xE3, 0xC6, 0xFD, 0xFF, 0x1F, 0xFF, 0xD8, 0xF1,
  0xFF, 0xFC, 0x3C, 0xFF, 0x9E, 0x0C, 0x18, 0x39, 0xBF, 0x3C, 0xF9, 0xFB, 0x3E, 0x3C, 0x78, 0xF3,
  0xFE, 0xF8, 0xFF, 0xFC, 0x3E, 0xFB, 0x0C, 0x3F, 0xFC, 0xFF, 0xFC, 0x30, 0xFB, 0xEC, 0x30, 0xC0,
  0x3E, 0x7E, 0xE0, 0xCF, 0xCF, 0xC3, 0xE7, 0x7F, 0x3B, 0xC7, 0x8F, 0x1F, 0xFF, 0xF8, 0xF1, 0xE3,
  0xC6, 0xFF, 0x66, 0x66, 0x6F, 0xF0, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xF3, 0xFE, 0x78, 0xC7, 0x9F,
  0x77, 0xCF, 0x1F, 0x37, 0x67, 0xC6, 0xC3, 0x0C, 0x30, 0xC3, 0x0C, 0x3F, 0xFC, 0xE1, 0xF8, 0x7F,
  0x3F, 0xCF, 0xDE, 0xF7, 0xBC, 0xCF, 0x33, 0xC0, 0xC0, 0xC7, 0x8F, 0x9F, 0xBF, 0xFB, 0xF3, 0xE3,
  0xC6, 0x3C, 0x7E, 0xE7, 0xC3, 0xC3, 0xC3, 0xE7, 0x7E, 0x3C, 0xFD, 0xFF, 0x1E, 0x3F, 0xFF, 0xB0,
  0x60, 0xC0, 0x3C, 0x7E, 0xE7, 0xC3, 0xDB, 0xDF, 0xEE, 0x7F, 0x3B, 0xFD, 0xFF, 0x1E, 0x3F, 0xDF,
  0x37, 0x67, 0xC6, 0x7D, 0xFF, 0x1F, 0x87, 0xC3, 0xF1, 0xFF, 0x7C, 0xFF, 0xFF, 0x18, 0x18, 0x18,
  0x18, 0x18, 0x18, 0x18, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xE7, 0x7E, 0x3C, 0xC3, 0xC3, 0xC3,
  0x66, 0x66, 0x3C, 0x3C, 0x18, 0x18, 0xC0, 0xF0, 0x3C, 0xCF, 0xB7, 0x6D, 0x9F, 0xE7, 0xF8, 0xCC,
  0x33, 0x00, 0xC3, 0xC3, 0x66, 0x7E, 0x3C, 0x7E, 0x66, 0xC3, 0xC3, 0xC3, 0xC3, 0xE7, 0x7E, 0x3C,
  0x18, 0x18, 0x18, 0x18, 0xFF, 0xFC, 0x38, 0xE3, 0x8E, 0x38, 0x7F, 0xFE, 0xFC, 0xCC, 0xCC, 0xCC,
  0xCC, 0xF0, 0xC6, 0x18, 0xC6, 0x18, 0xC6, 0x30, 0xC6, 0xF3, 0x33, 0x33, 0x33, 0x33, 0xF0, 0x31,
  0xEC, 0xE1, 0xEE, 0x73, 0x78, 0x37, 0xF3, 0xFD, 0xD0, 0xC3, 0x0C, 0x36, 0xFF, 0x3C, 0xFF, 0xD8,
  0x7F, 0xF1, 0x8F, 0xBC, 0x0C, 0x30, 0xDB, 0xFF, 0x3C, 0xFF, 0x6C, 0x7B, 0xFC, 0xFF, 0xC1, 0xE0,
  0x3B, 0xD9, 0xFF, 0xB1, 0x8C, 0x60, 0x77, 0xFC, 0xFF, 0x7C, 0x37, 0xDE, 0xC3, 0x0C, 0x3E, 0xFF,
  0x3C, 0xF3, 0xCC, 0x6C, 0x7E, 0xDB, 0x60, 0x6C, 0x36, 0xDB, 0x7F, 0x00, 0xC3, 0x0C, 0x33, 0xDF,
  0xEF, 0xB7, 0xCC, 0xFD, 0xB6, 0xDB, 0x60, 0xB6, 0xFF, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xFE, 0xF3,
  0xCF, 0x30, 0x7B, 0xFC, 0xF3, 0xFD, 0xE0, 0xDB, 0xFC, 0xF3, 0xFF, 0x6C, 0x30, 0x6F, 0xFC, 0xF3,
  0xFD, 0xB0, 0xC3, 0xDF, 0xF9, 0x8C, 0x60, 0x7B, 0x37, 0x07, 0xCD, 0xE0, 0x63, 0x3F, 0xF6, 0x31,
  0xE7, 0xCF, 0x3C, 0xF3, 0xFD, 0xB0, 0xCF, 0x37, 0x9E, 0x30, 0xC0, 0xC3, 0xDB, 0xDB, 0xFF, 0x66,
  0x66, 0xCF, 0xF7, 0x9E, 0xFF, 0x30, 0xC7, 0x8D, 0xB3, 0xE3, 0x83, 0x0C, 0x18, 0xFF, 0xF3, 0x9C,
  0xFF, 0xF0, 0x3B, 0x18, 0xC6, 0x61, 0x8C, 0x63, 0x0E, 0xFF, 0xFF, 0xFC, 0xE1, 0x8C, 0x63, 0x0C,
  0xC6, 0x31, 0xB8, 0x0E, 0x9F, 0xF9, 0x70,
};

const SHAPOFONT_GFXFONT_NAMESPACE GFXglyph ShapoSansP_s12c09a01w02Glyphs[] SHAPOFONT_PROGMEM = {
  { 0x0000,  0,  0,  5,  4,   2 },
  { 0x0000,  2,  9,  3,  0,  -9 },
  { 0x0003,  5,  4,  6,  0, -10 },
  { 0x0006,  7,  9,  8,  0,  -9 },
  { 0x000E,  8,  9,  9,  0,  -9 },
  { 0x0017, 10,  9, 11,  0,  -9 },
  { 0x0023,  8,  9,  9,  0,  -9 },
  { 0x01F5,  3,  4,  4,  0, -10 },
  { 0x002C,  4, 11,  5,  0, -10 },
  { 0x0032,  4, 11,  5,  0, -10 },
  { 0x0038,  8,  7,  9,  0,  -8 },
  { 0x003F,  6,  5,  7,  0,  -7 },
  { 0x01F5,  3,  4,  4,  0,  -2 },
  { 0x0090,  5,  1,  6,  0,  -5 },
  { 0x004B,  3,  2,  4,  0,  -2 },
  { 0x0043,  5, 11,  6,  0, -10 },
  { 0x004A,  6,  9,  7,  0,  -9 },
  { 0x0051,  4,  9,  7,  0,  -9 },
  { 0x0056,  6,  9,  7,  0,  -9 },
  { 0x005D,  6,  9,  7,  0,  -9 },
  { 0x0064,  6,  9,  7,  0,  -9 },
  { 0x006B,  6,  9,  7,  0,  -9 },
  { 0x0072,  6,  9,  7,  0,  -9 },
  { 0x0079,  6,  9,  7,  0,  -9 },
  { 0x0080,  6,  9,  7,  0,  -9 },
  { 0x0087,  6,  9,  7,  0,  -9 },
  { 0x008E,  3,  7,  4,  0,  -7 },
  { 0x0091,  3,  9,  4,  0,  -7 },
  { 0x0095,  5,  7,  6,  0,  -8 },
  { 0x009A,  5,  4,  6,  0,  -6 },
  { 0x009D,  5,  7,  6,  0,  -8 },
  { 0x00A2,  6,  9,  7,  0,  -9 },
  { 0x00A9,  8,  9,  9,  0,  -9 },
  { 0x00B2,  7,  9,  8,  0,  -9 },
  { 0x00BA,  7,  9,  8,  0,  -9 },
  { 0x00C2,  7,  9,  8,  0,  -9 },
  { 0x00CA,  7,  9,  8,  0,  -9 },
  { 0x00D2,  6,  9,  7,  0,  -9 },
  { 0x00D9,  6,  9,  7,  0,  -9 },
  { 0x00E0,  8,  9,  9,  0,  -9 },
  { 0x00E9,  7,  9,  8,  0,  -9 },
  { 0x00F1,  4,  9,  5,  0,  -9 },
  { 0x00F6,  7,  9,  8,  0,  -9 },
  { 0x00FE,  7,  9,  8,  0,  -9 },
  { 0x0106,  6,  9,  7,  0,  -9 },
  { 0x010D, 10,  9, 11,  0,  -9 },
  { 0x0119,  7,  9,  8,  0,  -9 },
  { 0x0121,  8,  9,  9,  0,  -9 },
  { 0x012A,  7,  9,  8,  0,  -9 },
  { 0x0132,  8,  9,  9,  0,  -9 },
  { 0x013B,  7,  9,  8,  0,  -9 },
  { 0x0143,  7,  9,  8,  0,  -9 },
  { 0x014B,  8,  9,  9,  0,  -9 },
  { 0x0154,  8,  9,  9,  0,  -9 },
  { 0x015D,  8,  9,  9,  0,  -9 },
  { 0x0166, 10,  9, 11,  0,  -9 },
  { 0x0172,  8,  9,  9,  0,  -9 },
  { 0x017B,  8,  9,  9,  0,  -9 },
  { 0x0184,  7,  9,  8,  0,  -9 },
  { 0x018C,  4, 11,  5,  0, -10 },
  { 0x0192,  5, 11,  6,  0, -10 },
  { 0x0199,  4, 11,  5,  0, -10 },
  { 0x019F,  6,  4,  7,  0, -10 },
  { 0x004B,  6,  1,  7,  0,  -1 },
  { 0x01A2,  4,  4,  5,  0, -10 },
  { 0x01A4,  6,  6,  7,  0,  -6 },
  { 0x01A9,  6,  9,  7,  0,  -9 },
  { 0x01B0,  5,  6,  6,  0,  -6 },
  { 0x01B4,  6,  9,  7,  0,  -9 },
  { 0x01BB,  6,  6,  7,  0,  -6 },
  { 0x01C0,  5,  9,  6,  0,  -9 },
  { 0x01C6,  6,  8,  7,  0,  -6 },
  { 0x01CC,  6,  9,  7,  0,  -9 },
  { 0x01D3,  3,  9,  4,  0,  -9 },
  { 0x01D7,  3, 11,  4,  0,  -9 },
  { 0x01DC,  6,  9,  7,  0,  -9 },
  { 0x01E3,  3,  9,  4,  0,  -9 },
  { 0x01E7,  8,  6,  9,  0,  -6 },
  { 0x01ED,  6,  6,  7,  0,  -6 },
  { 0x01F2,  6,  6,  7,  0,  -6 },
  { 0x01F7,  6,  8,  7,  0,  -6 },
  { 0x01FD,  6,  8,  7,  0,  -6 },
  { 0x0203,  5,  6,  6,  0,  -6 },
  { 0x0207,  6,  6,  7,  0,  -6 },
  { 0x020C,  5,  8,  6,  0,  -8 },
  { 0x0211,  6,  6,  7,  0,  -6 },
  { 0x0216,  6,  6,  7,  0,  -6 },
  { 0x021B,  8,  6,  9,  0,  -6 },
  { 0x0221,  6,  6,  7,  0,  -6 },
  { 0x0226,  7,  8,  8,  0,  -6 },
  { 0x022D,  6,  6,  7,  0,  -6 },
  { 0x0232,  5, 11,  6,  0, -10 },
  { 0x0239,  2, 11,  3,  0, -10 },
  { 0x023C,  5, 11,  6,  0, -10 },
  { 0x0243,  8,  4,  9,  0,  -6 },
};

const SHAPOFONT_GFXFONT_NAMESPACE GFXfont ShapoSansP_s12c09a01w02 SHAPOFONT_PROGMEM = {
  (uint8_t*)ShapoSansP_s12c09a01w02Bitmaps,
  (GFXglyph*)ShapoSansP_s12c09a01w02Glyphs,
  0x20,
  0x7E,
  14
};

#ifdef SHAPOFONT_PROGMEM_SELF_DEFINED
#undef SHAPOFONT_PROGMEM
#endif

#ifdef SHAPOFONT_GFXFONT_NAMESPACE_SELF_DEFINED
#undef SHAPOFONT_GFXFONT_NAMESPACE
#endif

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
// #include "fixbrot/mandelbrot.hpp"

// #include "fixbrot/packed_bitmap.hpp"

// #include "fixbrot/renderer.hpp"

// #include "fixbrot/worker.hpp"


#endif

#endif
