#ifndef FIXBROT_FIXED_COMMON_HPP
#define FIXBROT_FIXED_COMMON_HPP

#ifndef FIXBROT_NO_STDLIB
#include <stdint.h>
#endif

#define FIXBROT_INLINE __attribute__((always_inline)) inline

namespace fixbrot {

static constexpr int FIXED_INT_BITS = 8;

}

#endif
