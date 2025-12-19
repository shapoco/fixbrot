#ifndef FIXBROT_FIXED64_HPP
#define FIXBROT_FIXED64_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#endif

#include "fixbrot/fixed_common.hpp"

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
