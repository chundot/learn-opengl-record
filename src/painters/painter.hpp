#ifndef PAINTER_HPP
#define PAINTER_HPP
#include "../utils/misc.hpp"
class Painter {
 public:
  static Painter *cur;
  virtual void init() = 0;
  virtual void terminate() = 0;
  virtual void onRender() = 0;
  virtual void onImGuiRender() = 0;
};
#endif