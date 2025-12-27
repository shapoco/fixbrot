#ifndef FIXBROT_MANDELBROT_HPP
#define FIXBROT_MANDELBROT_HPP

#include "fixbrot/common.hpp"

namespace fixbrot {

class Mandelbrot {
 public:
  static iter_t compute(const scene_t &scene, vec_t loc) {
    real_t re64 = scene.real + scene.step * loc.x;
    real_t im64 = scene.imag + scene.step * loc.y;
    iter_t max_iter = scene.max_iter;
    if (scene.step.is_fixed32()) {
      fixed32_t re32 = (fixed32_t)re64;
      fixed32_t im32 = (fixed32_t)im64;
      switch (scene.formula) {
        case formula_t::BURNING_SHIP:
          return burning_ship32(re32, im32, max_iter);
        case formula_t::CELTIC:
          return celtic32(re32, im32, max_iter);
        case formula_t::BUFFALO:
          return buffalo32(re32, im32, max_iter);
        case formula_t::PERP_BURNING_SHIP:
          return perp_burning_ship32(re32, im32, max_iter);
        case formula_t::AIRSHIP:
          return airship32(re32, im32, max_iter);
        case formula_t::SHARK_FIN:
          return shark_fin32(re32, im32, max_iter);
        case formula_t::POWER_DRILL:
          return power_drill32(re32, im32, max_iter);
        case formula_t::CROWN:
          return crown32(re32, im32, max_iter);
        case formula_t::SUPER:
          return super32(re32, im32, max_iter);
        case formula_t::CUBIC_MANDELBROT:
          return cubic_mandelbrot_32(re32, im32, max_iter);
        case formula_t::CUBIC_01344:
          return cubic_01344_32(re32, im32, max_iter);
        case formula_t::CUBIC_01417:
          return cubic_01417_32(re32, im32, max_iter);
        case formula_t::CUBIC_01479:
          return cubic_01479_32(re32, im32, max_iter);
        case formula_t::CUBIC_01856:
          return cubic_01856_32(re32, im32, max_iter);
        case formula_t::CUBIC_09601:
          return cubic_09601_32(re32, im32, max_iter);
        case formula_t::CUBIC_09743:
          return cubic_09743_32(re32, im32, max_iter);
        case formula_t::FEATHER:
          return feather32(re32, im32, max_iter);
        default:  // formula_t::MANDELBROT:
          return mandelbrot32(re32, im32, max_iter);
      }
    } else {
      switch (scene.formula) {
        case formula_t::BURNING_SHIP:
          return burning_ship64(re64, im64, max_iter);
        case formula_t::CELTIC:
          return celtic64(re64, im64, max_iter);
        case formula_t::BUFFALO:
          return buffalo64(re64, im64, max_iter);
        case formula_t::PERP_BURNING_SHIP:
          return perp_burning_ship64(re64, im64, max_iter);
        case formula_t::AIRSHIP:
          return airship64(re64, im64, max_iter);
        case formula_t::SHARK_FIN:
          return shark_fin64(re64, im64, max_iter);
        case formula_t::POWER_DRILL:
          return power_drill64(re64, im64, max_iter);
        case formula_t::CROWN:
          return crown64(re64, im64, max_iter);
        case formula_t::SUPER:
          return super64(re64, im64, max_iter);
        case formula_t::CUBIC_MANDELBROT:
          return cubic_mandelbrot_64(re64, im64, max_iter);
        case formula_t::CUBIC_01344:
          return cubic_01344_64(re64, im64, max_iter);
        case formula_t::CUBIC_01417:
          return cubic_01417_64(re64, im64, max_iter);
        case formula_t::CUBIC_01479:
          return cubic_01479_64(re64, im64, max_iter);
        case formula_t::CUBIC_01856:
          return cubic_01856_64(re64, im64, max_iter);
        case formula_t::CUBIC_09601:
          return cubic_09601_64(re64, im64, max_iter);
        case formula_t::CUBIC_09743:
          return cubic_09743_64(re64, im64, max_iter);
        case formula_t::FEATHER:  // not implemented yet
          return max_iter;
        default:  // formula_t::MANDELBROT:
          return mandelbrot64(re64, im64, max_iter);
      }
    }
  }

