#ifndef MTB_VULKAN_INSTANCE_H
#define MTB_VULKAN_INSTANCE_H

//#include vulkan
#include <vulkan/vulkan.h>

namespace MTB{
  class Vulkan_Instance{
  private:
    VkInstance m_instance;
  public:
    Vulkan_Instance();
    ~Vulkan_Instance();
  };
};

#endif /* MTB_VULKAN_INSTANCE_H */
