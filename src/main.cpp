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
const char METABALL_COUNT = 8;

int main(){
  std::cout << "Starting program: " << PROGRAM_NAME << std::endl;
  Window window = Window(PROGRAM_NAME, 600, 600);
  Vulkan_Instance vk_instance = Vulkan_Instance();
  GPU_Manager gpu_manager(vk_instance);
  Render_Context rc(window, &vk_instance, &gpu_manager);
  Pipeline pipe(&gpu_manager, rc);
  
  Metaball metaballs[METABALL_COUNT];
  
  metaballs[0].set_data((Metaball_Data){
      .color_and_radius=glm::vec4(1.0f,0.0f,0.0f, 0.2),
	.position=glm::vec2(0.0, 0.0)
	});
  metaballs[1].set_data((Metaball_Data){
      .color_and_radius=glm::vec4(0.0f,1.0f,0.0f, 0.3),
	.position=glm::vec2(0.7, 0.0)
	});
  metaballs[2].set_data((Metaball_Data){
      .color_and_radius=glm::vec4(0.0f,0.0f,1.0f, 0.2),
	.position=glm::vec2(0.7, -0.4)
	});
  metaballs[3].set_data((Metaball_Data){
      .color_and_radius=glm::vec4(1.0f,0.0f,1.0f, 0.4),
	.position=glm::vec2(-0.55, 0.0)
	});
  metaballs[4].set_data((Metaball_Data){
      .color_and_radius=glm::vec4(1.0f,1.0f,0.0f, 0.1),
	.position=glm::vec2(-0.6, 0.4)
	});

  std::vector<Metaball> data;
  data.push_back(metaballs[0]);
  data.push_back(metaballs[1]);
  data.push_back(metaballs[2]);
  data.push_back(metaballs[3]);
  data.push_back(metaballs[4]);

  pipe.build_command_buffer(data);
  
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
