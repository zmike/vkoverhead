static VkDescriptorSetLayout
dsl_cb3985cfac50f434(void)
{
   VkDescriptorSetLayoutCreateInfo dslci = {0};
   dslci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
   dslci.flags = 0;
   dslci.bindingCount = 1;
   VkDescriptorSetLayoutBinding dslci_VkDescriptorSetLayoutBinding[1];
   dslci_VkDescriptorSetLayoutBinding[0].descriptorType = 6;
   dslci_VkDescriptorSetLayoutBinding[0].descriptorCount = 1;
   dslci_VkDescriptorSetLayoutBinding[0].stageFlags = 1;
   dslci_VkDescriptorSetLayoutBinding[0].binding = 0;
   dslci_VkDescriptorSetLayoutBinding[0].pImmutableSamplers = NULL;
   dslci.pBindings = dslci_VkDescriptorSetLayoutBinding;
   VkDescriptorSetLayout dsl;
   VK(CreateDescriptorSetLayout)(dev->dev, &dslci, 0, &dsl);
   return dsl;
}