  static const char *get_name(formula_t f) {
    switch (f) {
      case formula_t::MANDELBROT:
        return "Mandelbrot";
      case formula_t::BURNING_SHIP:
        return "Burning Ship";
      case formula_t::CELTIC:
        return "Celtic";
      case formula_t::BUFFALO:
        return "Buffalo";
      case formula_t::PERP_BURNING_SHIP:
        return "Perp. Burning Ship";
      case formula_t::AIRSHIP:
        return "Airship";
      case formula_t::SHARK_FIN:
        return "Shark Fin";
      case formula_t::POWER_DRILL:
        return "Power Drill";
      case formula_t::CROWN:
        return "Crown";
      case formula_t::SUPER:
        return "Super";
      case formula_t::CUBIC_MANDELBROT:
        return "Cubic Mandelbrot";
      case formula_t::CUBIC_01344:
        return "Cubic #01344";
      case formula_t::CUBIC_01417:
        return "Cubic #01417";
      case formula_t::CUBIC_01479:
        return "Cubic #01479";
      case formula_t::CUBIC_01856:
        return "Cubic #01856";
      case formula_t::CUBIC_09601:
        return "Cubic #09601";
      case formula_t::CUBIC_09743:
        return "Cubic #09743";
      case formula_t::FEATHER:
        return "Feather";
      default:
        return "(Unknown)";
    }
  }

