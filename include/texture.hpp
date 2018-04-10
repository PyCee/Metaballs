#ifndef MTB_TEXTURE_H
#define MTB_TEXTURE_H

#include "gpu_manager.hpp"

namespace MTB{
  class Texture{
  private:
    VkImage m_image;
    GPU_Manager *p_gpu_manager;
    VkDeviceMemory m_memory;
    unsigned int m_width, m_height;
    VkFormat m_format;
    VkImageAspectFlags m_aspect_flags;
    uint32_t image_size;
  public:
    Texture(){};
    Texture(GPU_Manager *, unsigned int, unsigned int, VkFormat,
	    VkImageUsageFlags, VkMemoryPropertyFlags, VkImageAspectFlags);
    ~Texture();
    VkFormat get_format(void){return this->m_format;};
    VkImage get_image(void){return this->m_image;};
  };
};

#endif /* MTB_TEXTURE_H */
