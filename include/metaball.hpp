#ifndef MTB_METABALL_H
#define MTB_METABALL_H

#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace MTB {
  class Metaball {
  private:
    unsigned int id;
  public:
    Metaball();
    ~Metaball();
  };
};

#endif /* MTB_METABALL_H */
