#ifndef FIXBROT_PALETTE_HPP
#define FIXBROT_PALETTE_HPP

#include "fixbrot/common.hpp"

namespace fixbrot {

class Palette {
 public:
  static constexpr uint16_t MAX_PALETTE_SIZE = 256;

 private:
  pattern_t pattern = pattern_t::HEAT_MAP;
  uint16_t slope = 0;
  uint16_t shift = 0;
  col_t array[MAX_PALETTE_SIZE] = {0};
  uint16_t palette_size = MAX_PALETTE_SIZE;
  col_t deverged_color = 0x0000;

 public:
  Palette() { init_heat_map(); }

  result_t set_palette(pattern_t pal) {
    pattern = pal;

    return result_t::SUCCESS;
  }

  result_t next_pattern() {
    slope++;
    if (slope >= 4) {
      slope = 0;
      pattern = static_cast<pattern_t>((static_cast<uint16_t>(pattern) + 1) %
                                       static_cast<uint16_t>(pattern_t::LAST));
    }
    init_palette();
    return result_t::SUCCESS;
  }

  result_t shift_forward() {
    shift = (shift + MAX_PALETTE_SIZE - 1) % MAX_PALETTE_SIZE;
    return result_t::SUCCESS;
  }

  FIXBROT_INLINE col_t get_color(iter_t iter) const {
    if (iter == ITER_MAX) {
      return deverged_color;
    } else {
      return array[(iter + shift) & (palette_size - 1)];
    }
  }

 private:
  void init_palette() {
    switch (pattern) {
      case pattern_t::HEAT_MAP: init_heat_map(); break;
      case pattern_t::RAINBOW: init_rainbow(); break;
      case pattern_t::GRAY: init_gray(); break;
      case pattern_t::BLACK_WHITE: init_black_white(); break;
      default: init_heat_map(); break;
    }
  }

  void init_heat_map() {
    palette_size = MAX_PALETTE_SIZE >> slope;
    deverged_color = 0x0000;
    for (uint16_t i = 0; i < palette_size; i++) {
      int p = i * (32 * 6) / palette_size;
      int c = p / 32;
      int f = p % 32;
      switch (c) {
        case 0: array[i] = pack565(0, f / 2, f); break;
        case 1: array[i] = pack565(0, 16 + f, 31); break;
        case 2: array[i] = pack565(f, 48 + f / 2, 31); break;
        case 3: array[i] = pack565(31, 63 - f / 2, 31 - f); break;
        case 4: array[i] = pack565(31, 47 - f, 0); break;
        default: array[i] = pack565(31 - f, 15 - f / 2, 0); break;
      }
    }
  }

  void init_rainbow() {
    palette_size = MAX_PALETTE_SIZE >> slope;
    deverged_color = 0x0000;
    for (uint16_t i = 0; i < palette_size; i++) {
      int p = i * (64 * 6) / palette_size;
      int c = p / 64;
      int f = p % 64;
      switch (c) {
        case 0: array[i] = pack565(31, f, 0); break;
        case 1: array[i] = pack565(31 - (f / 2), 63, 0); break;
        case 2: array[i] = pack565(0, 63, f / 2); break;
        case 3: array[i] = pack565(0, 63 - f, 31); break;
        case 4: array[i] = pack565(f / 2, 0, 31); break;
        default: array[i] = pack565(31, 0, 31 - f / 2); break;
      }
    }
  }

  void init_gray() {
    palette_size = MAX_PALETTE_SIZE >> slope;
    deverged_color = 0x0000;
    for (uint16_t i = 0; i < palette_size; i++) {
      uint16_t gray = i * 128 / palette_size;
      if (gray >= 64) {
        gray = 127 - gray;
      }
      array[i] = pack565(gray >> 1, gray, gray >> 1);
    }
  }

  void init_black_white() {
    palette_size = 64 >> slope;
    deverged_color = 0x0000;
    for (uint16_t i = 0; i < palette_size; i++) {
      array[i] =
          (i < palette_size / 2) ? pack565(28, 56, 28) : pack565(4, 8, 4);
    }
  }
};

}  // namespace fixbrot

#endif
