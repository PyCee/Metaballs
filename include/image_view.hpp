#ifndef MTB_IMAGE_VIEW
#define MTB_IMAGE_VIEW

#include "vulkan_instance.hpp"
#include "gpu_manager.hpp"
#include "texture.hpp"

namespace MTB{
  class Image_View{
  private:
    GPU_Manager *m_p_gpu_manager;
    VkImageView m_vk_image_view;
  public:
    Image_View(void){};
    Image_View(GPU_Manager *, Texture &, VkImageAspectFlags);
    ~Image_View();
    VkImageView get_view(void){return this->m_vk_image_view;}
  };
}

#endif /* MTB_IMAGE_VIEW */
