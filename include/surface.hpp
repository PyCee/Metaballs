#ifndef MTB_SURFACE_H
#define MTB_SURFACE_H

#include "window.hpp"
#include "vulkan_instance.hpp"
#include "gpu_manager.hpp"
#include "texture.hpp"

namespace MTB{
  class Surface{
  private:
    VkSurfaceKHR m_surface;
    VkCommandBuffer m_command_buffer;
    Vulkan_Instance *p_vulkan_instance;
    GPU_Manager *p_gpu_manager;
  public:
    Surface(){};
    Surface(Window &, Vulkan_Instance *, GPU_Manager *);
    ~Surface();
    VkSurfaceKHR get_surface(void){return this->m_surface;};
  };
};

#endif /* MTB_SURFACE_H */
