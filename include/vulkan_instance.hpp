#ifndef MTB_VULKAN_INSTANCE_H
#define MTB_VULKAN_INSTANCE_H

#include <vector>
#include <string>

#if defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__unix__)
#define VK_USE_PLATFORM_XCB_KHR
#endif

#include <vulkan/vulkan.h>

namespace MTB{
  class Vulkan_Instance{
  private:
    VkInstance m_instance;
  public:
    Vulkan_Instance();
    ~Vulkan_Instance();
    VkInstance get_instance(){return this->m_instance;};
  };
};

#endif /* MTB_VULKAN_INSTANCE_H */
