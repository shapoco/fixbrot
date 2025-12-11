#ifndef FIXBROT_FIXED32_HPP
#define FIXBROT_FIXED32_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#endif

namespace fixbrot {

struct fixed32_t {
  static constexpr int INT_BITS = 8;
  static constexpr int FRAC_BITS = 32 - INT_BITS;
  int32_t raw;

  inline fixed32_t() : raw(0) {}
  inline fixed32_t(int integer)
      : raw(static_cast<int32_t>(integer) << FRAC_BITS) {}
  inline fixed32_t(float f)
      : raw(static_cast<int32_t>(f * (1ull << FRAC_BITS))) {}
  
  static inline fixed32_t from_raw(int32_t r) {
    fixed32_t f;
    f.raw = r;
    return f;
  }

  inline fixed32_t operator+(const fixed32_t &other) const {
    return fixed32_t::from_raw(raw + other.raw);
  }

  inline fixed32_t operator-(const fixed32_t &other) const {
    return fixed32_t::from_raw(raw - other.raw);
  }

  inline fixed32_t operator*(const int &other) const {
    return fixed32_t::from_raw(raw * other);
  }

  inline fixed32_t operator*(const short &other) const {
    return fixed32_t::from_raw(raw * other);
  }

  inline fixed32_t operator*(const fixed32_t &other) const {
    int64_t result = (int64_t)raw * (int64_t)other.raw;
    return fixed32_t::from_raw((int32_t)(result >> FRAC_BITS));
  }
};

static inline fixed32_t operator+=(fixed32_t &a, const fixed32_t &b) {
  a = a + b;
  return a;
}

static inline fixed32_t operator-=(fixed32_t &a, const fixed32_t &b) {
  a = a - b;
  return a;
}

static inline fixed32_t operator*=(fixed32_t &a, const fixed32_t &b) {
  a = a * b;
  return a;
}

static inline bool operator>(const fixed32_t &a, const fixed32_t &b) {
  return a.raw > b.raw;
}

static inline bool operator<=(const fixed32_t &a, const fixed32_t &b) {
  return a.raw <= b.raw;
}

static inline bool operator<(const fixed32_t &a, const fixed32_t &b) {
  return a.raw < b.raw;
}

static inline bool operator>=(const fixed32_t &a, const fixed32_t &b) {
  return a.raw >= b.raw;
}

static inline bool operator==(const fixed32_t &a, const fixed32_t &b) {
  return a.raw == b.raw;
}

static inline bool operator!=(const fixed32_t &a, const fixed32_t &b) {
  return a.raw != b.raw;
}

} // namespace fixbrot

#endif
