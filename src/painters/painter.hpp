#ifndef PAINTER_HPP
#define PAINTER_HPP
#include "../utils/misc.hpp"
class Painter {
 public:
  virtual ~Painter() = default;
  static Painter *cur;
  virtual void init() = 0;
  virtual void terminate() = 0;
  virtual void onRender() = 0;
  virtual void onImGuiRender() = 0;
  virtual void setFrameSize(int width, int height) = 0;
};
#endif