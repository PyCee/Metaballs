#ifndef MTB_SWAPCHAIN_H
#define MTB_SWAPCHAIN_H

#include <vector>
#include "vulkan_instance.hpp"
#include "gpu_manager.hpp"
#include "surface.hpp"

namespace MTB{
  class Swapchain{
    VkSwapchainKHR m_swapchain;
    VkImage *m_images;
    VkSurfaceFormatKHR m_surface_format;
    VkExtent2D m_extent;
    VkPresentModeKHR m_present_mode;
    unsigned int m_image_count;
    Surface *p_surface;
    VkSemaphore m_acquired_image,
      m_ready_to_present;
    GPU_Manager *p_gpu_manager;
    unsigned int m_image_index;
  private:
  public:
    Swapchain(){};
    Swapchain(GPU_Manager *, Surface *);
    ~Swapchain();
    void present_image(void);
    void set_new_image(void);
    VkSurfaceFormatKHR get_surface_format(void){return this->m_surface_format;};
    VkExtent2D get_extent(void){return this->m_extent;};
    VkImage get_image(unsigned int index){return this->m_images[index];};
    VkImage get_current_image(void){return this->get_image(this->m_image_index);};
    unsigned int get_image_count(void){return this->m_image_count;};
    unsigned int get_image_index(void){return this->m_image_index;};
    VkSemaphore get_semaphore_acquired_image(){return this->m_acquired_image;};
    VkSemaphore get_semaphore_ready_to_present(){return this->m_ready_to_present;};
    unsigned int get_width(void){return this->m_extent.width;};
    unsigned int get_height(void){return this->m_extent.height;};
  };
};

#endif /* MTB_SWAPCHAIN_H */
