#ifndef MTB_SWAPCHAIN_H
#define MTB_SWAPCHAIN_H

#include <vector>
#include "vulkan_instance.hpp"
#include "gpu_manager.hpp"

namespace MTB{
  class Swapchain{
    VkSwapchainKHR m_swapchain;
    std::vector<VkImage> m_images;
    GPU_Manager *p_gpu_manager;
  private:
  public:
    Swapchain(){};
    Swapchain(GPU_Manager *, VkSurfaceKHR);
    ~Swapchain();
  };
};

#endif /* MTB_SWAPCHAIN_H */
