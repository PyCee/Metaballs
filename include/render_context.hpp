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
    Surface m_surface;
    Swapchain m_swapchain;
    Texture m_render_target;
    GPU_Manager *p_gpu_manager;
    VkCommandBuffer *m_render_target_copy_buffers;
    VkCommandBuffer m_clear_render_target_buffer;
    VkCommandBuffer get_current_render_target_copy_buffer(void){
      return this->m_render_target_copy_buffers[this->m_swapchain.get_image_index()];
    };
  public:
    VkSemaphore m_target_rendered;
    Render_Context(){};
    Render_Context(Window &, Vulkan_Instance *, GPU_Manager *);
    ~Render_Context();
    VkSemaphore get_semaphore_acquired_image(void){
      return this->m_swapchain.get_semaphore_acquired_image();
    }
    void present(void);
    void queue_up_target_clear(void);
  };
};

#endif /* MTB_RENDER_CONTEXT */
