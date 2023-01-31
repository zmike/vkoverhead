static VkSampler
sampler_0bf53ea75ea13dcf(void)
{
   VkSamplerCreateInfo samplerCreateInfo = {0};
   samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
   samplerCreateInfo.flags = 0;
   samplerCreateInfo.minFilter = 1;
   samplerCreateInfo.magFilter = 1;
   samplerCreateInfo.maxAnisotropy = 1.000000;
   samplerCreateInfo.compareOp = 1;
   samplerCreateInfo.anisotropyEnable = 0;
   samplerCreateInfo.mipmapMode = 1;
   samplerCreateInfo.addressModeU = 2;
   samplerCreateInfo.addressModeV = 2;
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
