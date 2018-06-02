#include "memory_map.hpp"

#include <iostream>
#include <cstring>

using namespace MTB;

Memory_Map::Memory_Map(GPU_Manager *p_gpu_manager, size_t size,
		       VkBufferUsageFlags usage_flags,
		       VkMemoryPropertyFlags property_flags):
  m_p_gpu_manager(p_gpu_manager),
  m_buffer(VK_NULL_HANDLE),
  m_device_memory(VK_NULL_HANDLE),
  m_max_size(size){
  if(property_flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT){
    property_flags ^= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    std::cout << "error::using memory map property \
VK_MEMORY_PROPERTY_HOST_COHERENT_BIT" << std::endl;
  }
  // vkFlushMappedMemory and vkInvalidateMappedMemoryRanges are not needed if the
  // map is created with the memory property VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  // but it is assumed disabled for minor preformance reasons. Attempting to
  // create a map with it currently only removes the bit and triggers a warning,
  // but that may change in the future
  // aka: don't use VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  
  VkBufferCreateInfo buffer_create_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .size = this->m_max_size,
    .usage = usage_flags,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 0,
    .pQueueFamilyIndices = NULL
  };
  if(vkCreateBuffer(this->m_p_gpu_manager->get_logical_device(),
		    &buffer_create_info, NULL, &this->m_buffer) != VK_SUCCESS){
    std::cout << "failed to create memory map buffer" << std::endl;
  }
  VkMemoryRequirements buffer_memory_req;
  vkGetBufferMemoryRequirements(this->m_p_gpu_manager->get_logical_device(),
				this->m_buffer, &buffer_memory_req);
  
  VkMemoryAllocateInfo memory_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    .pNext = NULL,
    .allocationSize = buffer_memory_req.size,
    .memoryTypeIndex =
    this->m_p_gpu_manager->get_memory_type_index(buffer_memory_req,
						 property_flags)
  };
  if(vkAllocateMemory(this->m_p_gpu_manager->get_logical_device(),
		      &memory_allocate_info,
		      NULL, &this->m_device_memory) != VK_SUCCESS){
    std::cout << "failed to allocate memory map" << std::endl;
  }
  
  if(vkBindBufferMemory(this->m_p_gpu_manager->get_logical_device(),
		        this->m_buffer, this->m_device_memory, 0) != VK_SUCCESS){
    std::cout << "failed to bind buffer memory" << std::endl;
  }
}
Memory_Map::~Memory_Map(){
    vkDestroyBuffer(this->m_p_gpu_manager->get_logical_device(), this->m_buffer, NULL);
    vkFreeMemory(this->m_p_gpu_manager->get_logical_device(), this->m_device_memory,
		 NULL);
}
void Memory_Map::flush(void *p_data, size_t data_size){
  void *p_map = NULL;

  if(data_size > this->m_max_size){
    // If the data size is greater than our alotted device memory
    std::cout << "error::attempting to flush memory_map, when data_size (" <<
      data_size << ") > this->m_max_size (" << this->m_max_size << ")" << std::endl;
  }

  // Map gpu memory to somewhere we can access
  if(vkMapMemory(this->m_p_gpu_manager->get_logical_device(), this->m_device_memory,
		 0, this->m_max_size, 0, &p_map) != VK_SUCCESS){
    std::cout << "failed to map gpu memory" << std::endl;
  }

  // Copy data to mapped area
  std::memcpy(p_map, p_data, data_size);
  
  VkMappedMemoryRange memory_range = {
    .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
    .pNext = NULL,
    .memory = this->m_device_memory,
    .offset = 0,
    .size = VK_WHOLE_SIZE
  };
  // Makes sure the memory is mapped to the gpu
  vkFlushMappedMemoryRanges(this->m_p_gpu_manager->get_logical_device(),
			    1, &memory_range);
  
  // Un-map the accessible memory
  vkUnmapMemory(this->m_p_gpu_manager->get_logical_device(), this->m_device_memory);
}
