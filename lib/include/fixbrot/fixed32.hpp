#ifndef FIXBROT_FIXED32_HPP
#define FIXBROT_FIXED32_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#endif

#include "fixbrot/fixed_common.hpp"

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

  FIXBROT_INLINE fixed32_t square() const {
    int32_t a = raw << (FIXED_INT_BITS / 2);
    int64_t result = a;
    result *= a;
    return fixed32_t::from_raw((int32_t)(result >> 32));
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
#if 0
    bool a_neg = (raw < 0);
    bool b_neg = (other.raw < 0);
    uint32_t a = a_neg ? -raw : raw;
    uint32_t b = b_neg ? -other.raw : other.raw;
    a <<= (FIXED_INT_BITS / 2);
    b <<= (FIXED_INT_BITS / 2);
    uint32_t a0 = a & 0xFFFF;
    uint32_t a1 = a >> 16;
    uint32_t b0 = b & 0xFFFF;
    uint32_t b1 = b >> 16;
    uint32_t m0 = a0 * b0;
    uint32_t m1 = a1 * b0;
    uint32_t m2 = a0 * b1;
    uint32_t m3 = a1 * b1;
    uint32_t accum = (m0 >> 16) & 0xFFFF;
    accum += m1;
    accum += m2;
    accum >>= 16;
    accum += m3;
    if (a_neg ^ b_neg) {
      return fixed32_t::from_raw(-((int32_t)accum));
    } else {
      return fixed32_t::from_raw((int32_t)accum);
    }
#else
    int64_t result = (int64_t)raw * (int64_t)other.raw;
    return fixed32_t::from_raw((int32_t)(result >> FRAC_BITS));
#endif
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

} // namespace fixbrot

#endif
