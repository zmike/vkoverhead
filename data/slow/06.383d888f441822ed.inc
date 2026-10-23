static VkPipeline
pso_383d888f441822ed(VkPipelineLayout layout, VkRenderPass renderPass, void *shader_ptr)
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

   VkPipelineRasterizationStateCreateInfo rasterizationState = {0};
   rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
   rasterizationState.flags = 0;
   rasterizationState.depthBiasConstantFactor = 0.000000;
   rasterizationState.depthBiasSlopeFactor = 0.000000;
   rasterizationState.depthBiasClamp = 0.000000;
   rasterizationState.depthBiasEnable = 1;
   rasterizationState.depthClampEnable = 1;
   rasterizationState.polygonMode = 0;
   rasterizationState.rasterizerDiscardEnable = 0;
   rasterizationState.frontFace = 0;
   rasterizationState.lineWidth = 1.000000;
   rasterizationState.cullMode = 0;
   pci.pRasterizationState = &rasterizationState;

   VkPipelineViewportStateCreateInfo viewportState = {0};
   viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
   viewportState.flags = 0;
   viewportState.viewportCount = 0;
   viewportState.scissorCount = 0;
   pci.pViewportState = &viewportState;

   pci.stageCount = 1;
VkShaderModule *modules = (VkShaderModule *)shader_ptr;
   VkPipelineShaderStageCreateInfo pci_VkPipelineShaderStageCreateInfo[1];
   pci_VkPipelineShaderStageCreateInfo[0].pNext = NULL;
   pci_VkPipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
   pci_VkPipelineShaderStageCreateInfo[0].flags = 0;
   pci_VkPipelineShaderStageCreateInfo[0].pName = "MainVs";
   pci_VkPipelineShaderStageCreateInfo[0].module = modules[0];
   pci_VkPipelineShaderStageCreateInfo[0].stage = 1;
   pci_VkPipelineShaderStageCreateInfo[0].pSpecializationInfo = NULL;
   pci.pStages = pci_VkPipelineShaderStageCreateInfo;
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
   GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT.flags = 2;

   VkPipeline pipeline;
   VK(CreateGraphicsPipelines)(dev->dev, VK_NULL_HANDLE, 1, &pci, NULL, &pipeline);
   return pipeline;
}
