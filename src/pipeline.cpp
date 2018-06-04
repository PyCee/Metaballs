#include "pipeline.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <cstring>
#include <cstdlib>

using namespace MTB;

static float screen_plane[] = {
  -1.0f, -1.0f,
  -1.0f, 1.0f,
  1.0f, -1.0f,
  1.0f, 1.0f
};
static short plane_indices[] = {
  0, 1, 2,
  3, 2, 1
};

const short MAX_METABALL_COUNT = 32;

Pipeline::Pipeline(GPU_Manager *p_gpu_manager, Render_Context &render_context):
  m_p_gpu_manager(p_gpu_manager),
  m_vk_render_pass(VK_NULL_HANDLE),
  m_vk_framebuffer(VK_NULL_HANDLE),
  m_image_view(p_gpu_manager, render_context.get_render_target(),
	       VK_IMAGE_ASPECT_COLOR_BIT),
  m_desc_set_layout(VK_NULL_HANDLE),
  m_vk_pipeline_layout(VK_NULL_HANDLE),
  m_vk_graphics_pipeline(VK_NULL_HANDLE),
  m_vk_sampler(VK_NULL_HANDLE),
  m_descriptor_pool(VK_NULL_HANDLE),
  m_descriptor_set(VK_NULL_HANDLE),
  m_metaball_buffer(p_gpu_manager, p_gpu_manager->get_alignment(sizeof(Metaball_Data)) *
		    MAX_METABALL_COUNT),
  m_command_buffer(VK_NULL_HANDLE),
  m_screen_plane(p_gpu_manager, sizeof(screen_plane),
		 VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT),
  m_plane_indices(p_gpu_manager, sizeof(plane_indices),
		  VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT),
  m_clear_value({.color={{0.0f, 0.0f, 0.0f, 1.0f}}}){
  
  // Renderpass Creation
  VkAttachmentDescription attachment_descriptions[] = {
    {
      .flags = 0,
      .format = render_context.get_surface_format().format,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    }
  };
  VkAttachmentReference color_attachment_refs[] = {
    {
      .attachment = 0,
      .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    }
  };
  unsigned int color_attachment_ref_count =
    sizeof(color_attachment_refs) / sizeof(color_attachment_refs[0]);
  VkSubpassDescription subpass_descriptions[] = {
    {
      .flags = 0,
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .inputAttachmentCount = 0,
      .pInputAttachments = NULL,
      .colorAttachmentCount = color_attachment_ref_count,
      .pColorAttachments = color_attachment_refs,
      .pResolveAttachments = NULL,
      .pDepthStencilAttachment = NULL,
      .preserveAttachmentCount = 0,
      .pPreserveAttachments = NULL
    }
  };
  
  VkRenderPassCreateInfo render_pass_create_info = {
    .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .attachmentCount = 1,
    .pAttachments = attachment_descriptions,
    .subpassCount = 1,
    .pSubpasses = subpass_descriptions,
    .dependencyCount = 0,
    .pDependencies = NULL,
  };
  if(vkCreateRenderPass(this->m_p_gpu_manager->get_logical_device(),
			&render_pass_create_info, NULL,
			&this->m_vk_render_pass) != VK_SUCCESS){
    std::cout << "failed to create vk render pass" << std::endl;
  } else {
    std::cout << "Created Vk Render Pass" << std::endl;
  }

  // Framebuffer Creation
  VkImageView img_view = this->m_image_view.get_view();
  VkFramebufferCreateInfo framebuffer_create_info = {
    .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .renderPass = this->m_vk_render_pass,
    .attachmentCount = 1,
    .pAttachments = &img_view,
    .width = render_context.get_width(),
    .height = render_context.get_height(),
    .layers = 1
  };
  
  if(vkCreateFramebuffer(this->m_p_gpu_manager->get_logical_device(),
			 &framebuffer_create_info, NULL,
			 &this->m_vk_framebuffer) != VK_SUCCESS){
    std::cout << "failed to create framebuffer" << std::endl;
  } else {
    std::cout << "Created Vk Framebuffer" << std::endl;
  }
  
  
  // Graphics Pipeline Creation
  Shader_Module vertex_shader(this->m_p_gpu_manager, "shaders/meta.vert.spv",
			      VK_SHADER_STAGE_VERTEX_BIT),
    frag_shader(this->m_p_gpu_manager, "shaders/meta.frag.spv",
		VK_SHADER_STAGE_FRAGMENT_BIT);
  
  VkPipelineShaderStageCreateInfo shader_stage_create_info[] = {
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .stage = vertex_shader.get_stage(),
      .module = vertex_shader.get_module(),
      .pName = "main",
      .pSpecializationInfo = NULL
    },
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .stage = frag_shader.get_stage(),
      .module = frag_shader.get_module(),
      .pName = "main",
      .pSpecializationInfo = NULL
    }
  };
  
  VkVertexInputBindingDescription vertex_binding_description = {
    .binding = 0,
    .stride = (unsigned int)sizeof(float) * 2,
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
  };
  VkVertexInputAttributeDescription vertex_attribute_description[] = {
    {
      .location = 0,
      .binding = vertex_binding_description.binding,
      .format = VK_FORMAT_R32G32_SFLOAT,
      .offset = sizeof(float) * 0
    }
  };
  
  VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .vertexBindingDescriptionCount = 1,
    .pVertexBindingDescriptions = &vertex_binding_description,
    .vertexAttributeDescriptionCount = 1,
    .pVertexAttributeDescriptions = vertex_attribute_description
  };
  
  VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    .primitiveRestartEnable = VK_FALSE
  };
  VkViewport viewport = {
    .x = 0,
    .y = 0,
    .width = (float)render_context.get_width(),
    .height = (float)render_context.get_height(),
    .minDepth = 0.0f,
    .maxDepth = 1.0f
  };
  VkRect2D scissor = {
    .offset = {
      .x = 0,
      .y = 0
    },
    .extent = {
      .width = render_context.get_width(),
      .height = render_context.get_height()
    }
  };
  VkPipelineViewportStateCreateInfo viewport_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    // The below variables allow for a dynamic viewport state
    .viewportCount = 1,
    .pViewports = &viewport,
    .scissorCount = 1,
    .pScissors = &scissor
  };
  
  VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .depthClampEnable = VK_FALSE,
    .rasterizerDiscardEnable = VK_FALSE,
    .polygonMode = VK_POLYGON_MODE_FILL,
    .cullMode = VK_CULL_MODE_BACK_BIT,
    .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
    .depthBiasEnable = VK_FALSE,
    .depthBiasConstantFactor = 0.0f,
    .depthBiasClamp = 0.0f,
    .depthBiasSlopeFactor = 0.0f,
    .lineWidth = 1.0f
  };
  
  VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    .sampleShadingEnable = VK_FALSE,
    .minSampleShading = 1.0f,
    .pSampleMask = NULL,
    .alphaToCoverageEnable = VK_FALSE,
    .alphaToOneEnable = VK_FALSE
  };
  
  VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
    .blendEnable = VK_FALSE,
    .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
    .colorBlendOp = VK_BLEND_OP_ADD,
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
    .alphaBlendOp = VK_BLEND_OP_ADD,
    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
    VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
  };
  
  VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .logicOpEnable = VK_FALSE,
    .logicOp = VK_LOGIC_OP_COPY,
    .attachmentCount = 1,
    .pAttachments = &color_blend_attachment_state,
    .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
  };
  
  VkDescriptorSetLayoutBinding desc_set_layout_bindings[] = {
    {
      .binding = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
      .pImmutableSamplers = NULL
    },
    {
      .binding = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
      .pImmutableSamplers = NULL
    }
  };
  unsigned int desc_set_layout_bindings_count =
    sizeof(desc_set_layout_bindings) / sizeof(*desc_set_layout_bindings);
    
  VkDescriptorSetLayoutCreateInfo desc_set_layout_create_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .bindingCount = desc_set_layout_bindings_count,
      .pBindings = desc_set_layout_bindings
  };
  vkCreateDescriptorSetLayout(this->m_p_gpu_manager->get_logical_device(),
			      &desc_set_layout_create_info,
			      NULL, &this->m_desc_set_layout);
    
  VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .setLayoutCount = 1,
    .pSetLayouts = &this->m_desc_set_layout,
    .pushConstantRangeCount = 0,
    .pPushConstantRanges = NULL
  };
  if(vkCreatePipelineLayout(this->m_p_gpu_manager->get_logical_device(),
			    &pipeline_layout_create_info, NULL,
			    &this->m_vk_pipeline_layout) !=
     VK_SUCCESS){
    std::cout << "failed to create pipeline layout" << std::endl;
  } else {
    std::cout << "Created pipeline layout" << std::endl;
  }
  
  VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .stageCount = 2,
    .pStages = shader_stage_create_info,
    .pVertexInputState = &vertex_input_state_create_info,
    .pInputAssemblyState = &input_assembly_state_create_info,
    .pTessellationState = NULL,
    .pViewportState = &viewport_state_create_info,
    .pRasterizationState = &rasterization_state_create_info,
    .pMultisampleState = &multisample_state_create_info,
    .pDepthStencilState = NULL,
    .pColorBlendState = &color_blend_state_create_info,
    .pDynamicState = NULL,
    .layout = this->m_vk_pipeline_layout,
    .renderPass = this->m_vk_render_pass,
    .subpass = 0,
    .basePipelineHandle = VK_NULL_HANDLE,
    .basePipelineIndex = -1
  };
  
  if(vkCreateGraphicsPipelines(this->m_p_gpu_manager->get_logical_device(),
			       VK_NULL_HANDLE, 1, &graphics_pipeline_create_info,
			       NULL, &this->m_vk_graphics_pipeline) != VK_SUCCESS){
    std::cout << "failed to create graphics pipeline" << std::endl;
  } else {
    std::cout << "Created graphics pipeline" << std::endl;
  }

  // Descriptor Pool Creation
  VkDescriptorPoolSize pool_sizes[] = {
    {
      .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
      .descriptorCount = 1
    },
    {
      .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = 1
    }
  };
  
  VkDescriptorPoolCreateInfo desc_pool_create_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .maxSets = 1,
    .poolSizeCount = 2,
    .pPoolSizes = pool_sizes
  };
  
  if(vkCreateDescriptorPool(this->m_p_gpu_manager->get_logical_device(),
			    &desc_pool_create_info,
			    NULL, &this->m_descriptor_pool) != VK_SUCCESS){
    std::cout << "failed to create descriptor pool" << std::endl;
  } else {
    std::cout << "Created descriptor pool" << std::endl;
  }

  // Descriptor Set Creation
  VkDescriptorSetAllocateInfo desc_set_alloc_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
    .pNext = NULL,
    .descriptorPool = this->m_descriptor_pool,
    .descriptorSetCount = 1,
    .pSetLayouts = &this->m_desc_set_layout
  };

  if(vkAllocateDescriptorSets(this->m_p_gpu_manager->get_logical_device(),
			      &desc_set_alloc_info,
			      &this->m_descriptor_set) != VK_SUCCESS){
    std::cout << "failed to allocate descriptor set" << std::endl;
  } else {
    std::cout << "Allocated descriptor set" << std::endl;
  }

  // Create Image Sampler for fragment shader sampling of the render-target
  VkSamplerCreateInfo image_sampler_create_info = {
    .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .magFilter = VK_FILTER_LINEAR,
    .minFilter = VK_FILTER_LINEAR,
    .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
    .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    .mipLodBias = 0,
    .anisotropyEnable = VK_FALSE,
    .maxAnisotropy = 0,
    .compareEnable = VK_FALSE,
    .compareOp = VK_COMPARE_OP_ALWAYS,
    .minLod = 0.0f,
    .maxLod = 0.0f,
    .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
    .unnormalizedCoordinates = VK_FALSE
  };
  if(vkCreateSampler(p_gpu_manager->get_logical_device(),
		     &image_sampler_create_info, NULL,
		     &this->m_vk_sampler) != VK_SUCCESS) {
    std::cout << "failed to create image sampler" << std::endl;
  } else {
    std::cout << "Created sampler" << std::endl;
  }

  // Setup descriptor writes
  VkDescriptorBufferInfo metaball_buffer_descriptor_info = {
    .buffer = this->m_metaball_buffer.get_buffer(),
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };
  VkDescriptorImageInfo image_descriptor_info = {
    .sampler = this->m_vk_sampler,
    .imageView = this->m_image_view.get_view(),
    .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
  };
  VkWriteDescriptorSet write_desc_sets[] = {
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = NULL,
      .dstSet = this->m_descriptor_set,
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
      .pImageInfo = NULL,
      .pBufferInfo = &metaball_buffer_descriptor_info,
      .pTexelBufferView = NULL
    },
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = NULL,
      .dstSet = this->m_descriptor_set,
      .dstBinding = 1,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .pImageInfo = &image_descriptor_info,
      .pBufferInfo = NULL,
      .pTexelBufferView = NULL
    }
  };
  vkUpdateDescriptorSets(p_gpu_manager->get_logical_device(), 2, write_desc_sets, 0,
			 NULL);
  
  // Allocate Command Buffer
  m_p_gpu_manager->allocate_command_buffers(&this->m_command_buffer, 1);

  // Flush screen plane data
  this->m_screen_plane.flush(screen_plane, sizeof(screen_plane));
  this->m_plane_indices.flush(plane_indices, sizeof(plane_indices));

  // Set variables that will be used with the command buffer
  VkImageSubresourceRange image_subresource_range = {
    VK_IMAGE_ASPECT_COLOR_BIT,
    0,
    1,
    0,
    1
  };
  
  this->m_render_pass_begin_info = (VkRenderPassBeginInfo) {
    .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    .pNext = NULL,
    .renderPass = this->m_vk_render_pass,
    .framebuffer = this->m_vk_framebuffer,
    .renderArea = {
      .offset = {
	.x = 0,
	.y = 0
      },
      .extent = render_context.get_swapchain_extent()
    },
    .clearValueCount = 1,
    .pClearValues = &this->m_clear_value
  };
  
  this->m_undefined_to_present = (VkImageMemoryBarrier) {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .pNext = NULL,
    .srcAccessMask = 0,
    .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
    .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    .srcQueueFamilyIndex = p_gpu_manager->get_queue_family_index(),
    .dstQueueFamilyIndex = p_gpu_manager->get_queue_family_index(),
    .image = render_context.get_render_target().get_image(),
    .subresourceRange = image_subresource_range
  };
  this->m_present_to_draw = (VkImageMemoryBarrier) {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .pNext = NULL,
    .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    .oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    .srcQueueFamilyIndex = p_gpu_manager->get_queue_family_index(),
    .dstQueueFamilyIndex = p_gpu_manager->get_queue_family_index(),
    .image = render_context.get_render_target().get_image(),
    .subresourceRange = image_subresource_range
  };
  this->m_draw_to_present = (VkImageMemoryBarrier) {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .pNext = NULL,
    .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
    .oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    .srcQueueFamilyIndex = p_gpu_manager->get_queue_family_index(),
    .dstQueueFamilyIndex = p_gpu_manager->get_queue_family_index(),
    .image = render_context.get_render_target().get_image(),
    .subresourceRange = image_subresource_range
  };
}
Pipeline::~Pipeline(){
  vkDestroySampler(this->m_p_gpu_manager->get_logical_device(),
		   this->m_vk_sampler, NULL);
  vkDestroyDescriptorSetLayout(this->m_p_gpu_manager->get_logical_device(),
			       this->m_desc_set_layout, NULL);
  vkDestroyDescriptorPool(this->m_p_gpu_manager->get_logical_device(),
			  this->m_descriptor_pool, NULL);
  vkDestroyPipelineLayout(this->m_p_gpu_manager->get_logical_device(),
			  this->m_vk_pipeline_layout, NULL);
  vkDestroyPipeline(this->m_p_gpu_manager->get_logical_device(),
		    this->m_vk_graphics_pipeline, NULL);
  vkDestroyFramebuffer(this->m_p_gpu_manager->get_logical_device(),
		       this->m_vk_framebuffer, NULL);
  vkDestroyRenderPass(this->m_p_gpu_manager->get_logical_device(),
		      this->m_vk_render_pass, NULL);
}
void Pipeline::build_command_buffer(std::vector<Metaball>& metaballs){
  // TODO:: have another dynamic buffer with scale/translation for the points in vertex shader. Will save time by not rendereing the entire screen every pass
  assert(metaballs.size() < MAX_METABALL_COUNT);
  unsigned int i;

  // Setup dynamic uniform buffer
  unsigned int buffer_alignment =
    this->m_p_gpu_manager->get_alignment(sizeof(Metaball_Data));
  size_t data_size = buffer_alignment * metaballs.size();
  void *metaball_data = malloc(data_size);
  
  for(i = 0; i < metaballs.size(); ++i){
    Metaball_Data tmp_data = metaballs[i].get_data();
    char *dst = ((char*)metaball_data) + buffer_alignment * i;
    memcpy(dst, &tmp_data, sizeof(tmp_data));
  }
  this->m_metaball_buffer.flush(metaball_data, data_size);
  free(metaball_data);
  
  // Ready to Record Command Buffer
  vkResetCommandBuffer(this->m_command_buffer,
		       VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
  
  VkCommandBufferBeginInfo command_buffer_begin_info = {
    VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    NULL,
    VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
    NULL
  };
  
  VkDeviceSize vertex_memory_offset = 0,
    index_memory_offset = 0;
  
  // Record Command Buffer
  vkBeginCommandBuffer(this->m_command_buffer, &command_buffer_begin_info);
  vkCmdPipelineBarrier(this->m_command_buffer,
		       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		       0, 0, NULL, 0, NULL, 1, &this->m_undefined_to_present);
  vkCmdPipelineBarrier(this->m_command_buffer,
		       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		       0, 0, NULL, 0, NULL, 1, &this->m_present_to_draw);
  
  vkCmdBeginRenderPass(this->m_command_buffer, &this->m_render_pass_begin_info,
		       VK_SUBPASS_CONTENTS_INLINE);
  
  vkCmdBindPipeline(this->m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		    this->m_vk_graphics_pipeline);  
  
  vkCmdBindVertexBuffers(this->m_command_buffer, 0, 1,
			 &this->m_screen_plane.get_buffer(), &vertex_memory_offset);
  vkCmdBindIndexBuffer(this->m_command_buffer, this->m_plane_indices.get_buffer(),
		       index_memory_offset, VK_INDEX_TYPE_UINT16);
  
  for(i = 0; i < metaballs.size(); ++i){
    
    unsigned int dynamic_offset = i * buffer_alignment;
    vkCmdBindDescriptorSets(this->m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			    this->m_vk_pipeline_layout,
			    0, 1, &this->m_descriptor_set, 1, &dynamic_offset);
    
    vkCmdDrawIndexed(this->m_command_buffer,
		     sizeof(plane_indices)/sizeof(plane_indices[0]),
		     1, 0, 0, 0);
  }
  
  vkCmdEndRenderPass(this->m_command_buffer);
  
  vkCmdPipelineBarrier(this->m_command_buffer,
		       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		       VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		       0, 0, NULL, 0, NULL, 1, &this->m_draw_to_present);
  
  if(vkEndCommandBuffer(this->m_command_buffer) != VK_SUCCESS){
    std::cout << "ERROR::Could not record pipeline command buffer" << std::endl;
  }
  
}
