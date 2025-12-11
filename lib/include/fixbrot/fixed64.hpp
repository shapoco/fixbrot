#ifndef FIXBROT_FIXED64_HPP
#define FIXBROT_FIXED64_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#endif

#include "fixbrot/common.hpp"

namespace fixbrot {

struct fixed64_t {
  static constexpr int FRAC_BITS = 64 - FIXED_INT_BITS;
  int64_t raw;

  inline fixed64_t() : raw(0) {}
  inline fixed64_t(int integer)
      : raw(static_cast<int64_t>(integer) << FRAC_BITS) {}
  inline fixed64_t(float f)
      : raw(static_cast<int64_t>(f * (1ull << FRAC_BITS))) {}

  static inline fixed64_t from_raw(int64_t r) {
    fixed64_t f;
    f.raw = r;
    return f;
  }

  inline fixed64_t operator+(const fixed64_t &other) const {
    return fixed64_t::from_raw(raw + other.raw);
  }

  inline fixed64_t operator-(const fixed64_t &other) const {
    return fixed64_t::from_raw(raw - other.raw);
  }

  inline fixed64_t operator*(const int &other) const {
    return fixed64_t::from_raw(raw * other);
  }

  inline fixed64_t operator*(const short &other) const {
    return fixed64_t::from_raw(raw * other);
  }

  inline fixed64_t operator*(const fixed64_t &other) const {
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

  inline bool is_fixed32() const { return (raw & 0xFFFFFFFF) == 0; }
  inline explicit operator fixed32_t() {
    return fixed32_t::from_raw(raw >> 32);
  }
};

static inline fixed64_t operator+=(fixed64_t &a, const fixed64_t &b) {
  a = a + b;
  return a;
}

static inline fixed64_t operator-=(fixed64_t &a, const fixed64_t &b) {
  a = a - b;
  return a;
}

static inline fixed64_t operator*=(fixed64_t &a, const fixed64_t &b) {
  a = a * b;
  return a;
}

static inline bool operator>(const fixed64_t &a, const fixed64_t &b) {
  return a.raw > b.raw;
}

static inline bool operator<=(const fixed64_t &a, const fixed64_t &b) {
  return a.raw <= b.raw;
}

static inline bool operator<(const fixed64_t &a, const fixed64_t &b) {
  return a.raw < b.raw;
}

static inline bool operator>=(const fixed64_t &a, const fixed64_t &b) {
  return a.raw >= b.raw;
}

static inline bool operator==(const fixed64_t &a, const fixed64_t &b) {
  return a.raw == b.raw;
}

static inline bool operator!=(const fixed64_t &a, const fixed64_t &b) {
  return a.raw != b.raw;
}

} // namespace fixbrot

#endif
