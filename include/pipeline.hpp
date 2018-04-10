#ifndef MTB_PIPELINE
#define MTB_PIPELINE

#include "vulkan_instance.hpp"
#include "gpu_manager.hpp"
#include "render_context.hpp"
#include "image_view.hpp"
#include "shader_module.hpp"
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
    VkDescriptorPool m_descriptor_pool;
    VkDescriptorSet m_descriptor_set;
  public:
    Pipeline(){};
    Pipeline(GPU_Manager *, Render_Context &);
    ~Pipeline();
  };
}

#endif /* MTB_PIPELINE */
