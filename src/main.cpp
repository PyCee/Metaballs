#include <iostream>
#include "metaball.hpp"
#include "window.hpp"
#include "vulkan_instance.hpp"

using namespace MTB;

const char *PROGRAM_NAME = "Metaballs";
const char METABALL_COUNT = 4;

int main(){
  std::cout << "Starting program: " << PROGRAM_NAME << std::endl;
  Metaball metaballs[METABALL_COUNT];
  Window window = Window("title", 600, 400);
  Vulkan_Instance vk_instance;

  while(!window.is_closed()){
    // Update metaballs
    // Draw metaballs
  }
  std::cout << "Ending program: " << PROGRAM_NAME << std::endl;
}
