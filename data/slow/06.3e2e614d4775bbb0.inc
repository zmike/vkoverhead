static VkPipeline
pso_3e2e614d4775bbb0(VkPipelineLayout layout, VkRenderPass renderPass, void *shader_ptr)
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
   dynamicState.dynamicStateCount = 1;
   VkDynamicState dynamicState_VkDynamicState[1];
   dynamicState_VkDynamicState[0] = 4;
   dynamicState.pDynamicStates = dynamicState_VkDynamicState;
   pci.pDynamicState = &dynamicState;

   VkPipelineMultisampleStateCreateInfo multisampleState = {0};
   multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
   multisampleState.flags = 0;
   multisampleState.rasterizationSamples = 1;
   multisampleState.sampleShadingEnable = 0;
   multisampleState.minSampleShading = 0.000000;
   multisampleState.alphaToOneEnable = 0;
   multisampleState.alphaToCoverageEnable = 0;
   VkSampleMask multisampleState_VkSampleMask[1];
   multisampleState_VkSampleMask[0] = 4294967295;
   multisampleState.pSampleMask = multisampleState_VkSampleMask;
   pci.pMultisampleState = &multisampleState;

   VkPipelineColorBlendStateCreateInfo colorBlendState = {0};
   colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
   colorBlendState.flags = 0;
   colorBlendState.logicOp = 3;
   colorBlendState.logicOpEnable = 0;
   colorBlendState.blendConstants[0] = 0.000000;
   colorBlendState.blendConstants[1] = 0.000000;
   colorBlendState.blendConstants[2] = 0.000000;
   colorBlendState.blendConstants[3] = 0.000000;
   colorBlendState.attachmentCount = 1;
   VkPipelineColorBlendAttachmentState colorBlendState_VkPipelineColorBlendAttachmentState[1];
   colorBlendState_VkPipelineColorBlendAttachmentState[0] = (VkPipelineColorBlendAttachmentState){1, 1, 7, 0, 1, 1, 0, 15};
   colorBlendState.pAttachments = colorBlendState_VkPipelineColorBlendAttachmentState;
   pci.pColorBlendState = &colorBlendState;

   VkPipelineRenderingCreateInfo PIPELINE_RENDERING_CREATE_INFO = {0};
   PIPELINE_RENDERING_CREATE_INFO.pNext = (void*)pci.pNext;
   pci.pNext = &PIPELINE_RENDERING_CREATE_INFO;
   PIPELINE_RENDERING_CREATE_INFO.sType = 1000044002;
   PIPELINE_RENDERING_CREATE_INFO.depthAttachmentFormat = 0;
   PIPELINE_RENDERING_CREATE_INFO.stencilAttachmentFormat = 0;
   PIPELINE_RENDERING_CREATE_INFO.viewMask = 0;
   PIPELINE_RENDERING_CREATE_INFO.colorAttachmentCount = 1;
   VkFormat PIPELINE_RENDERING_CREATE_INFO_VkFormat[1];
   PIPELINE_RENDERING_CREATE_INFO_VkFormat[0] = VK_FORMAT_B8G8R8A8_SRGB;
   PIPELINE_RENDERING_CREATE_INFO.pColorAttachmentFormats = PIPELINE_RENDERING_CREATE_INFO_VkFormat;

   VkGraphicsPipelineLibraryCreateInfoEXT GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT = {0};
   GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT.pNext = (void*)pci.pNext;
   pci.pNext = &GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT;
   GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT.sType = 1000320002;
   GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT.flags = 8;

   VkPipeline pipeline;
   VK(CreateGraphicsPipelines)(dev->dev, VK_NULL_HANDLE, 1, &pci, NULL, &pipeline);
   return pipeline;
}
