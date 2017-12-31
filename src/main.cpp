#include <iostream>
#include "metaball.hpp"
#include "window.hpp"
#include "vulkan_instance.hpp"
#include "gpu_manager.hpp"
#include "surface.hpp"

using namespace MTB;

const char *PROGRAM_NAME = "Metaballs";
const char METABALL_COUNT = 4;

int main(){
  std::cout << "Starting program: " << PROGRAM_NAME << std::endl;
  Metaball metaballs[METABALL_COUNT];
  Window window = Window(PROGRAM_NAME, 600, 400);
  Vulkan_Instance vk_instance = Vulkan_Instance();
  GPU_Manager gpu_manager(vk_instance);
  Surface surface(window, &vk_instance, &gpu_manager);
  Swapchain swapchain(&gpu_manager, surface.get_surface());

  while(!window.is_closed()){
    // Update metaballs
    // Draw metaballs
  }
  std::cout << "Ending program: " << PROGRAM_NAME << std::endl;
}
