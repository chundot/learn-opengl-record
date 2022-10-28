#ifndef IMODEL_HPP
#define IMODEL_HPP
#include "shader.hpp"
class IModel {
 public:
  virtual void Draw(Shader &shader) = 0;
};
#endif