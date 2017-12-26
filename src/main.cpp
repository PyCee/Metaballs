#include <iostream>
#include "metaball.hpp"
#include "vector.hpp"

using namespace MTB;

const char *PROGRAM_NAME = "Metaballs";
int main(){
  std::cout << "Starting program: " << PROGRAM_NAME << std::endl;
  
  Metaball mb();
  Vector v(2.4, 0.0);
  Vector a(1.2, 4.5);
  a = a + v;
  std::cout << a.X << std::endl;
  
  std::cout << "Ending program: " << PROGRAM_NAME << std::endl;
}
