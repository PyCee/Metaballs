#ifndef MTB_GPU_MANAGER_H
#define MTB_GPU_MANAGER_H

#include <vulkan/vulkan.h>

namespace MTB{
  class GPU_Manager{
    VkDevice m_logical_device;
    VkCommandPool m_command_pool;
    VkQueue m_queue;
  public:
    GPU_Manager(){};
    GPU_Manager(VkInstance);
    ~GPU_Manager();
  };
};

#endif /* MTB_GPU_MANAGER_H */
