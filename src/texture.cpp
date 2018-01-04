#include "texture.hpp"
#include <iostream>

using namespace MTB;
Texture::Texture(GPU_Manager *gpu_manager, unsigned int width, unsigned int height,
		 VkFormat format, VkImageUsageFlags usage_flags,
		 VkMemoryPropertyFlags memory_property_flags,
		 VkImageAspectFlags aspect_flags):
  p_gpu_manager(gpu_manager), m_width(width), m_height(height), m_format(format),
  m_aspect_flags(aspect_flags){
  unsigned int i;
  VkImageCreateInfo image_create_info = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .imageType = VK_IMAGE_TYPE_2D,
    .format = format,
    .extent = {
      .width = width,
      .height = height,
      .depth = 1
    },
    .mipLevels = 1,
    .arrayLayers = 1,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .tiling = VK_IMAGE_TILING_OPTIMAL,
    .usage = usage_flags,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 0,
    .pQueueFamilyIndices = NULL,
    .initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED
  };
  if(vkCreateImage(gpu_manager->get_logical_device(), &image_create_info, NULL,
		   &this->m_image) != VK_SUCCESS){
    std::cout << "ERROR::Failed to create texture image" << std::endl;
  }
  VkMemoryRequirements image_memory_req;
  vkGetImageMemoryRequirements(gpu_manager->get_logical_device(), this->m_image,
			       &image_memory_req);

  unsigned int memory_type_index = 0;
  for(i = 0; i < gpu_manager->get_memory_properties().memoryTypeCount; ++i){
    if((image_memory_req.memoryTypeBits & (uint32_t)(1 << i)) &&
       ((gpu_manager->get_memory_properties().memoryTypes[i].propertyFlags &
	 memory_property_flags) == memory_property_flags)){
      // If viable memory type has been found
      memory_type_index = i;
      break;
    }
  }
  
  VkMemoryAllocateInfo memory_alloc_info = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    .pNext = NULL,
    .allocationSize = image_memory_req.size,
    .memoryTypeIndex = memory_type_index
  };
  if(vkAllocateMemory(gpu_manager->get_logical_device(), &memory_alloc_info, NULL,
		      &this->m_memory) != VK_SUCCESS){
    std::cout << "ERROR::Failed to allocate texture memory" << std::endl;
  }
  vkBindImageMemory(gpu_manager->get_logical_device(), this->m_image,
		    this->m_memory, 0);
}
Texture::~Texture(){
  vkFreeMemory(this->p_gpu_manager->get_logical_device(), this->m_memory, NULL);
  vkDestroyImage(this->p_gpu_manager->get_logical_device(), this->m_image, NULL);
}
