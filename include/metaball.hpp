#ifndef MTB_METABALL_H
#define MTB_METABALL_H

#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace MTB {
  struct Metaball_Data {
    glm::vec2 position;
    float radius;
    glm::vec3 color;
  };
  class Metaball {
  private:
    unsigned int id;
    Metaball_Data data;
  public:
    Metaball();
    ~Metaball();
  };
};

#endif /* MTB_METABALL_H */
