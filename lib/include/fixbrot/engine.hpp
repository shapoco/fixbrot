#ifndef FIXBROT_ENGINE_HPP
#define FIXBROT_ENGINE_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#include <stdlib.h>
#endif

#include "fixbrot/array_queue.hpp"
#include "fixbrot/common.hpp"

namespace fixbrot {

template <uint16_t prm_REQ_QUEUE_DEPTH, uint16_t prm_RESP_QUEUE_DEPTH>
class Engine {
private:
  ArrayQueue<vec_t, prm_REQ_QUEUE_DEPTH> req_queue;
  ArrayQueue<cell_t, prm_RESP_QUEUE_DEPTH> resp_queue;
  scene_t scene;

public:
  result_t init(scene_t &scene) {
    this->scene = scene;
    req_queue.clear();
    resp_queue.clear();
    return result_t::SUCCESS;
  }

  inline uint16_t load() const { return req_queue.size(); }
  inline result_t dispatch(vec_t loc) { return req_queue.enqueue(loc); }
  inline bool collect(cell_t *resp) { return resp_queue.dequeue(resp); }

  result_t service() {
    int n = req_queue.size();
    vec_t loc;
    for (int i = 0; i < n; i++) {
      if (!req_queue.dequeue(&loc)) {
        break;
      }
      real_t re = scene.re + scene.step * loc.x;
      real_t im = scene.im + scene.step * loc.y;

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
      FIXBROT_TRY(resp_queue.enqueue(resp));
    }
    return result_t::SUCCESS;
  }

private:
  iter_t mandelbrot64(fixed64_t a, fixed64_t b) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < ITER_MAX && (xx + yy).int_part() < 4) {
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
    while (++iter < ITER_MAX && (xx + yy).int_part() < 4) {
      y = x * y * 2 + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }
};

} // namespace fixbrot
#endif
