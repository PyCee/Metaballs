#include "surface.hpp"
#include <iostream>
#include "swapchain.hpp"

using namespace MTB;
Surface::Surface(Window &window, Vulkan_Instance *vulkan_instance,
		 GPU_Manager *gpu_manager):
  p_vulkan_instance(vulkan_instance), p_gpu_manager(gpu_manager){
#if defined(_WIN32)
  VkWin32SurfaceCreateInfoKHR surface_create_info = {
    .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
    .pNext = NULL,
    .flags = 0,
    //.hinstance = Window.Instance,//TODO support windows
    //.hwind = Window.Handle,
  };
  if(vkCreateWin32SurfaceKHR(this->p_vulkan_instance->get_instance(),
			     &surface_create_info, NULL,
			     &p_presentation_surface.surface) != VK_SUCCESS){
    std::cout << "ERROR::Failed to create win32 surface" << std::endl;
  } else{
    std::cout << "Created win32 surface" << std::endl;
  }
#elif defined(__unix__)
  VkXcbSurfaceCreateInfoKHR surface_create_info = {
    .sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
    .pNext = NULL,
    .flags = 0,
    .connection = window.m_connection,
    .window = window.m_window,
  };
  if(vkCreateXcbSurfaceKHR(this->p_vulkan_instance->get_instance(),
			   &surface_create_info, NULL,
			   &this->m_surface) != VK_SUCCESS){
    std::cout << "ERROR::Failed to create xcb surface" << std::endl;
  } else{
    std::cout << "Created xcb surface: " << this->m_surface << std::endl;
  }
#endif /* __unix__ */
  unsigned int surface_support;
  vkGetPhysicalDeviceSurfaceSupportKHR(gpu_manager->get_physical_device(),
				       gpu_manager->get_queue_family_index(),
				       this->m_surface, &surface_support);
  if(surface_support != VK_TRUE){
    std::cout << "ERROR::Physical device does not support surfaces" << std::endl;
  }
}
Surface::~Surface(){
  std::cout << "destroying surface: " << this->m_surface << std::endl;
  vkDestroySurfaceKHR(this->p_vulkan_instance->get_instance(),
		      this->m_surface, NULL);
}
