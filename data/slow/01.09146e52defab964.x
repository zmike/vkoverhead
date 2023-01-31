static VkSampler
sampler_09146e52defab964(void)
{
   VkSamplerCreateInfo samplerCreateInfo = {0};
   samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
   samplerCreateInfo.flags = 0;
   samplerCreateInfo.minFilter = 1;
   samplerCreateInfo.magFilter = 1;
   samplerCreateInfo.maxAnisotropy = 1.000000;
   samplerCreateInfo.compareOp = 1;
   samplerCreateInfo.anisotropyEnable = 0;
   samplerCreateInfo.mipmapMode = 0;
   samplerCreateInfo.addressModeU = 3;
   samplerCreateInfo.addressModeV = 3;
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
