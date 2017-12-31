#include "metaball.hpp"
using namespace MTB;
Metaball::Metaball(){
  static unsigned int metaball_id = 0;
  this->id = metaball_id++;
}
Metaball::~Metaball(){
}
