#include "swapchain.hpp"
#include <iostream>

using namespace MTB;
Swapchain::Swapchain(GPU_Manager *gpu_manager, VkSurfaceKHR surface):
  p_gpu_manager(gpu_manager){
  unsigned int i;
  VkSurfaceCapabilitiesKHR capabilities;
  if(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu_manager->get_physical_device(),
					       surface, &capabilities) != VK_SUCCESS){
    std::cout << "ERROR::Failed to get physical device surface capabilities" <<
      std::endl;
  }
  unsigned int format_count;
  if(vkGetPhysicalDeviceSurfaceFormatsKHR(gpu_manager->get_physical_device(),
					  surface, &format_count,
					  NULL) != VK_SUCCESS){
    std::cout << "ERROR::Failed to get surface format count" << std::endl;
  }
  VkSurfaceFormatKHR available_formats[format_count];
  if(vkGetPhysicalDeviceSurfaceFormatsKHR(gpu_manager->get_physical_device(),
					  surface, &format_count,
					  available_formats) != VK_SUCCESS){
    std::cout << "ERROR::Failed to get surface formats" << std::endl;
  }
  VkSurfaceFormatKHR swapchain_format = available_formats[0];
  if((1 == format_count) && (VK_FORMAT_UNDEFINED == available_formats[0].format)){
    swapchain_format = (VkSurfaceFormatKHR)
      {VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR};
  } else{
    for(i = 0; i < format_count; ++i){
      if(VK_FORMAT_R8G8B8A8_UNORM == available_formats[i].format &&
	 VK_COLORSPACE_SRGB_NONLINEAR_KHR == available_formats[i].colorSpace){
	swapchain_format = available_formats[i];
	break;
      }
    }
  }
  
  VkSwapchainCreateInfoKHR swapchain_create_info = {
    .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .pNext = NULL,
    .flags = 0,
    .surface = surface,
    .minImageCount = 3,
    .imageFormat = swapchain_format.format,
    .imageColorSpace = swapchain_format.colorSpace,
    .imageExtent = capabilities.currentExtent,
    .imageArrayLayers = 1,
    .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 0,
    .pQueueFamilyIndices = NULL,
    .preTransform = capabilities.currentTransform,
    .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    .presentMode = VK_PRESENT_MODE_FIFO_KHR,
    .clipped = VK_TRUE,
    .oldSwapchain = VK_NULL_HANDLE
  };
  if(vkCreateSwapchainKHR(this->p_gpu_manager->get_logical_device(),
			  &swapchain_create_info,
			  NULL, &this->m_swapchain) != VK_SUCCESS){
    std::cout << "ERROR::Failed to create swapchain" << std::endl;
  } else{
    std::cout << "Created swapchain" << std::endl;
  }
}
Swapchain::~Swapchain(){
  vkDeviceWaitIdle(this->p_gpu_manager->get_logical_device());
  vkDestroySwapchainKHR(this->p_gpu_manager->get_logical_device(),
			this->m_swapchain, NULL);
}
