#ifndef FIXBROT_HPP
#define FIXBROT_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#include <stdlib.h>
#endif

#include "fixbrot/common.hpp"
#include "fixbrot/engine.hpp"

namespace fixbrot {

void on_render_start(scene_t &scene);
void on_render_finished(result_t res);
result_t on_iterate();
result_t on_dispatch(const vec_t &req);
bool on_collect(cell_t *resp);
result_t on_send_line(pos_t x, pos_t y, pos_t width, const col_t *data);

template <uint16_t prm_WIDTH, uint16_t prm_HEIGHT> class App {
public:
  static constexpr pos_t SCREEN_W = prm_WIDTH;
  static constexpr pos_t SCREEN_H = prm_HEIGHT;
  col_t line_buff[SCREEN_W];
  iter_t work_buff[SCREEN_W * SCREEN_H];

  uint16_t busy_items = 0;

  scene_t scene;

  float scroll_accum_x = 0;
  float scroll_accum_y = 0;

  pad_t last_keys = pad_t::NONE;

  App() {}

  result_t init() {
    scene.re = -0.5f;
    scene.im = 0;
    scene.scale_exp = -2;
    pos_t p = SCREEN_W > SCREEN_H ? SCREEN_W : SCREEN_H;
    while (p > 0) {
      scene.scale_exp++;
      p /= 2;
    }
    scene.step = real_exp2(-scene.scale_exp);

    FIXBROT_TRY(clear_rect(rect_t{0, 0, SCREEN_W, SCREEN_H}));
    FIXBROT_TRY(start_render(rect_t{0, 0, SCREEN_W, SCREEN_H}));
    return result_t::SUCCESS;
  }

  result_t service(uint32_t delta_us, pad_t keys) {
    if (busy_items == 0) {
      if ((keys & pad_t::ZOOM_IN) && !(last_keys & pad_t::ZOOM_IN)) {
        scene.scale_exp++;
        scene.step = real_exp2(-scene.scale_exp);
        FIXBROT_TRY(clear_rect(rect_t{0, 0, SCREEN_W, SCREEN_H}));
        FIXBROT_TRY(start_render(rect_t{0, 0, SCREEN_W, SCREEN_H}));
      } else if ((keys & pad_t::ZOOM_OUT) && !(last_keys & pad_t::ZOOM_OUT)) {
        scene.scale_exp--;
        scene.step = real_exp2(-scene.scale_exp);
        FIXBROT_TRY(clear_rect(rect_t{0, 0, SCREEN_W, SCREEN_H}));
        FIXBROT_TRY(start_render(rect_t{0, 0, SCREEN_W, SCREEN_H}));
      } else {
        if (keys & pad_t::UP) {
          scroll_accum_y -= (float)delta_us * 0.0001f;
        }
        if (keys & pad_t::DOWN) {
          scroll_accum_y += (float)delta_us * 0.0001f;
        }
        if (keys & pad_t::LEFT) {
          scroll_accum_x -= (float)delta_us * 0.0001f;
        }
        if (keys & pad_t::RIGHT) {
          scroll_accum_x += (float)delta_us * 0.0001f;
        }

        vec_t delta{0, 0};
        if (scroll_accum_x >= 1.0f) {
          delta.x = (pos_t)scroll_accum_x;
          scroll_accum_x -= (float)delta.x;
        } else if (scroll_accum_x <= -1.0f) {
          delta.x = (pos_t)scroll_accum_x;
          scroll_accum_x -= (float)delta.x;
        }
        if (scroll_accum_y >= 1.0f) {
          delta.y = (pos_t)scroll_accum_y;
          scroll_accum_y -= (float)delta.y;
        } else if (scroll_accum_y <= -1.0f) {
          delta.y = (pos_t)scroll_accum_y;
          scroll_accum_y -= (float)delta.y;
        }
        if (delta.x != 0 || delta.y != 0) {
          FIXBROT_TRY(scroll(delta));
        }
      }
    } else {
      FIXBROT_TRY(iterate());

      if (busy_items == 0) {
        for (pos_t y = 0; y < SCREEN_H; y++) {
          for (pos_t x = 0; x < SCREEN_W; x++) {
            iter_t iter = work_buff[y * SCREEN_W + x];
            if (iter < ITER_MAX) {
              uint16_t gray = iter % 64;
              if (gray >= 32) {
                gray = 63 - gray;
              }
              line_buff[x] = (gray << 11) | (gray << 6) | gray;
            } else {
              line_buff[x] = 0x0000;
            }
          }
          FIXBROT_TRY(on_send_line(0, y, SCREEN_W, line_buff));
        }
      }
    }

    last_keys = keys;
    return result_t::SUCCESS;
  }

private:
  result_t scroll(vec_t delta) {
    scene.re += scene.step * delta.x;
    scene.im += scene.step * delta.y;

    rect_t dest;
    dest.x = (delta.x >= 0) ? 0 : -delta.x;
    dest.y = (delta.y >= 0) ? 0 : -delta.y;
    dest.h = SCREEN_H - ((delta.y >= 0) ? delta.y : -delta.y);
    dest.w = SCREEN_W - ((delta.x >= 0) ? delta.x : -delta.x);

    // スクロール
    iter_t *src_line = work_buff;
    iter_t *dst_line = work_buff;
    if (delta.x >= 0) {
      src_line += delta.x;
    } else {
      dst_line -= delta.x;
    }
    if (delta.y >= 0) {
      src_line += delta.y * SCREEN_W;
      for (pos_t i = 0; i < dest.h; i++) {
        memmove(dst_line, src_line, sizeof(iter_t) * dest.w);
        src_line += SCREEN_W;
        dst_line += SCREEN_W;
      }
    } else {
      src_line += (dest.h - 1) * SCREEN_W;
      dst_line += (dest.h - 1 - delta.y) * SCREEN_W;
      for (pos_t i = 0; i < dest.h; i++) {
        memmove(dst_line, src_line, sizeof(iter_t) * dest.w);
        src_line -= SCREEN_W;
        dst_line -= SCREEN_W;
      }
    }

    // 新しい領域をクリア
    if (delta.x > 0) {
      FIXBROT_TRY(clear_rect(rect_t{dest.right(), 0, delta.x, SCREEN_H}));
    } else if (delta.x < 0) {
      FIXBROT_TRY(clear_rect(rect_t{0, 0, (pos_t)-delta.x, SCREEN_H}));
    }
    if (delta.y > 0) {
      FIXBROT_TRY(clear_rect(rect_t{dest.x, dest.bottom(), dest.w, delta.y}));
    } else if (delta.y < 0) {
      FIXBROT_TRY(clear_rect(rect_t{dest.x, 0, dest.w, (pos_t)-delta.y}));
    }

    // 新しい領域をレンダリング
    FIXBROT_TRY(start_render({0, 0, SCREEN_W, SCREEN_H}));
    if (delta.x != 0) {
      pos_t x0 = (delta.x > 0) ? (dest.right() - 1) : dest.x;
      pos_t x1 = (delta.x > 0) ? dest.right() : (dest.x - 1);
      cell_t a = get_cell(x0, dest.y);
      cell_t c = get_cell(x1, dest.y);
      for (pos_t y = dest.y; y < dest.y + dest.h - 1; y++) {
        cell_t b = get_cell(x0, y + 1);
        cell_t d = get_cell(x1, y + 1);
        FIXBROT_TRY(compare(a, b, c, d));
        a = b;
        c = d;
      }
    }
    if (delta.y != 0) {
      pos_t y0 = (delta.y > 0) ? (dest.bottom() - 1) : dest.y;
      pos_t y1 = (delta.y > 0) ? dest.bottom() : (dest.y - 1);
      cell_t a = get_cell(dest.x, y0);
      cell_t c = get_cell(dest.x, y1);
      for (pos_t x = dest.x; x < dest.x + dest.w - 1; x++) {
        cell_t b = get_cell(x + 1, y0);
        cell_t d = get_cell(x + 1, y1);
        FIXBROT_TRY(compare(a, b, c, d));
        a = b;
        c = d;
      }
    }

    return result_t::SUCCESS;
  }

  result_t clear_rect(rect_t view) {
    iter_t *line_ptr = work_buff + (view.y * SCREEN_W + view.x);
    for (pos_t i = 0; i < view.h; i++) {
      memset(line_ptr, 0, sizeof(iter_t) * view.w);
      line_ptr += SCREEN_W;
    }
    return result_t::SUCCESS;
  }

  result_t start_render(rect_t view) {
    if (busy_items > 0) {
      return result_t::ERROR_BUSY;
    }
    on_render_start(scene);

    scene.view = view;

    for (pos_t x = view.x; x < view.x + view.w; x++) {
      enqueue(vec_t{x, view.y});
      if (view.h >= 2) {
        enqueue(vec_t{x, (pos_t)(view.y + view.h - 1)});
      }
    }
    for (pos_t y = view.y + 1; y < view.y + view.h - 1; y++) {
      enqueue(vec_t{view.x, y});
      if (view.w >= 2) {
        enqueue(vec_t{(pos_t)(view.x + view.w - 1), y});
      }
    }
    return result_t::SUCCESS;
  }

  result_t iterate() {
    if (busy_items == 0) {
      return result_t::SUCCESS;
    }

    while (true) {
      FIXBROT_TRY(on_iterate());
      cell_t c;
      if (!dequeue(&c)) {
        break;
      }
      pos_t x = c.loc.x;
      pos_t y = c.loc.y;
      work_buff[y * SCREEN_W + x] = c.iter;
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

    if (busy_items == 0) {
      rect_t &view = scene.view;
      iter_t *line_ptr = work_buff + (view.y * SCREEN_W + view.x);
      for (pos_t y = view.y; y < view.y + view.h; y++) {
        iter_t *ptr = line_ptr;
        iter_t last = 1;
        for (pos_t x = view.x; x < view.x + view.w; x++) {
          iter_t iter = *ptr;
          if (iter == ITER_BLANK) {
            *ptr = last;
          } else {
            last = iter;
          }
          ptr++;
        }
        line_ptr += SCREEN_W;
      }

      on_render_finished(result_t::SUCCESS);
    }

    return result_t::SUCCESS;
  }

  inline cell_t get_cell(pos_t x, pos_t y) {
    vec_t loc{x, y};
    cell_t cell;
    if (scene.view.contains(loc)) {
      cell.loc = loc;
      cell.iter = work_buff[y * SCREEN_W + x];
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
    // if (loc.x < 0 || SCREEN_W <= loc.x || loc.y < 0 || SCREEN_H <= loc.y) {
    //   return result_t::SUCCESS;
    // }
    iter_t &target = work_buff[loc.y * SCREEN_W + loc.x];
    if (target != ITER_BLANK) {
      return result_t::SUCCESS;
    }
    target = ITER_QUEUED;
    FIXBROT_TRY(on_dispatch(loc));
    busy_items++;
    return result_t::SUCCESS;
  }

  bool dequeue(cell_t *cell) {
    if (on_collect(cell)) {
      busy_items--;
      return true;
    } else {
      return false;
    }
  }
};

} // namespace fixbrot

#endif
