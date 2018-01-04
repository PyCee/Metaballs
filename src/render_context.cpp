#include "render_context.hpp"
#include <iostream>

using namespace MTB;
Render_Context::Render_Context(Window &window, Vulkan_Instance *vulkan_instance,
			       GPU_Manager *gpu_manager):
  p_gpu_manager(gpu_manager),
  m_surface(window, vulkan_instance, gpu_manager),
  m_swapchain(gpu_manager, &this->m_surface),
  m_render_target(gpu_manager,
		  this->m_swapchain.get_extent().width,
		  this->m_swapchain.get_extent().height,
		  this->m_swapchain.get_surface_format().format,
		  VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
		  VK_IMAGE_USAGE_TRANSFER_DST_BIT |
		  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		  VK_IMAGE_ASPECT_COLOR_BIT){
  unsigned int i;
  // Create render_target --> swapchain image copy buffer for each swapchain image
  VkCommandBufferBeginInfo command_buffer_begin_info = {
    VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    NULL,
    VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
    NULL
  };
  VkImageSubresourceRange image_subresource_range = {
    VK_IMAGE_ASPECT_COLOR_BIT,
    0,
    1,
    0,
    1
  };
  VkImageSubresourceLayers sub_resource = {
    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    .mipLevel = 0,
    .baseArrayLayer = 0,
    .layerCount = 1
  };
  VkImageCopy region = {
    .srcSubresource = sub_resource,
    .srcOffset = {0, 0, 0},
    .dstSubresource = sub_resource,
    .dstOffset = {0, 0, 0},
    .extent = {
      this->m_swapchain.get_extent().width,
      this->m_swapchain.get_extent().height,
      .depth = 1
    }
  };
  this->m_render_target_copy_buffers =
    new VkCommandBuffer[this->m_swapchain.get_image_count()];
  gpu_manager->allocate_command_buffer(this->m_render_target_copy_buffers,
				       this->m_swapchain.get_image_count());
  
  for(i = 0; i < this->m_swapchain.get_image_count(); ++i){;
    VkImageMemoryBarrier setup_barriers[] = {
      {
	.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
	.pNext = NULL,
	.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
	.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
	.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
	.srcQueueFamilyIndex = gpu_manager->get_queue_family_index(),
	.dstQueueFamilyIndex = gpu_manager->get_queue_family_index(),
	.image = this->m_render_target.get_image(),
	.subresourceRange = image_subresource_range
      },
      {
	.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
	.pNext = NULL,
	.srcAccessMask = 0,
	.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
	.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	.srcQueueFamilyIndex = gpu_manager->get_queue_family_index(),
	.dstQueueFamilyIndex = gpu_manager->get_queue_family_index(),
	.image = this->m_swapchain.get_image(i),
	.subresourceRange = image_subresource_range
      }
    };
    VkImageMemoryBarrier teardown_barriers[] = {
      {
	.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
	.pNext = NULL,
	.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
	.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
	.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
	.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	.srcQueueFamilyIndex = gpu_manager->get_queue_family_index(),
	.dstQueueFamilyIndex = gpu_manager->get_queue_family_index(),
	.image = this->m_render_target.get_image(),
	.subresourceRange = image_subresource_range
      },
      {
	.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
	.pNext = NULL,
	.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
	.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
	.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	.srcQueueFamilyIndex = gpu_manager->get_queue_family_index(),
	.dstQueueFamilyIndex = gpu_manager->get_queue_family_index(),
	.image = this->m_swapchain.get_image(i),
	.subresourceRange = image_subresource_range
      }
    };
    
    vkBeginCommandBuffer(this->m_render_target_copy_buffers[i],
			 &command_buffer_begin_info);
    vkCmdPipelineBarrier(this->m_render_target_copy_buffers[i],
			 VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			 VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			 0, 0, NULL, 0, NULL, 2, setup_barriers);
    vkCmdCopyImage(this->m_render_target_copy_buffers[i],
		   this->m_render_target.get_image(),
		   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		   this->m_swapchain.get_image(i),
		   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		   1, &region);
    vkCmdPipelineBarrier(this->m_render_target_copy_buffers[i],
			 VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			 VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			 0, 0, NULL, 0, NULL, 2, teardown_barriers);
    vkEndCommandBuffer(this->m_render_target_copy_buffers[i]);
  }
  
  VkSemaphoreCreateInfo semaphore_create_info = {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0
  };
  if(vkCreateSemaphore(gpu_manager->get_logical_device(), &semaphore_create_info, NULL,
		       &this->m_target_rendered) != VK_SUCCESS){
    std::cout << "ERROR::Failed to create render_context->m_target_rendered"<<std::endl;
  }
}
Render_Context::~Render_Context(){
  delete[] this->m_render_target_copy_buffers;
  vkDestroySemaphore(this->p_gpu_manager->get_logical_device(),
		     this->m_target_rendered, NULL);
}
void Render_Context::present(void){
  this->p_gpu_manager->queue_up_command(this->get_current_render_target_copy_buffer());
  this->p_gpu_manager->add_wait_semaphore(this->m_target_rendered,
					  VK_PIPELINE_STAGE_TRANSFER_BIT);
  this->p_gpu_manager->add_signal_semaphore(this->m_swapchain.get_semaphore_ready_to_present());
  this->p_gpu_manager->submit();
  this->m_swapchain.present_image();
}
