#include "shader_module.hpp"
#include <iostream>
#include <fstream>

using namespace MTB;
Shader_Module::Shader_Module(GPU_Manager *p_gpu_manager,
			     std::string path, VkShaderStageFlagBits stage):
  m_p_gpu_manager(p_gpu_manager),
  m_module(VK_NULL_HANDLE),
  m_stage(stage){
  
  std::ifstream file(path.c_str(), std::ios::in | std::ios::ate);
  if(!file.is_open()){
    std::cout << "failed to open shader file (dne): " << path << std::endl;
  }
  std::streampos size = file.tellg();
  char *data = new char[size];
  file.seekg(0, std::ios::beg);
  file.read(data, size);
  file.close();
  
  VkShaderModuleCreateInfo shader_module_create_info = {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .codeSize = (long unsigned int)size,
    .pCode = (unsigned int *)data
  };
  if(vkCreateShaderModule(this->m_p_gpu_manager->get_logical_device(),
			  &shader_module_create_info,
			  NULL, &this->m_module) != VK_SUCCESS){
    std::cout << "failed to create shader module:" << path << std::endl;
  }
}
Shader_Module::~Shader_Module(){
  vkDestroyShaderModule(this->m_p_gpu_manager->get_logical_device(),
			this->m_module, NULL);
}
