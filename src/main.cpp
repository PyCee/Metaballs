#include <iostream>
#include "metaball.hpp"
#include "window.hpp"
#include "vulkan_instance.hpp"
#include "gpu_manager.hpp"
#include "surface.hpp"
#include "swapchain.hpp"
#include "render_context.hpp"

using namespace MTB;

const char *PROGRAM_NAME = "Metaballs";
const char METABALL_COUNT = 4;

int main(){
  std::cout << "Starting program: " << PROGRAM_NAME << std::endl;
  Metaball metaballs[METABALL_COUNT];
  Window window = Window(PROGRAM_NAME, 600, 400);
  Vulkan_Instance vk_instance = Vulkan_Instance();
  GPU_Manager gpu_manager(vk_instance);
  Render_Context rc(window, &vk_instance, &gpu_manager);
  
  VkCommandBuffer command_buffer;
  gpu_manager.allocate_command_buffer(&command_buffer, 1);
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
  VkImageMemoryBarrier barrier_from_present_to_clear = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .pNext = NULL,
    .srcAccessMask = 0,
    .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
    .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    .srcQueueFamilyIndex = gpu_manager.get_queue_family_index(),
    .dstQueueFamilyIndex = gpu_manager.get_queue_family_index(),
    .image = rc.m_render_target.get_image(),
    .subresourceRange = image_subresource_range
  };
  VkImageMemoryBarrier barrier_from_clear_to_present = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .pNext = NULL,
    .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
    .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
    .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    .srcQueueFamilyIndex = gpu_manager.get_queue_family_index(),
    .dstQueueFamilyIndex = gpu_manager.get_queue_family_index(),
    .image = rc.m_render_target.get_image(),
    .subresourceRange = image_subresource_range
  };
  VkClearColorValue clear_color = {{0.005f, 0.0f, 0.005f, 0.0f}};
  
  vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);
  
  vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
		       VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL,
		       1, &barrier_from_present_to_clear);
  
  vkCmdClearColorImage(command_buffer, rc.m_render_target.get_image(),
		       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		       &clear_color, 1, &image_subresource_range);
  
  vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
		       VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL,
		       0, NULL, 1, &barrier_from_clear_to_present);
  
  if(vkEndCommandBuffer(command_buffer) != VK_SUCCESS){
    std::cout << "ERROR::Failed to end command buffer" << std::endl;
  }
  
  
  std::cout << "Pre loop" << std::endl;
  while(!window.is_closed()){
    
    gpu_manager.add_wait_semaphore(rc.m_swapchain.get_semaphore_acquired_image(),
				   VK_PIPELINE_STAGE_TRANSFER_BIT);
    gpu_manager.add_signal_semaphore(rc.m_target_rendered);
    gpu_manager.queue_up_command(command_buffer);
    gpu_manager.submit();

    rc.present();
    
  }
  std::cout << "Post loop" << std::endl;
  
  vkDeviceWaitIdle(gpu_manager.get_logical_device());
  std::cout << "Ending program: " << PROGRAM_NAME << std::endl;
}
