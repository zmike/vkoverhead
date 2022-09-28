/*
 * Copyright 2022 Valve Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHOR(S) AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include "vkoverhead.h"
#include "data/shaders.x"

static VkPipeline
create_pipeline(VkPipelineLayout layout, VkRenderPass render_pass, VkShaderModule *modules,
                VkPipelineVertexInputStateCreateInfo *vertex_input_state, unsigned num_rts, bool dynamic,
                VkGraphicsPipelineLibraryCreateInfoEXT *gplci)
{
   VkPipelineInputAssemblyStateCreateInfo primitive_state = {0};
   primitive_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
   primitive_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

   VkPipelineColorBlendAttachmentState blend_att[MAX_RTS] = {0};
   for (unsigned i = 0; i < MAX_RTS; i++)
      blend_att[i].colorWriteMask = VK_COLOR_COMPONENT_FLAG_BITS_MAX_ENUM;
   VkPipelineColorBlendStateCreateInfo blend_state = {0};
   blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
   blend_state.attachmentCount = num_rts;
   blend_state.pAttachments = blend_att;

   VkPipelineMultisampleStateCreateInfo ms_state = {0};
   ms_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
   ms_state.rasterizationSamples = 1;
   ms_state.pSampleMask = NULL;

   VkPipelineViewportStateCreateInfo viewport_state = {0};
   viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
   VkViewport viewport = {0, 0, 100, 100, 0, 1};
   viewport_state.pViewports = &viewport;
   VkRect2D scissor = {0};
   scissor.extent.width = 100;
   scissor.extent.height = 100;
   viewport_state.pScissors = &scissor;

   VkPipelineRasterizationStateCreateInfo rast_state = {0};
   rast_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
   rast_state.polygonMode = VK_POLYGON_MODE_FILL;
   rast_state.cullMode = VK_CULL_MODE_NONE;
   rast_state.frontFace = VK_FRONT_FACE_CLOCKWISE;

   VkPipelineDepthStencilStateCreateInfo depth_stencil_state = {0};
   depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

   VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {0};
   pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

   VkDynamicState dynamicStateEnables[30] = {
      VK_DYNAMIC_STATE_DEPTH_BIAS,
      VK_DYNAMIC_STATE_BLEND_CONSTANTS,
      VK_DYNAMIC_STATE_STENCIL_REFERENCE,
   };
   unsigned state_count = 3;
   if (dev->info.have_EXT_extended_dynamic_state) {
      dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT;
      dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT;
      dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_DEPTH_BOUNDS;
      dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE;
      dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_DEPTH_COMPARE_OP;
      dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE;
      dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE;
      dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_STENCIL_WRITE_MASK;
      dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK;
      dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_STENCIL_OP;
      dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE;
      dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_FRONT_FACE;
      dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY;
      dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_CULL_MODE;
   } else {
      dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_VIEWPORT;
      dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_SCISSOR;
   }
   if (dev->info.have_EXT_vertex_input_dynamic_state)
      dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_VERTEX_INPUT_EXT;
   else if (dev->info.have_EXT_extended_dynamic_state)
      dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE;
   if (dev->info.have_EXT_extended_dynamic_state2) {
      dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE;
      dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE;
      if (dev->info.dynamic_state2_feats.extendedDynamicState2PatchControlPoints)
         dynamicStateEnables[state_count++] = VK_DYNAMIC_STATE_PATCH_CONTROL_POINTS_EXT;
   }
   if (dynamic) {
      pipelineDynamicStateCreateInfo.pDynamicStates = dynamicStateEnables;
      pipelineDynamicStateCreateInfo.dynamicStateCount = state_count;
   } else {
      viewport_state.viewportCount = 1;
      viewport_state.scissorCount = 1;
   }

   VkPipelineRenderingCreateInfo rendering = {0};
   VkFormat formats[MAX_RTS];
   for (unsigned i = 0; i < MAX_RTS; i++)
      formats[i] = VK_FORMAT_R32G32B32A32_SFLOAT;
   rendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
   rendering.colorAttachmentCount = num_rts;
   rendering.pNext = gplci;
   rendering.pColorAttachmentFormats = formats;

   VkGraphicsPipelineCreateInfo pci = {0};
   pci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
   pci.layout = layout;
   pci.renderPass = render_pass;
   if (gplci)
      pci.flags = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR;
   if (!render_pass)
      pci.pNext = &rendering;
   else
      pci.pNext = gplci;
   pci.pVertexInputState = vertex_input_state;
   pci.pInputAssemblyState = &primitive_state;
   pci.pRasterizationState = &rast_state;
   pci.pColorBlendState = &blend_state;
   pci.pMultisampleState = &ms_state;
   pci.pViewportState = &viewport_state;
   pci.pDepthStencilState = &depth_stencil_state;
   pci.pDynamicState = &pipelineDynamicStateCreateInfo;

   VkPipelineShaderStageCreateInfo shader_stages[5] = {0};
   unsigned module_count = 0;
   for (int i = 0; modules && i < 5; ++i) {
      if (!modules[i])
         continue;
      shader_stages[module_count].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      shader_stages[module_count].stage = 1<<i;
      shader_stages[module_count].module = modules[i];
      shader_stages[module_count].pName = "main";
      module_count++;
   }

   pci.pStages = shader_stages;
   pci.stageCount = module_count;

   VkPipeline pipeline;
   VkResult result = VK(CreateGraphicsPipelines)(dev->dev, VK_NULL_HANDLE, 1, &pci, NULL, &pipeline);
   VK_CHECK("CreateGraphicsPipelines", result);
   return pipeline;
}

static VkShaderModule
create_shader_module(unsigned char *spirv, unsigned int size)
{
   VkShaderModule mod;
   VkShaderModuleCreateInfo smci = {0};

   smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
   smci.codeSize = size;
   smci.pCode = (uint32_t*)spirv;
   VkResult result = VK(CreateShaderModule)(dev->dev, &smci, NULL, &mod);
   VK_CHECK("CreateShaderModule", result);
   return mod;
}


VkPipelineLayout
create_pipeline_layout(VkDescriptorSetLayout *desc_layouts, unsigned num_desc_layouts)
{
   VkPipelineLayoutCreateInfo plci = {0};
   plci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

   plci.pSetLayouts = desc_layouts;
   plci.setLayoutCount = num_desc_layouts;

   VkPipelineLayout layout;
   VkResult result = VK(CreatePipelineLayout)(dev->dev, &plci, NULL, &layout);
   VK_CHECK("CreatePipelineLayout", result);
   return layout;
}

static void
generate_vattribs(VkVertexInputBindingDescription *bindings, VkVertexInputAttributeDescription *vattr, unsigned num_vattrib)
{
   for (unsigned i = 0; i < num_vattrib; i++) {
      VkFormat formats[] = {
         VK_FORMAT_R32_SFLOAT,
         VK_FORMAT_R32G32_SFLOAT,
         VK_FORMAT_R32G32B32_SFLOAT,
         VK_FORMAT_R32G32B32A32_SFLOAT,
      };
      /* random format */
      int fmt_idx = rand() % ARRAY_SIZE(formats);
      unsigned num_components = fmt_idx + 1;

      bindings[i].binding = i;
      /* random stride aligned to element size */
      bindings[i].stride = sizeof(float) * num_components * MAX2(rand() % 3, 1);
      bindings[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      vattr[i].location = i;
      vattr[i].binding = i;
      vattr[i].format = formats[fmt_idx];
      vattr[i].offset = sizeof(float) * (rand() % 4);
   }
}

