#ifndef MTB_SHADER_MODULE
#define MTB_SHADER_MODULE

#include "vulkan_instance.hpp"
#include "gpu_manager.hpp"

namespace MTB{
  class Shader_Module{
  private:
    GPU_Manager *m_p_gpu_manager;
    VkShaderModule m_module;
    VkShaderStageFlagBits m_stage;
  public:
    Shader_Module(void){};
    Shader_Module(GPU_Manager *, std::string, VkShaderStageFlagBits);
    ~Shader_Module();
    VkShaderModule get_module(void){return this->m_module;}
    VkShaderStageFlagBits get_stage(void){return this->m_stage;}
  };
}

#endif /* MTB_SHADER_MODULE */
