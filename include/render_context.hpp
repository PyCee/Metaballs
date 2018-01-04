#ifndef MTB_RENDER_CONTEXT_H
#define MTB_RENDER_CONTEXT_H

#include "window.hpp"
#include "vulkan_instance.hpp"
#include "gpu_manager.hpp"
#include "surface.hpp"
#include "swapchain.hpp"
#include "texture.hpp"

namespace MTB{
  class Render_Context{
  private:
    GPU_Manager *p_gpu_manager;
    VkCommandBuffer *m_render_target_copy_buffers;
    VkCommandBuffer get_current_render_target_copy_buffer(void){
      return this->m_render_target_copy_buffers[this->m_swapchain.get_image_index()];
    };
  public:
    Surface m_surface;
    Swapchain m_swapchain;
    Texture m_render_target;
    VkSemaphore m_target_rendered;
    Render_Context(){};
    Render_Context(Window &, Vulkan_Instance *, GPU_Manager *);
    ~Render_Context();
    void present(void);
  };
};

#endif /* MTB_RENDER_CONTEXT */
