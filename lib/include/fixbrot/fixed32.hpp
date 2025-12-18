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
