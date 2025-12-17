#ifndef FIXBROT_ENGINE_HPP
#define FIXBROT_ENGINE_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#include <stdlib.h>
#endif

#include "fixbrot/common.hpp"

namespace fixbrot {

class Engine {
 public:
  using index_t = uint32_t;
  static constexpr index_t DEPTH = BATCH_SIZE;

 private:
  cell_t queue[DEPTH];
  volatile index_t wr_ptr = 0;
  volatile index_t proc_ptr = 0;
  volatile index_t rd_ptr = 0;
  scene_t scene;

 public:
  result_t init(scene_t &scene) {
    this->scene = scene;
    wr_ptr = 0;
    proc_ptr = 0;
    rd_ptr = 0;
    return result_t::SUCCESS;
  }

  FIXBROT_INLINE bool full() const {
    return ((wr_ptr + 1) & (DEPTH - 1)) == rd_ptr;
  }

  FIXBROT_INLINE bool empty() const { return rd_ptr == wr_ptr; }

  FIXBROT_INLINE index_t num_queued() const {
    return (wr_ptr - proc_ptr) & (DEPTH - 1);
  }

  FIXBROT_INLINE index_t num_processed() const {
    return (proc_ptr - rd_ptr) & (DEPTH - 1);
  }

  FIXBROT_INLINE result_t dispatch(vec_t loc) {
    index_t wp = wr_ptr;
    index_t next_wp = (wp + 1) & (DEPTH - 1);
    if (next_wp == rd_ptr) {
      return result_t::ERROR_QUEUE_OVERFLOW;
    }
    queue[wp].loc = loc;
    wr_ptr = next_wp;
    return result_t::SUCCESS;
  }

  FIXBROT_INLINE bool collect(cell_t *resp) {
    index_t rp = rd_ptr;
    if (rp == proc_ptr) return false;
    *resp = queue[rp];
    rd_ptr = (rp + 1) & (DEPTH - 1);
    return true;
  }

  result_t service() {
    int n = num_queued();
    vec_t loc;
    while (n-- > 0 && fetch(&loc)) {
      real_t re = scene.real + scene.step * loc.x;
      real_t im = scene.imag + scene.step * loc.y;

      cell_t resp;
      resp.loc = loc;
      if (scene.step.is_fixed32()) {
        fixed32_t a = (fixed32_t)re;
        fixed32_t b = (fixed32_t)im;
        resp.iter = mandelbrot32(a, b);
      } else {
        fixed64_t a = re;
        fixed64_t b = im;
        resp.iter = mandelbrot64(a, b);
      }
      if (resp.iter == scene.max_iter) {
        resp.iter = ITER_MAX;
      }
      advance(resp.iter);
    }
    return result_t::SUCCESS;
  }

 private:
  FIXBROT_INLINE bool fetch(vec_t *loc) {
    index_t pp = proc_ptr;
    if (pp == wr_ptr) return false;
    *loc = queue[pp].loc;
    return true;
  }

  FIXBROT_INLINE void advance(iter_t iter) {
    index_t pp = proc_ptr;
    queue[pp].iter = iter;
    proc_ptr = (pp + 1) & (DEPTH - 1);
  }

  iter_t mandelbrot64(fixed64_t a, fixed64_t b) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < scene.max_iter && (xx + yy).int_part() < 4) {
      y = x * y * 2 + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  iter_t mandelbrot32(fixed32_t a, fixed32_t b) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < scene.max_iter && (xx + yy).int_part() < 4) {
      y = x * y * 2 + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }
};

}  // namespace fixbrot
#endif
