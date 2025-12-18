#ifndef FIXBROT_MENU_HPP
#define FIXBROT_MENU_HPP

#include "fixbrot/common.hpp"
#include "fixbrot/packed_bitmap.hpp"

#ifndef FIXBROT_NO_STDLIB
#include <string.h>
#endif

namespace fixbrot {
class Menu {
 public:
  static constexpr uint8_t BLACK = 0;
  static constexpr uint8_t WHITE = 3;

  const pos_t width;
  const pos_t height;
  Gray2Bitmap bitmap;
  const GFXfont &font;
  const pos_t padding;

  Menu(pos_t width, pos_t height, const GFXfont &font)
      : width(width),
        height(height),
        bitmap(width, height),
        font(font),
        padding(height / 25) {}

  result_t paint() {
    bitmap.clear(BLACK);
    return result_t::SUCCESS;
  }

  result_t render_to(pos_t sx, pos_t sy, pos_t w, pos_t h, col_t *dest_buff,
                     int dest_stride, const col_t *palette) {
    bitmap.render_to(sx, sy, w, h, dest_buff, dest_stride, palette);
    return result_t::SUCCESS;
  }
};
}  // namespace fixbrot

#endif
