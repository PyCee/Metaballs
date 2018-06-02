#include <iostream>
#include "metaball.hpp"
#include "window.hpp"
#include "vulkan_instance.hpp"
#include "gpu_manager.hpp"
#include "surface.hpp"
#include "swapchain.hpp"
#include "shader_module.hpp"
#include "render_context.hpp"
#include "image_view.hpp"
#include "memory_map.hpp"
#include "pipeline.hpp"

using namespace MTB;

const char *PROGRAM_NAME = "Metaballs";
const char METABALL_COUNT = 4;

int main(){
  std::cout << "Starting program: " << PROGRAM_NAME << std::endl;
  Metaball metaballs[METABALL_COUNT];
  Window window = Window(PROGRAM_NAME, 600, 400);
  Vulkan_Instance vk_instance = Vulkan_Instance();
  GPU_Manager gpu_manager(vk_instance);
  Render_Context rc(window, &vk_instance, &gpu_manager);
  Pipeline pipe(&gpu_manager, rc);
  
  std::cout << "Pre loop" << std::endl;
  
  while(!window.is_closed()){
  
    gpu_manager.add_wait_semaphore(rc.get_semaphore_acquired_image());
    gpu_manager.add_signal_semaphore(rc.m_target_rendered);
    gpu_manager.queue_up_command(pipe.get_command_buffer());
    
    gpu_manager.submit();

    rc.present();
  }
  
  std::cout << "Post loop" << std::endl;
  
  vkDeviceWaitIdle(gpu_manager.get_logical_device());
  std::cout << "Ending program: " << PROGRAM_NAME << std::endl;
}