static VkPipeline
create_pipeline_helper(VkPipelineLayout layout, VkRenderPass render_pass, VkShaderModule *modules, unsigned num_rts)
{
   VkVertexInputBindingDescription vbinding;
   VkVertexInputAttributeDescription vattr;
   VkPipelineVertexInputStateCreateInfo vertex_input_state = {0};
   vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
   vertex_input_state.pVertexBindingDescriptions = &vbinding;
   vertex_input_state.vertexBindingDescriptionCount = 1;
   vertex_input_state.pVertexAttributeDescriptions = &vattr;
   vertex_input_state.vertexAttributeDescriptionCount = 1;
   generate_vattribs(&vbinding, &vattr, 1);
   return create_pipeline(layout, render_pass, modules, &vertex_input_state, num_rts, false, NULL);
}

void
create_basic_pipelines(VkRenderPass render_pass, VkPipelineLayout layout, VkPipeline *pipelines)
{
   VkShaderModule modules[5] = {
      create_shader_module(basic_vert_spirv, basic_vert_spirv_len),
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      create_shader_module(basic_frag_spirv, basic_frag_spirv_len),
   };

   for (unsigned i = 0; i < 4; i++)
      pipelines[i] = create_pipeline_helper(layout, render_pass, modules, 1);
}

VkPipeline
create_multirt_pipeline(VkRenderPass render_pass, VkPipelineLayout layout)
{
   VkShaderModule modules[5] = {
      create_shader_module(basic_vert_spirv, basic_vert_spirv_len),
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      create_shader_module(multirt_frag_spirv, multirt_frag_spirv_len),
   };

   return create_pipeline_helper(layout, render_pass, modules, 8);
}