 private:
  static iter_t mandelbrot64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      y = x * y * 2 + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t mandelbrot32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      y = x * y * 2 + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t burning_ship64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      y = (x * y * 2).abs() + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t burning_ship32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      y = (x * y * 2).abs() + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t celtic64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      y = x * y * 2 + b;
      x = (xx - yy).abs() + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t celtic32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      y = x * y * 2 + b;
      x = (xx - yy).abs() + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t buffalo64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      y = (x * y).abs() * -2 + b;
      x = (xx - yy).abs() + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t buffalo32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      y = (x * y).abs() * -2 + b;
      x = (xx - yy).abs() + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t perp_burning_ship64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      y = x * y.abs() * 2 + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t perp_burning_ship32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      y = x * y.abs() * 2 + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t airship64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (y >= 0) {
        y = x * y * 2 + b;
        x = xx - yy + a;
      } else {
        y = x * y * -2 + b;
        x = xx + yy + a;
      }
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t airship32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (y >= 0) {
        y = x * y * 2 + b;
        x = xx - yy + a;
      } else {
        y = x * y * -2 + b;
        x = xx + yy + a;
      }
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t shark_fin64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (y < 0) yy = -yy;
      y = x * y * 2 + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t shark_fin32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (y < 0) yy = -yy;
      y = x * y * 2 + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t power_drill64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (y < 0) yy = -yy;
      y = x * y * -2 + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t power_drill32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (y < 0) yy = -yy;
      y = x * y * -2 + b;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t super64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (y < 0) {
        y = -y;
        yy = -yy;
      }
      y = x * y * 2 + b;
      if (x >= 0) xx = -xx;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t super32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (y < 0) {
        y = -y;
        yy = -yy;
      }
      y = x * y * 2 + b;
      if (x >= 0) xx = -xx;
      x = xx - yy + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t crown64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (x < 0) {
        x = -x;
        xx = -xx;
      }
      y = x * y * -2 + b;
      x = (xx - yy).abs() + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t crown32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (x < 0) {
        x = -x;
        xx = -xx;
      }
      y = x * y * -2 + b;
      x = (xx - yy).abs() + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_mandelbrot_64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      fixed64_t xxx = xx * x;
      fixed64_t xxy = xx * y;
      fixed64_t xyy = yy * x;
      fixed64_t yyy = yy * y;
      y = xxy * 3 - yyy + b;
      x = xxx - xyy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_mandelbrot_32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      fixed32_t xxx = xx * x;
      fixed32_t xxy = xx * y;
      fixed32_t xyy = yy * x;
      fixed32_t yyy = yy * y;
      y = xxy * 3 - yyy + b;
      x = xxx - xyy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_01344_64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (x < 0) xx = -xx;
      fixed64_t xxx = xx * x;
      fixed64_t xxy = xx * y;
      fixed64_t xyy = yy * x.abs();
      fixed64_t yyy = yy * y;
      y = xxy * 3 - yyy + b;
      x = xxx - xyy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_01344_32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (x < 0) xx = -xx;
      fixed32_t xxx = xx * x;
      fixed32_t xxy = xx * y;
      fixed32_t xyy = yy * x.abs();
      fixed32_t yyy = yy * y;
      y = xxy * 3 - yyy + b;
      x = xxx - xyy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_01417_64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      fixed64_t xy = x.abs() * y;
      fixed64_t xxx = xx * x;
      fixed64_t xxy = xy * x;
      fixed64_t xyy = xy * y.abs();
      fixed64_t yyy = yy * y;
      y = xxy * 3 + yyy + b;
      x = -xxx - xyy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_01417_32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      fixed32_t xy = x.abs() * y;
      fixed32_t xxx = xx * x;
      fixed32_t xxy = xy * x;
      fixed32_t xyy = xy * y.abs();
      fixed32_t yyy = yy * y;
      y = xxy * 3 + yyy + b;
      x = -xxx - xyy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_01479_64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (x < 0) xx = -xx;
      fixed64_t xxx = xx * x;
      fixed64_t xxy = xx * y;
      fixed64_t xyy = x.abs() * y.abs() * y;
      fixed64_t yyy = yy * y;
      y = xxy * -3 - yyy + b;
      x = -xxx + xyy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_01479_32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (x < 0) xx = -xx;
      fixed32_t xxx = xx * x;
      fixed32_t xxy = xx * y;
      fixed32_t xyy = x.abs() * y.abs() * y;
      fixed32_t yyy = yy * y;
      y = xxy * -3 - yyy + b;
      x = -xxx + xyy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_01856_64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      // temp = ((abs(zr) * zr * zr) - (3 * abs(zr) * zi * zi)) + cr;
      // zi = ((3 * abs(zr) * zr * abs(zi)) - (zi * zi * zi)) + ci;
      // zr = temp;
      if (x < 0) xx = -xx;
      fixed64_t xxx = xx * x;
      fixed64_t xxy = xx * y.abs();
      fixed64_t xyy = yy * x.abs();
      fixed64_t yyy = yy * y;
      y = xxy * 3 - yyy + b;
      x = xxx - xyy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_01856_32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      if (x < 0) xx = -xx;
      fixed32_t xxx = xx * x;
      fixed32_t xxy = xx * y.abs();
      fixed32_t xyy = yy * x.abs();
      fixed32_t yyy = yy * y;
      y = xxy * 3 - yyy + b;
      x = xxx - xyy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_09601_64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      fixed64_t xxx = x * xx;
      fixed64_t yyy = y * yy;
      fixed64_t xy = x.abs() * y;
      fixed64_t y_abs = y.abs();
      y = ((x * xy * 3) - yyy).abs() + b;
      x = -xxx - xy * y_abs * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_09601_32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      fixed32_t xxx = x * xx;
      fixed32_t yyy = y * yy;
      fixed32_t xy = x.abs() * y;
      fixed32_t y_abs = y.abs();
      y = ((x * xy * 3) - yyy).abs() + b;
      x = -xxx - xy * y_abs * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_09743_64(fixed64_t a, fixed64_t b, iter_t max_iter) {
    fixed64_t x = 0;
    fixed64_t y = 0;
    fixed64_t xx = 0;
    fixed64_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      fixed64_t xxx = x * xx;
      fixed64_t yyy = y * yy;
      if (x < 0) xx = -xx;
      y = ((xx * y.abs() * -3) + yyy).abs() + b;
      x = -xxx + x * yy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t cubic_09743_32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      fixed32_t xxx = x * xx;
      fixed32_t yyy = y * yy;
      if (x < 0) xx = -xx;
      y = ((xx * y.abs() * -3) + yyy).abs() + b;
      x = -xxx + x * yy * 3 + a;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }

  static iter_t feather32(fixed32_t a, fixed32_t b, iter_t max_iter) {
    fixed32_t x = 0;
    fixed32_t y = 0;
    fixed32_t xx = 0;
    fixed32_t yy = 0;
    iter_t iter = 0;
    while (++iter < max_iter && (xx + yy).int_part() < 4) {
      fixed32_t xxx = xx * x;
      fixed32_t yyy = yy * y;
      fixed32_t xyy = x * yy;
      fixed32_t yxx = y * xx;
      fixed32_t p = xxx - xyy * 3;
      fixed32_t q = yxx * 3 - yyy;
      fixed32_t r = xx + 1;
      fixed32_t s = yy;
      fixed32_t dsor = (r.square() + s.square()).inverse();
      x = (p * r + q * s) * dsor + a;
      y = (q * r - p * s) * dsor + b;
      xx = x.square();
      yy = y.square();
    }
    return iter;
  }
};

}  // namespace fixbrot

#endif
