#ifndef FIXBROT_QUEUE_HPP
#define FIXBROT_QUEUE_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#include <stdlib.h>
#endif

#include "fixbrot/common.hpp"

namespace fixbrot {

template <typename prm_TData, size_t prm_DEPTH> class ArrayQueue {
public:
  using TData = prm_TData;
  using index_t = uint32_t;
  static constexpr index_t DEPTH = prm_DEPTH;

  volatile index_t wr_ptr = 0;
  volatile index_t rd_ptr = 0;

  TData array[DEPTH];

  inline index_t size() const {
    index_t rp = rd_ptr;
    index_t wp = wr_ptr;
    if (wp >= rp) {
      return wp - rp;
    } else {
      return (DEPTH - rp) + wp;
    }
  }

  inline bool empty() const { return rd_ptr == wr_ptr; }
  inline bool full() const { return size() >= (DEPTH - 1); }

  void clear() {
    rd_ptr = 0;
    wr_ptr = 0;
  }

  inline result_t enqueue(const TData &data) {
    index_t rp = rd_ptr;
    index_t wp = wr_ptr;
    index_t wp_next = wp + 1;
    if (wp_next >= DEPTH) {
      wp_next = 0;
    }
    if (wp_next == rp) {
      return result_t::ERROR_QUEUE_OVERFLOW;
    }
    array[wp] = data;
    wr_ptr = wp_next;
    return result_t::SUCCESS;
  }

  inline bool dequeue(TData *entry) {
    index_t rp = rd_ptr;
    index_t wp = wr_ptr;
    if (rp == wp) {
      return false;
    }
    *entry = array[rp];
    rp++;
    if (rp >= DEPTH) {
      rp = 0;
    }
    rd_ptr = rp;
    return true;
  }
};

} // namespace fixbrot

#endif