#include "swapchain.hpp"
#include <iostream>
#include <limits>

using namespace MTB;
Swapchain::Swapchain(GPU_Manager *gpu_manager, Surface *surface):
  p_surface(surface), p_gpu_manager(gpu_manager){
  unsigned int i;
  VkSurfaceCapabilitiesKHR capabilities;
  if(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu_manager->get_physical_device(),
					       surface->get_surface(),
					       &capabilities) != VK_SUCCESS){
    std::cout << "ERROR::Failed to get physical device surface capabilities" <<
      std::endl;
  }
  this->m_extent = capabilities.currentExtent;
  unsigned int format_count;
  if(vkGetPhysicalDeviceSurfaceFormatsKHR(gpu_manager->get_physical_device(),
					  surface->get_surface(), &format_count,
					  NULL) != VK_SUCCESS){
    std::cout << "ERROR::Failed to get surface format count" << std::endl;
  }
  VkSurfaceFormatKHR available_formats[format_count];
  if(vkGetPhysicalDeviceSurfaceFormatsKHR(gpu_manager->get_physical_device(),
					  surface->get_surface(), &format_count,
					  available_formats) != VK_SUCCESS){
    std::cout << "ERROR::Failed to get surface formats" << std::endl;
  }
  this->m_surface_format = available_formats[0];
  if((1 == format_count) && (VK_FORMAT_UNDEFINED == available_formats[0].format)){
    this->m_surface_format = (VkSurfaceFormatKHR) {
      .format = VK_FORMAT_R8G8B8A8_UNORM,
      .colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR
    };
  } else{
    for(i = 0; i < format_count; ++i){
      if(VK_FORMAT_R8G8B8A8_UNORM == available_formats[i].format &&
	 VK_COLORSPACE_SRGB_NONLINEAR_KHR == available_formats[i].colorSpace){
	this->m_surface_format = available_formats[i];
	break;
      }
    }
  }
  
  unsigned int present_mode_count;
  if((vkGetPhysicalDeviceSurfacePresentModesKHR(gpu_manager->get_physical_device(),
						surface->get_surface(),
						&present_mode_count,
						NULL) != VK_SUCCESS) ||
     (present_mode_count == 0)){
    std::cout << "ERROR::Failed to get present modes" << std::endl;
  }
  VkPresentModeKHR available_present_modes[present_mode_count];
  if(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu_manager->get_physical_device(),
					       surface->get_surface(),
					       &present_mode_count,
					       available_present_modes) !=
     VK_SUCCESS){
    std::cout << "ERROR::Failed to get present modes" << std::endl;
  }
  unsigned int present_mode_index = (unsigned int)-1;
  for(i = 0; i < present_mode_count; ++i){
    // Search for our desired present mode
    if(VK_PRESENT_MODE_MAILBOX_KHR == available_present_modes[i]){
      present_mode_index = i;
      break;
    }
  }
  if((unsigned int)-1 == present_mode_index){
    // If our desired present mode has not been found
    for(i = 0; i < present_mode_count; ++i){
      if(VK_PRESENT_MODE_FIFO_KHR == available_present_modes[i]){
	// Present mode FIFO will always be supported
	present_mode_index = i;
	break;
      }
    }
  }
  this->m_present_mode = available_present_modes[present_mode_index];
  
  VkSwapchainCreateInfoKHR swapchain_create_info = {
    .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .pNext = NULL,
    .flags = 0,
    .surface = surface->get_surface(),
    .minImageCount = 3,
    .imageFormat = this->get_surface_format().format,
    .imageColorSpace = this->get_surface_format().colorSpace,
    .imageExtent = capabilities.currentExtent,
    .imageArrayLayers = 1,
    .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 0,
    .pQueueFamilyIndices = NULL,
    .preTransform = capabilities.currentTransform,
    .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    .presentMode = this->m_present_mode,
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
  if(vkGetSwapchainImagesKHR(gpu_manager->get_logical_device(), this->m_swapchain,
			     &this->m_image_count, NULL) != VK_SUCCESS ||
     this->m_image_count == 0){
    std::cout << "ERROR::Failed to get swapchain image count" << std::endl;
  } else{
    std::cout << "Swapchain image count: " << this->m_image_count << std::endl;
  }
  this->m_images = new VkImage[this->m_image_count];
  
  if(vkGetSwapchainImagesKHR(gpu_manager->get_logical_device(), this->m_swapchain,
			     &this->m_image_count, this->m_images) != VK_SUCCESS){
    std::cout << "ERROR::Failed to get swapchain images" << std::endl;
  }
  VkSemaphoreCreateInfo semaphore_create_info = {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0
  };
  if(vkCreateSemaphore(gpu_manager->get_logical_device(), &semaphore_create_info, NULL,
		       &this->m_acquired_image) != VK_SUCCESS){
    std::cout << "ERROR::Failed to create surface->m_image_available" << std::endl;
  }
  if(vkCreateSemaphore(gpu_manager->get_logical_device(), &semaphore_create_info, NULL,
		       &this->m_ready_to_present) != VK_SUCCESS){
    std::cout << "ERROR::Failed to create surface->m_image_presentable" << std::endl;
  }
  this->set_new_image();
}
Swapchain::~Swapchain(){
  vkDeviceWaitIdle(this->p_gpu_manager->get_logical_device());
  delete[] this->m_images;
  vkDestroySemaphore(this->p_gpu_manager->get_logical_device(),
		     this->m_acquired_image, NULL);
  vkDestroySemaphore(this->p_gpu_manager->get_logical_device(),
		     this->m_ready_to_present, NULL);
  vkDestroySwapchainKHR(this->p_gpu_manager->get_logical_device(),
			this->m_swapchain, NULL);
}