VkPipeline
create_ubo_pipeline(VkRenderPass render_pass, VkPipelineLayout layout)
{
   VkShaderModule modules[5] = {
      create_shader_module(basic_vert_spirv, basic_vert_spirv_len),
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      create_shader_module(ubo_frag_spirv, ubo_frag_spirv_len),
   };

   return create_pipeline_helper(layout, render_pass, modules, 1);
}

VkPipeline
create_sampler_pipeline(VkRenderPass render_pass, VkPipelineLayout layout)
{
   VkShaderModule modules[5] = {
      create_shader_module(basic_vert_spirv, basic_vert_spirv_len),
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      create_shader_module(sampler_frag_spirv, sampler_frag_spirv_len),
   };

   return create_pipeline_helper(layout, render_pass, modules, 1);
}

VkPipeline
create_sampler_many_pipeline(VkRenderPass render_pass, VkPipelineLayout layout)
{
   VkShaderModule modules[5] = {
      create_shader_module(basic_vert_spirv, basic_vert_spirv_len),
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      create_shader_module(sampler_many_frag_spirv, sampler_many_frag_spirv_len),
   };

   return create_pipeline_helper(layout, render_pass, modules, 1);
}

VkPipeline
create_image_pipeline(VkRenderPass render_pass, VkPipelineLayout layout)
{
   VkShaderModule modules[5] = {
      create_shader_module(basic_vert_spirv, basic_vert_spirv_len),
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      create_shader_module(image_frag_spirv, image_frag_spirv_len),
   };

   return create_pipeline_helper(layout, render_pass, modules, 1);
}

VkPipeline
create_image_many_pipeline(VkRenderPass render_pass, VkPipelineLayout layout)
{
   unsigned char *spirv = image_many4_frag_spirv;
   unsigned len = image_many4_frag_spirv_len;
   if (dev->info.props.limits.maxPerStageDescriptorStorageImages > 4) {
      spirv = image_many8_frag_spirv;
      len = image_many8_frag_spirv_len;
   }
   VkShaderModule modules[5] = {
      create_shader_module(basic_vert_spirv, basic_vert_spirv_len),
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      create_shader_module(spirv, len),
   };

   return create_pipeline_helper(layout, render_pass, modules, 1);
}

VkPipeline
create_tbo_pipeline(VkRenderPass render_pass, VkPipelineLayout layout)
{
   VkShaderModule modules[5] = {
      create_shader_module(basic_vert_spirv, basic_vert_spirv_len),
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      create_shader_module(tbo_frag_spirv, tbo_frag_spirv_len),
   };

   return create_pipeline_helper(layout, render_pass, modules, 1);
}

VkPipeline
create_tbo_many_pipeline(VkRenderPass render_pass, VkPipelineLayout layout)
{
   VkShaderModule modules[5] = {
      create_shader_module(basic_vert_spirv, basic_vert_spirv_len),
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      create_shader_module(tbo_many_frag_spirv, tbo_many_frag_spirv_len),
   };

   return create_pipeline_helper(layout, render_pass, modules, 1);
}

VkPipeline
create_ibo_pipeline(VkRenderPass render_pass, VkPipelineLayout layout)
{
   VkShaderModule modules[5] = {
      create_shader_module(basic_vert_spirv, basic_vert_spirv_len),
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      create_shader_module(ibo_frag_spirv, ibo_frag_spirv_len),
   };

   return create_pipeline_helper(layout, render_pass, modules, 1);
}

VkPipeline
create_ibo_many_pipeline(VkRenderPass render_pass, VkPipelineLayout layout)
{
   unsigned char *spirv = ibo_many4_frag_spirv;
   unsigned len = ibo_many4_frag_spirv_len;
   if (dev->info.props.limits.maxPerStageDescriptorStorageImages > 4) {
      spirv = ibo_many8_frag_spirv;
      len = ibo_many8_frag_spirv_len;
   }
   VkShaderModule modules[5] = {
      create_shader_module(basic_vert_spirv, basic_vert_spirv_len),
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      create_shader_module(spirv, len),
   };

   return create_pipeline_helper(layout, render_pass, modules, 1);
}

