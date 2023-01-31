static VkPipeline
pso_022584b7c03a4127(VkPipelineLayout layout, VkRenderPass renderPass, void *shader_ptr)
{
   VkGraphicsPipelineCreateInfo pci = {0};
   pci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
   pci.flags = 8390656;
   pci.layout = layout;
   pci.renderPass = renderPass;
   pci.subpass = 0;
   VkPipelineDynamicStateCreateInfo dynamicState = {0};
   dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
   dynamicState.flags = 0;
   dynamicState.dynamicStateCount = 17;
   VkDynamicState dynamicState_VkDynamicState[17];
   dynamicState_VkDynamicState[0] = 3;
   dynamicState_VkDynamicState[1] = 6;
   dynamicState_VkDynamicState[2] = 7;
   dynamicState_VkDynamicState[3] = 8;
   dynamicState_VkDynamicState[4] = 4;
   dynamicState_VkDynamicState[5] = 1000267003;
   dynamicState_VkDynamicState[6] = 1000267004;
   dynamicState_VkDynamicState[7] = 1000267000;
   dynamicState_VkDynamicState[8] = 1000267001;
   dynamicState_VkDynamicState[9] = 1000267006;
   dynamicState_VkDynamicState[10] = 1000267007;
   dynamicState_VkDynamicState[11] = 1000267008;
   dynamicState_VkDynamicState[12] = 1000267010;
   dynamicState_VkDynamicState[13] = 1000267011;
   dynamicState_VkDynamicState[14] = 1000267002;
   dynamicState_VkDynamicState[15] = 5;
   dynamicState_VkDynamicState[16] = 1000267009;
   dynamicState.pDynamicStates = dynamicState_VkDynamicState;
   pci.pDynamicState = &dynamicState;

   VkPipelineDepthStencilStateCreateInfo depthStencilState = {0};
   depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
   depthStencilState.flags = 0;
   depthStencilState.stencilTestEnable = 0;
   depthStencilState.maxDepthBounds = 0.000000;
   depthStencilState.minDepthBounds = 0.000000;
   depthStencilState.depthBoundsTestEnable = 0;
   depthStencilState.depthWriteEnable = 0;
   depthStencilState.depthTestEnable = 0;
   depthStencilState.depthCompareOp = 0;
   depthStencilState.front = (VkStencilOpState){0, 0, 0, 0, 0, 0, 0};
   depthStencilState.back = (VkStencilOpState){0, 0, 0, 0, 0, 0, 0};
   pci.pDepthStencilState = &depthStencilState;

   pci.stageCount = 0;
   VkPipelineRenderingCreateInfo PIPELINE_RENDERING_CREATE_INFO = {0};
   PIPELINE_RENDERING_CREATE_INFO.pNext = (void*)pci.pNext;
   pci.pNext = &PIPELINE_RENDERING_CREATE_INFO;
   PIPELINE_RENDERING_CREATE_INFO.sType = 1000044002;
   PIPELINE_RENDERING_CREATE_INFO.depthAttachmentFormat = 0;
   PIPELINE_RENDERING_CREATE_INFO.stencilAttachmentFormat = 0;
   PIPELINE_RENDERING_CREATE_INFO.viewMask = 0;

   VkGraphicsPipelineLibraryCreateInfoEXT GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT = {0};
   GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT.pNext = (void*)pci.pNext;
   pci.pNext = &GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT;
   GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT.sType = 1000320002;
   GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT.flags = 4;

   VkPipeline pipeline;
   VK(CreateGraphicsPipelines)(dev->dev, VK_NULL_HANDLE, 1, &pci, NULL, &pipeline);
   return pipeline;
}
