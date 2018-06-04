#ifndef MTB_METABALL_H
#define MTB_METABALL_H

#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace MTB {
  struct Metaball_Data {
    glm::vec4 color_and_radius;
    glm::vec2 position;
  };
  class Metaball {
  private:
    unsigned int id;
    Metaball_Data data;
  public:
    Metaball();
    ~Metaball();
    unsigned int get_id(void){return this->id;};
    void set_data(Metaball_Data d){this->data = d;};
    Metaball_Data get_data(void){return this->data;};
  };
};

#endif /* MTB_METABALL_H */
