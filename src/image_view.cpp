#include "image_view.hpp"
#include <iostream>

using namespace MTB;
Image_View::Image_View(GPU_Manager *p_gpu_manager, Texture &texture,
		       VkImageAspectFlags aspect_flags):
  m_p_gpu_manager(p_gpu_manager),
  m_vk_image_view(VK_NULL_HANDLE){
  VkImageViewCreateInfo view_create_info = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .image = texture.get_image(),
    .viewType = VK_IMAGE_VIEW_TYPE_2D,
    .format = texture.get_format(),
    .components = {
      .r = VK_COMPONENT_SWIZZLE_IDENTITY,
      .g = VK_COMPONENT_SWIZZLE_IDENTITY,
      .b = VK_COMPONENT_SWIZZLE_IDENTITY,
      .a = VK_COMPONENT_SWIZZLE_IDENTITY
    },
    .subresourceRange = {
      .aspectMask = aspect_flags,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1
    }
  };
  if(vkCreateImageView(this->m_p_gpu_manager->get_logical_device(),
		       &view_create_info, NULL,
		       &this->m_vk_image_view) != VK_SUCCESS){
    std::cout << "failed to create image view" << std::endl;;
  }
}
Image_View::~Image_View(){
  vkDestroyImageView(this->m_p_gpu_manager->get_logical_device(),
		     this->m_vk_image_view, NULL);
}
