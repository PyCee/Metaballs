#ifndef MTB_PIPELINE
#define MTB_PIPELINE

#include "vulkan_instance.hpp"
#include "gpu_manager.hpp"
#include "render_context.hpp"
#include "image_view.hpp"
#include "shader_module.hpp"
#include "memory_map.hpp"
#include "metaball.hpp"
#include <vector>
namespace MTB{
  class Pipeline {
  private:
    GPU_Manager *m_p_gpu_manager;
    VkRenderPass m_vk_render_pass;
    VkFramebuffer m_vk_framebuffer;
    Image_View m_image_view;
    VkDescriptorSetLayout m_desc_set_layout;
    VkPipelineLayout m_vk_pipeline_layout;
    VkPipeline m_vk_graphics_pipeline;
    VkSampler m_vk_sampler;
    VkDescriptorPool m_descriptor_pool;
    VkDescriptorSet m_descriptor_set;
    Uniform_Buffer m_metaball_buffer;
    VkCommandBuffer m_command_buffer;
    Memory_Map m_screen_plane,
      m_plane_indices;
    VkClearValue m_clear_value;
    VkRenderPassBeginInfo m_render_pass_begin_info;
    VkImageMemoryBarrier m_undefined_to_present,
      m_present_to_draw,
      m_draw_to_present;
  public:
    Pipeline(){};
    Pipeline(GPU_Manager *, Render_Context &);
    ~Pipeline();
    VkCommandBuffer get_command_buffer(void){return this->m_command_buffer;};
    void build_command_buffer(std::vector<Metaball>&);
    void set_metaball_data(Metaball_Data data){
      this->m_metaball_buffer.flush(&data, sizeof(data));
    };
  };
}

#endif /* MTB_PIPELINE */
