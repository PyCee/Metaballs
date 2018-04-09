#ifndef MTB_GPU_MANAGER_H
#define MTB_GPU_MANAGER_H

#include "vulkan_instance.hpp"

namespace MTB{
  class GPU_Manager{
    VkPhysicalDevice *m_physical_devices;
    VkDevice m_logical_device;
    VkCommandPool m_command_pool;
    unsigned int m_queue_family_index;
    VkPhysicalDeviceMemoryProperties m_memory_properties;
    VkQueue m_queue;
    std::vector<VkSemaphore> wait_sem,
      sig_sem;
    std::vector<VkPipelineStageFlags> wait_dst_mask;
    std::vector<VkCommandBuffer> command_buffers;
  public:
    GPU_Manager(){};
    GPU_Manager(Vulkan_Instance &);
    ~GPU_Manager();
    void allocate_command_buffers(VkCommandBuffer *, unsigned int);
    void submit(void);
    
    VkPhysicalDevice get_physical_device(void){return this->m_physical_devices[0];};
    VkDevice get_logical_device(void){return this->m_logical_device;};
    unsigned int get_queue_family_index(void){return this->m_queue_family_index;};
    VkQueue get_queue(void){return this->m_queue;};
    VkPhysicalDeviceMemoryProperties get_memory_properties(void){
      return this->m_memory_properties;};
    void add_wait_semaphore(VkSemaphore wait,
			    VkPipelineStageFlags flags=VK_PIPELINE_STAGE_TRANSFER_BIT){
      this->wait_sem.push_back(wait);
      this->wait_dst_mask.push_back(flags);
    };
    void add_signal_semaphore(VkSemaphore sig){this->sig_sem.push_back(sig);};
    void queue_up_command(VkCommandBuffer command_buffer){
      this->command_buffers.push_back(command_buffer);
    };
  };
};

#endif /* MTB_GPU_MANAGER_H */
