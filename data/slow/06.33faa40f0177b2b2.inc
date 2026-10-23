static VkPipeline
pso_33faa40f0177b2b2(VkPipelineLayout layout, VkRenderPass renderPass, void *shader_ptr)
{
   VkGraphicsPipelineCreateInfo pci = {0};
   pci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
   pci.flags = 0;
   pci.layout = layout;
   pci.renderPass = renderPass;
   pci.subpass = 0;
   pci.stageCount = 0;
   VkPipelineLibraryCreateInfoKHR PIPELINE_LIBRARY_CREATE_INFO_KHR = {0};
   PIPELINE_LIBRARY_CREATE_INFO_KHR.pNext = (void*)pci.pNext;
   pci.pNext = &PIPELINE_LIBRARY_CREATE_INFO_KHR;
   PIPELINE_LIBRARY_CREATE_INFO_KHR.sType = 1000290000;
   PIPELINE_LIBRARY_CREATE_INFO_KHR.libraryCount = 4;
   PIPELINE_LIBRARY_CREATE_INFO_KHR.pLibraries = shader_ptr;

   VkPipeline pipeline;
   VK(CreateGraphicsPipelines)(dev->dev, VK_NULL_HANDLE, 1, &pci, NULL, &pipeline);
   return pipeline;
}
