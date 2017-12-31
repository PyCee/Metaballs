#ifndef MTB_SWAPCHAIN_H
#define MTB_SWAPCHAIN_H

#include <vector>
#include "vulkan_instance.hpp"
#include "gpu_manager.hpp"
#include "surface.hpp"

namespace MTB{
  class Swapchain{
    VkSwapchainKHR m_swapchain;
    std::vector<VkImage> m_images;
    Surface *p_surface;
    GPU_Manager *p_gpu_manager;
    unsigned int m_image_index;
  private:
  public:
    Swapchain(){};
    Swapchain(GPU_Manager *, Surface *);
    ~Swapchain();
    void present_image(void);
    void set_next_image(void);
    VkImage get_current_image(void){return this->m_images[this->m_image_index];};
  };
};

#endif /* MTB_SWAPCHAIN_H */
