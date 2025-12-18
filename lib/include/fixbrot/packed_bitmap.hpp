#ifndef PACKED_BITMAP_HPP
#define PACKED_BITMAP_HPP

#include <gfxfont.h>

#include "fixbrot/common.hpp"

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
