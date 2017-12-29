#include "vulkan_instance.hpp"
#include <iostream>

using namespace MTB;
Vulkan_Instance::Vulkan_Instance(){
#if defined(MTB_DEBUG)
  // support extensions
#endif
  VkApplicationInfo application_create_info = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pNext = NULL,
    .pApplicationName = "Metaball",
    .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
    .pEngineName = "Metaball_Custom",
    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    .apiVersion = VK_API_VERSION_1_0
  };
  VkInstanceCreateInfo instance_create_info = {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .pApplicationInfo = &application_create_info,
    .enabledLayerCount = 0,
    .ppEnabledLayerNames = NULL,
    .enabledExtensionCount = 0,
    .ppEnabledExtensionNames = NULL
  };
  this->m_instance = VK_NULL_HANDLE;
  if(vkCreateInstance(&instance_create_info, NULL,
		      &this->m_instance) != VK_SUCCESS){
    std::cout << "ERROR::Failed to create vulkan instance" << std::endl;
  } else {
    std::cout << "Created vulkan instance" << std::endl;
  }
}
Vulkan_Instance::~Vulkan_Instance(){
  
}
