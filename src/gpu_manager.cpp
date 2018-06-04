#include "gpu_manager.hpp"
#include <iostream>

using namespace MTB;

GPU_Manager::GPU_Manager(Vulkan_Instance &instance){
  unsigned int gpu_count;
  if(vkEnumeratePhysicalDevices(instance.get_instance(), &gpu_count, NULL) !=
     VK_SUCCESS || 0 == gpu_count){
    std::cout << "ERROR::Failed enumerating physical device count" << std::endl;
  }
  this->m_physical_devices = new VkPhysicalDevice[gpu_count];
  if(vkEnumeratePhysicalDevices(instance.get_instance(), &gpu_count,
				this->m_physical_devices) != VK_SUCCESS){
    std::cout << "ERROR::Failed enumerating physical devices" << std::endl;
  } else {
    std::cout << "Enumerated physical devices: " << gpu_count << std::endl;
    vkGetPhysicalDeviceProperties(this->m_physical_devices[0],
				  &this->m_device_properties);
    std::cout << "Using physical device: " << this->m_device_properties.deviceName <<
      std::endl;
  }
  
  vkGetPhysicalDeviceMemoryProperties(this->get_physical_device(),
				      &this->m_memory_properties);
    
  unsigned int queue_family_count;
  vkGetPhysicalDeviceQueueFamilyProperties(this->m_physical_devices[0],
					   &queue_family_count, NULL);
  
  VkQueueFamilyProperties queue_family_properties[queue_family_count];
  vkGetPhysicalDeviceQueueFamilyProperties(this->m_physical_devices[0],
					   &queue_family_count, 
					   queue_family_properties);
  unsigned int queue_family_index;
  for(queue_family_index = 0; queue_family_index < queue_family_count;
      ++queue_family_index){
    if(queue_family_properties[queue_family_index].queueCount != 0){
      break;
    }
  }
  if(queue_family_index == queue_family_count){
    std::cout << "ERROR::failed to find queue family with any queues" << std::endl;
  }
  this->m_queue_family_index = queue_family_index;
  
  // Create 1 queue family
  float queue_priorities[1] = {1.0};
  VkDeviceQueueCreateInfo queue_create_info = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .queueFamilyIndex = queue_family_index,
    .queueCount = 1,
    .pQueuePriorities = queue_priorities
  };
  const char *extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
  };
  const char *layers[] = {
#if defined(MTB_DEBUG)
    "VK_LAYER_LUNARG_standard_validation"
#endif /* MTB_DEBUG */
  };
  VkDeviceCreateInfo device_create_info = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .queueCreateInfoCount = 1,
    .pQueueCreateInfos = &queue_create_info,
    .enabledLayerCount = sizeof(layers) / sizeof(*layers),
    .ppEnabledLayerNames = layers,
    .enabledExtensionCount = sizeof(extensions) / sizeof(*extensions),
    .ppEnabledExtensionNames = extensions,
    .pEnabledFeatures = NULL
  };
  this->m_logical_device = VK_NULL_HANDLE;
  if(vkCreateDevice(this->m_physical_devices[0], &device_create_info, NULL,
		    &this->m_logical_device) != VK_SUCCESS){
    std::cout << "ERRROR::Failed to create logical device" << std::endl;
  } else{
    std::cout << "Created logical device" << std::endl;
  }
  
  VkCommandPoolCreateInfo pool_create_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .pNext = NULL,
    //.flags = 0,
    .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    .queueFamilyIndex = queue_family_index
  };
  if(vkCreateCommandPool(this->m_logical_device, &pool_create_info,
			 NULL, &this->m_command_pool) != VK_SUCCESS){
    std::cout << "ERROR::Failed to create command pool" << std::endl;
  } else{
    std::cout << "Created command pool" << std::endl;
  }
  
  // Get a single device queue from index 0
  vkGetDeviceQueue(this->m_logical_device, queue_family_index, 0,
		   &this->m_queue);
}
GPU_Manager::~GPU_Manager(){
  // Wait for queue to finish
  vkQueueWaitIdle(this->m_queue);
  // Free Command Pool
  vkResetCommandPool(this->m_logical_device, this->m_command_pool,
   		     VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
  vkDestroyCommandPool(this->m_logical_device, this->m_command_pool, NULL);
  // Free logical device
  vkDeviceWaitIdle(this->m_logical_device);
  vkDestroyDevice(this->m_logical_device, NULL);
}
void GPU_Manager::allocate_command_buffers(VkCommandBuffer *p_command_buffer,
					   unsigned int buffer_count=1){
  VkCommandBufferAllocateInfo command_buffer_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .pNext = NULL,
    .commandPool = this->m_command_pool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = buffer_count
  };
  if(vkAllocateCommandBuffers(this->get_logical_device(), &command_buffer_allocate_info,
			      p_command_buffer) != VK_SUCCESS){
    std::cout << "ERROR::Failed to allocate command buffer" << std::endl;
  }
}
void GPU_Manager::submit(void){
  if(this->command_buffers.size() > 0){
    VkSubmitInfo submission_info = {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = NULL,
      .waitSemaphoreCount = (unsigned int) this->wait_sem.size(),
      .pWaitSemaphores = this->wait_sem.data(),
      .pWaitDstStageMask = this->wait_dst_mask.data(),
      .commandBufferCount = (unsigned int) this->command_buffers.size(),
      .pCommandBuffers = this->command_buffers.data(),
      .signalSemaphoreCount = (unsigned int) this->sig_sem.size(),
      .pSignalSemaphores = this->sig_sem.data()
    };
    vkQueueWaitIdle(this->get_queue());
    if(vkQueueSubmit(this->get_queue(), 1, &submission_info,
		     NULL) != VK_SUCCESS){
		     //p_submission_queue->queue_finished) != VK_SUCCESS){
      std::cout << "ERROR::Failed to submit command queues" << std::endl;
    }
    this->wait_sem.clear();
    this->sig_sem.clear();
    this->wait_dst_mask.clear();
    this->command_buffers.clear();
  } else{
    std::cout << "ERROR::Command submission: <= 1 command" << std::endl;
  }
}
unsigned int GPU_Manager::get_memory_type_index(VkMemoryRequirements mem_req,
			       VkMemoryPropertyFlags prop_flags){
  unsigned int i;
  for(i = 0; i < this->get_memory_properties().memoryTypeCount; ++i){
    if((mem_req.memoryTypeBits & (uint32_t)(1 << i)) &&
       ((this->get_memory_properties().memoryTypes[i].propertyFlags & prop_flags) ==
	prop_flags)){
      // If the memory requirements correspond with our current index
      // And the supported memory types at this index contain prop_flags
      
      // Then a viable memory type has been found
      return i;
    }
  }
  std::cout << "failed to find viable memory type" << std::endl;
  return (unsigned int)-1;
}
long GPU_Manager::get_alignment(long size){
  long min_alignment = this->m_device_properties.limits.minUniformBufferOffsetAlignment;
  if(min_alignment){
    return (size + min_alignment - 1) & ~(min_alignment - 1);
  }
  return size;
}
