static VkSampler
sampler_48465701329d9e2c(void)
{
   VkSamplerCreateInfo samplerCreateInfo = {0};
   samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
   samplerCreateInfo.flags = 0;
   samplerCreateInfo.minFilter = 0;
   samplerCreateInfo.magFilter = 0;
   samplerCreateInfo.maxAnisotropy = 1.000000;
   samplerCreateInfo.compareOp = 1;
   samplerCreateInfo.anisotropyEnable = 0;
   samplerCreateInfo.mipmapMode = 0;
   samplerCreateInfo.addressModeU = 0;
   samplerCreateInfo.addressModeV = 0;
   samplerCreateInfo.addressModeW = 0;
   samplerCreateInfo.borderColor = 0;
   samplerCreateInfo.unnormalizedCoordinates = 0;
   samplerCreateInfo.compareEnable = 0;
   samplerCreateInfo.mipLodBias = 0.000000;
   samplerCreateInfo.minLod = 0.000000;
   samplerCreateInfo.maxLod = 15.000000;
   VkSampler sampler;
   VK(CreateSampler)(dev->dev, &samplerCreateInfo, 0, &sampler);
   return sampler;
}
