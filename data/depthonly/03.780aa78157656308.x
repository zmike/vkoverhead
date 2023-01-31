static VkPipelineLayout
layout_780aa78157656308(VkDescriptorSetLayout *dsl)
{
   VkPipelineLayoutCreateInfo layoutCreateInfo = {0};
   layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
   layoutCreateInfo.flags = 2;
   layoutCreateInfo.pushConstantRangeCount = 1;
   VkPushConstantRange layoutCreateInfo_VkPushConstantRange[1];
   layoutCreateInfo_VkPushConstantRange[0] = (VkPushConstantRange){17, 0, 128};
   layoutCreateInfo.pPushConstantRanges = layoutCreateInfo_VkPushConstantRange;
   layoutCreateInfo.setLayoutCount = 4;
   layoutCreateInfo.pSetLayouts = dsl;
   VkPipelineLayout layout;
   VK(CreatePipelineLayout)(dev->dev, &layoutCreateInfo, 0, &layout);
   return layout;
}
