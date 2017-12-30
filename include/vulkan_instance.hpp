#ifndef MTB_VULKAN_INSTANCE_H
#define MTB_VULKAN_INSTANCE_H

#include <vector>
#include <string>
#include <vulkan/vulkan.h>
#include "gpu_manager.hpp"

namespace MTB{
  class Vulkan_Instance{
  private:
    VkInstance m_instance;
  public:
    Vulkan_Instance(){};
    Vulkan_Instance(std::vector<std::string>, std::vector<std::string>);
    ~Vulkan_Instance();
    VkInstance get_instance(){return this->m_instance;};
  };
};

#endif /* MTB_VULKAN_INSTANCE_H */
