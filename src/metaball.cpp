#include "metaball.hpp"
using namespace MTB;
Metaball::Metaball(){
  static unsigned int metaball_id = 0;
  this->id = metaball_id++;
  std::cout << "Creating Metaball w/ id: " << this->id << std::endl;
}
Metaball::~Metaball(){
  std::cout << "Deleting Metaball w/ id: " << this->id << std::endl;
}
