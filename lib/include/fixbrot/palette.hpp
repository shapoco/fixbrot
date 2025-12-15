#ifndef FIXBROT_PALETTE_HPP
#define FIXBROT_PALETTE_HPP

#include "fixbrot/common.hpp"

namespace fixbrot {

class Palette {
 public:
  static constexpr uint16_t MAX_PALETTE_SIZE = 512;

 private:
  palette_t name = palette_t::HEAT_MAP;
  col_t array[MAX_PALETTE_SIZE] = {0};
  col_t deverged_color = 0x0000;
  uint16_t size = 0;

 public:
  Palette() { init_heat_map(); }

  result_t set_palette(palette_t pal) {
    name = pal;
    switch (pal) {
      case palette_t::HEAT_MAP: init_heat_map(); break;
      case palette_t::RAINBOW: init_rainbow(); break;
      case palette_t::GRAY: init_gray(); break;
      case palette_t::BLACK_WHITE: init_black_white(); break;
      default: init_heat_map(); break;
    }
    return result_t::SUCCESS;
  }

  result_t next() {
    palette_t next =
        static_cast<palette_t>((static_cast<uint16_t>(name) + 1) %
                               static_cast<uint16_t>(palette_t::LAST));
    FIXBROT_TRY(set_palette(next));
    return result_t::SUCCESS;
  }

  FIXBROT_INLINE col_t get_color(iter_t iter) const {
    if (iter == ITER_MAX) {
      return deverged_color;
    } else {
      return array[iter % size];
    }
  }

 private:
  void init_heat_map() {
    size = 32 * 6;
    for (uint16_t i = 0; i < size; i++) {
      int c = i / 32;
      int f = i % 32;
      switch (c) {
        case 0: array[i] = pack565(0, f / 2, f); break;
        case 1: array[i] = pack565(0, 16 + f, 31); break;
        case 2: array[i] = pack565(f, 48 + f / 2, 31); break;
        case 3: array[i] = pack565(31, 63 - f / 2, 31 - f); break;
        case 4: array[i] = pack565(31, 47 - f, 0); break;
        default: array[i] = pack565(31 - f, 15 - f / 2, 0); break;
      }
    }
    deverged_color = 0x0000;
  }

  void init_rainbow() {
    size = 64 * 6;
    for (uint16_t i = 0; i < size; i++) {
      int c = i / 64;
      int f = i % 64;
      switch (c) {
        case 0: array[i] = pack565(31, f, 0); break;
        case 1: array[i] = pack565(31 - (f / 2), 63, 0); break;
        case 2: array[i] = pack565(0, 63, f / 2); break;
        case 3: array[i] = pack565(0, 63 - f, 31); break;
        case 4: array[i] = pack565(f / 2, 0, 31); break;
        default: array[i] = pack565(31, 0, 31 - f / 2); break;
      }
    }
    deverged_color = 0x0000;
  }

  void init_gray() {
    size = 64;
    deverged_color = 0x0000;
    for (uint16_t i = 0; i < size; i++) {
      uint16_t gray = i % 64;
      if (gray >= 32) {
        gray = 63 - gray;
      }
      array[i] = (gray << 11) | (gray << 6) | gray;
    }
  }

  void init_black_white() {
    size = 2;
    array[0] = 0x0000;
    array[1] = 0xFFFF;
    deverged_color = 0x0000;
  }
};

}  // namespace fixbrot

#endif
