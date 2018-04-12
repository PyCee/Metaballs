#ifndef MTB_MEMORY_MAP
#define MTB_MEMORY_MAP

#include "vulkan_instance.hpp"
#include "gpu_manager.hpp"

namespace MTB{
  class Memory_Map{
  private:
    GPU_Manager *m_p_gpu_manager;
    VkBuffer m_buffer;
    VkDeviceMemory m_device_memory;
    size_t m_max_size;
  public:
    Memory_Map(void){};
    Memory_Map(GPU_Manager *, size_t, VkBufferUsageFlags, VkMemoryPropertyFlags);
    ~Memory_Map();
    void flush(void *, size_t);
    VkBuffer get_buffer(void){return this->m_buffer;};
    VkDeviceMemory get_device_memory(void){return this->m_device_memory;};
  };
  class Uniform_Buffer : public Memory_Map {
  private:
  public:
    Uniform_Buffer(GPU_Manager *p_gpu_manager, size_t max_size):
      Memory_Map(p_gpu_manager, max_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT){};
  };
}

#endif /* MTB_MEMORY_MAP */