VkPipeline
create_ssbo_pipeline(VkRenderPass render_pass, VkPipelineLayout layout)
{
   VkShaderModule modules[5] = {
      create_shader_module(basic_vert_spirv, basic_vert_spirv_len),
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      create_shader_module(ssbo_frag_spirv, ssbo_frag_spirv_len),
   };

   return create_pipeline_helper(layout, render_pass, modules, 1);
}

VkPipeline
create_ssbo_many_pipeline(VkRenderPass render_pass, VkPipelineLayout layout)
{
   VkShaderModule modules[5] = {
      create_shader_module(basic_vert_spirv, basic_vert_spirv_len),
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      create_shader_module(ssbo_many_frag_spirv, ssbo_many_frag_spirv_len),
   };

   return create_pipeline_helper(layout, render_pass, modules, 1);
}

void
create_vattrib_pipelines(VkRenderPass render_pass, VkPipelineLayout layout, VkPipeline *pipelines)
{
   VkShaderModule modules[5] = {
      create_shader_module(vattrib_vert_spirv, vattrib_vert_spirv_len),
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      create_shader_module(basic_frag_spirv, basic_frag_spirv_len),
   };
   assert(layout);

   VkVertexInputBindingDescription vbinding[16];
   VkVertexInputAttributeDescription vattr[16];
   VkPipelineVertexInputStateCreateInfo vertex_input_state = {0};
   vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
   vertex_input_state.pVertexBindingDescriptions = vbinding;
   vertex_input_state.vertexBindingDescriptionCount = 16;
   vertex_input_state.pVertexAttributeDescriptions = vattr;
   vertex_input_state.vertexAttributeDescriptionCount = 16;

   for (unsigned i = 0; i < 4; i++) {
      generate_vattribs(vbinding, vattr, 16);
      pipelines[i] = create_pipeline(layout, render_pass, modules, &vertex_input_state, 1, false, NULL);
   }
}

VkPipeline
create_vattrib_pipeline_dynamic(VkRenderPass render_pass, VkPipelineLayout layout)
{
   VkShaderModule modules[5] = {
      create_shader_module(vattrib_vert_spirv, vattrib_vert_spirv_len),
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      create_shader_module(basic_frag_spirv, basic_frag_spirv_len),
   };

   VkVertexInputBindingDescription vbinding[16];
   VkVertexInputAttributeDescription vattr[16];
   VkPipelineVertexInputStateCreateInfo vertex_input_state = {0};
   vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
   vertex_input_state.pVertexBindingDescriptions = vbinding;
   vertex_input_state.vertexBindingDescriptionCount = 16;
   vertex_input_state.pVertexAttributeDescriptions = vattr;
   vertex_input_state.vertexAttributeDescriptionCount = 16;
   generate_vattribs(vbinding, vattr, 16);

   return create_pipeline(layout, render_pass, modules, &vertex_input_state, 1, true, NULL);
}

VkPipeline
create_gpl_basic_pipeline(VkRenderPass render_pass, VkPipelineLayout layout)
{
   VkShaderModule modules[5] = {
      create_shader_module(vattrib_vert_spirv, vattrib_vert_spirv_len),
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      create_shader_module(basic_frag_spirv, basic_frag_spirv_len),
   };

   VkGraphicsPipelineLibraryCreateInfoEXT gplci = {
      VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT,
      NULL,
      VK_GRAPHICS_PIPELINE_LIBRARY_PRE_RASTERIZATION_SHADERS_BIT_EXT | VK_GRAPHICS_PIPELINE_LIBRARY_FRAGMENT_SHADER_BIT_EXT | VK_GRAPHICS_PIPELINE_LIBRARY_FRAGMENT_OUTPUT_INTERFACE_BIT_EXT
   };

   return create_pipeline(layout, render_pass, modules, NULL, 1, false, &gplci);
}

VkPipeline
create_gpl_vert_pipeline(VkRenderPass render_pass, VkPipelineLayout layout, VkPipelineVertexInputStateCreateInfo *vertex_input_state)
{
   VkVertexInputBindingDescription *vattrs = (void*)vertex_input_state->pVertexBindingDescriptions;
   VkVertexInputAttributeDescription *vbindings = (void*)vertex_input_state->pVertexAttributeDescriptions;
   generate_vattribs(vattrs, vbindings, 16);

   VkGraphicsPipelineLibraryCreateInfoEXT gplci = {
      VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT,
      NULL,
      VK_GRAPHICS_PIPELINE_LIBRARY_VERTEX_INPUT_INTERFACE_BIT_EXT
   };

   return create_pipeline(layout, render_pass, NULL, vertex_input_state, 1, false, &gplci);
}
