static VkPipeline
pso_6d50553122782d72(VkPipelineLayout layout, VkRenderPass renderPass, void *shader_ptr)
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
   dynamicState_VkDynamicState[0] = 1000267002;
   dynamicState.pDynamicStates = dynamicState_VkDynamicState;
   pci.pDynamicState = &dynamicState;

   VkPipelineVertexInputStateCreateInfo vertexInputState = {0};
   vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
   vertexInputState.flags = 0;
   vertexInputState.vertexAttributeDescriptionCount = 13;
   VkVertexInputAttributeDescription vertexInputState_VkVertexInputAttributeDescription[13];
   vertexInputState_VkVertexInputAttributeDescription[0] = (VkVertexInputAttributeDescription){0, 0, 103, 0};
   vertexInputState_VkVertexInputAttributeDescription[1] = (VkVertexInputAttributeDescription){1, 1, 109, 0};
   vertexInputState_VkVertexInputAttributeDescription[2] = (VkVertexInputAttributeDescription){2, 1, 109, 16};
   vertexInputState_VkVertexInputAttributeDescription[3] = (VkVertexInputAttributeDescription){3, 1, 37, 32};
   vertexInputState_VkVertexInputAttributeDescription[4] = (VkVertexInputAttributeDescription){4, 1, 109, 36};
   vertexInputState_VkVertexInputAttributeDescription[5] = (VkVertexInputAttributeDescription){5, 1, 109, 52};
   vertexInputState_VkVertexInputAttributeDescription[6] = (VkVertexInputAttributeDescription){6, 1, 109, 68};
   vertexInputState_VkVertexInputAttributeDescription[7] = (VkVertexInputAttributeDescription){7, 1, 109, 84};
   vertexInputState_VkVertexInputAttributeDescription[8] = (VkVertexInputAttributeDescription){8, 1, 109, 100};
   vertexInputState_VkVertexInputAttributeDescription[9] = (VkVertexInputAttributeDescription){9, 1, 37, 120};
   vertexInputState_VkVertexInputAttributeDescription[10] = (VkVertexInputAttributeDescription){10, 1, 37, 116};
   vertexInputState_VkVertexInputAttributeDescription[11] = (VkVertexInputAttributeDescription){11, 1, 106, 124};
   vertexInputState_VkVertexInputAttributeDescription[12] = (VkVertexInputAttributeDescription){12, 1, 106, 136};
   vertexInputState.pVertexAttributeDescriptions = vertexInputState_VkVertexInputAttributeDescription;
   vertexInputState.vertexBindingDescriptionCount = 3;
   VkVertexInputBindingDescription vertexInputState_VkVertexInputBindingDescription[3];
   vertexInputState_VkVertexInputBindingDescription[0] = (VkVertexInputBindingDescription){0, 8, 0};
   vertexInputState_VkVertexInputBindingDescription[1] = (VkVertexInputBindingDescription){1, 148, 1};
   vertexInputState_VkVertexInputBindingDescription[2] = (VkVertexInputBindingDescription){2, 0, 1};
   vertexInputState.pVertexBindingDescriptions = vertexInputState_VkVertexInputBindingDescription;
   pci.pVertexInputState = &vertexInputState;

   VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {0};
   inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
   inputAssemblyState.flags = 0;
   inputAssemblyState.topology = 3;
   inputAssemblyState.primitiveRestartEnable = 0;
   pci.pInputAssemblyState = &inputAssemblyState;

   VkGraphicsPipelineLibraryCreateInfoEXT GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT = {0};
   GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT.pNext = (void*)pci.pNext;
   pci.pNext = &GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT;
   GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT.sType = 1000320002;
   GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT.flags = 1;

   VkPipeline pipeline;
   VK(CreateGraphicsPipelines)(dev->dev, VK_NULL_HANDLE, 1, &pci, NULL, &pipeline);
   return pipeline;
}
