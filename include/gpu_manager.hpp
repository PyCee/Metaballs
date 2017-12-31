#ifndef MTB_GPU_MANAGER_H
#define MTB_GPU_MANAGER_H

#include "vulkan_instance.hpp"

namespace MTB{
  class GPU_Manager{
    VkPhysicalDevice *m_physical_devices;
    VkDevice m_logical_device;
    VkCommandPool m_command_pool;
    unsigned int m_queue_family_index;
    VkQueue m_queue;
    VkSubmitInfo submit_info;
  public:
    GPU_Manager(){};
    GPU_Manager(Vulkan_Instance &);
    ~GPU_Manager();
    VkPhysicalDevice get_physical_device(void){return this->m_physical_devices[0];};
    VkDevice get_logical_device(void){return this->m_logical_device;};
    unsigned int get_queue_family_index(void){return this->m_queue_family_index;};
  };
};

#endif /* MTB_GPU_MANAGER_H */