void Swapchain::present_image(void){
  VkSemaphore wait_semaphores[] = {
    this->get_semaphore_ready_to_present()
  };
  VkPresentInfoKHR present_info = {
    .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    .pNext = NULL,
    .waitSemaphoreCount = sizeof(wait_semaphores) / sizeof(*wait_semaphores),
    .pWaitSemaphores = wait_semaphores,
    .swapchainCount = 1,
    .pSwapchains = &this->m_swapchain,
    .pImageIndices = &this->m_image_index,
    .pResults = NULL
  };
  switch(vkQueuePresentKHR(this->p_gpu_manager->get_queue(), &present_info)){
  case VK_SUCCESS:
    break;
  case VK_SUBOPTIMAL_KHR:
  case VK_ERROR_OUT_OF_DATE_KHR:
    // TODO: rebuild swapchain
    std::cout << "TODO: Need to rebuild out-of-date swapchain (present)" << std::endl;
    break;
  default:
    std::cout << "ERROR::Failed to present swapchain image" << std::endl;
    break;
  }
  // Set new swapchain image after presentation
  this->set_new_image();
}
void Swapchain::set_new_image(void){
  
  switch(vkAcquireNextImageKHR(this->p_gpu_manager->get_logical_device(),
			       this->m_swapchain, 
			       std::numeric_limits<uint64_t>::max(),
			       this->get_semaphore_acquired_image(),
			       VK_NULL_HANDLE, &this->m_image_index)){
  case VK_SUCCESS:
    break;
  case VK_TIMEOUT:
    std::cout << "ERROR::Timeout while acquiring image" << std::endl;
    break;
  case VK_SUBOPTIMAL_KHR:
  case VK_ERROR_OUT_OF_DATE_KHR:
    // TODO: rebuild swapchain
    std::cout << "TODO: Need to rebuild out-of-date swapchain (acquire)" << std::endl;
    break;
  case VK_NOT_READY:
    std::cout << "ERROR::Not ready to acquire image" << std::endl;
    break;
  case VK_ERROR_OUT_OF_HOST_MEMORY:
  case VK_ERROR_OUT_OF_DEVICE_MEMORY:
    std::cout << "ERROR::Out of memory when acquiring next image" << std::endl;
    break;
  default:
    std::cout << "ERROR::Failed to acquire next swapchain image" << std::endl;
    break;
  }
}
