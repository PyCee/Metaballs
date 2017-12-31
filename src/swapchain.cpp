#include "swapchain.hpp"
#include <iostream>

using namespace MTB;
Swapchain::Swapchain(GPU_Manager *gpu_manager, Surface *surface):
  p_surface(surface), p_gpu_manager(gpu_manager), m_image_index(0){
  unsigned int i;
  VkSurfaceCapabilitiesKHR capabilities;
  if(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu_manager->get_physical_device(),
					       surface->get_surface(),
					       &capabilities) != VK_SUCCESS){
    std::cout << "ERROR::Failed to get physical device surface capabilities" <<
      std::endl;
  }
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
    .surface = surface->get_surface(),
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

  unsigned int image_count;
  if(vkGetSwapchainImagesKHR(gpu_manager->get_logical_device(), this->m_swapchain,
			     &image_count, NULL) != VK_SUCCESS || image_count == 0){
    std::cout << "ERROR::Failed to get swapchain image count" << std::endl;
  }
  
  if(vkGetSwapchainImagesKHR(gpu_manager->get_logical_device(), this->m_swapchain,
			     &image_count, this->m_images.data()) != VK_SUCCESS){
    std::cout << "ERROR::Failed to get swapchain images" << std::endl;
  }
}
Swapchain::~Swapchain(){
  vkDeviceWaitIdle(this->p_gpu_manager->get_logical_device());
  vkDestroySwapchainKHR(this->p_gpu_manager->get_logical_device(),
			this->m_swapchain, NULL);
}

void Swapchain::present_image(void){

  // TODO: copy this->p_surface->render_target to this.get_current_image()
  // submit that command

  VkSemaphore wait_semaphores[] = {
    this->p_surface->get_semaphore_image_presentable()
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
}
void Swapchain::set_next_image(void){
  static unsigned int const acquire_image_timeout = 4000; // (ns)
  switch(vkAcquireNextImageKHR(this->p_gpu_manager->get_logical_device(),
			       this->m_swapchain, acquire_image_timeout,
			       this->p_surface->get_semaphore_image_available(),
			       VK_NULL_HANDLE, &this->m_image_index)){
  case VK_SUCCESS:
    break;
  case VK_SUBOPTIMAL_KHR:
  case VK_ERROR_OUT_OF_DATE_KHR:
    // TODO: rebuild swapchain
    std::cout << "TODO: Need to rebuild out-of-date swapchain (acquire)" << std::endl;
    break;
  default:
    std::cout << "ERROR::Failed to acquire next swapchain image" << std::endl;
    break;
  }
}
