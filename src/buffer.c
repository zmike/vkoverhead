/*
 * Copyright 2022 Valve Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHOR(S) AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include "vkoverhead.h"

static VkDeviceMemory
create_memory(VkDeviceSize size, uint32_t memoryTypeBits, unsigned alignment, bool host)
{
   VkMemoryAllocateInfo mai = {0};
   mai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
   mai.allocationSize = size;

   VkMemoryAllocateFlagsInfo ai = {0};
   ai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
   ai.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;
   if (dev->info.have_KHR_buffer_device_address)
      mai.pNext = &ai;
   mai.memoryTypeIndex = host ? dev->host_mem_idx : dev->vram_mem_idx;
   if (!(memoryTypeBits & BITFIELD_BIT(mai.memoryTypeIndex))) {
      VkMemoryPropertyFlags domains = host ? (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      for (unsigned i = 0; i < dev->info.mem_props.memoryTypeCount; i++) {
         if ((dev->info.mem_props.memoryTypes[i].propertyFlags & domains) == domains && BITFIELD_BIT(i) & memoryTypeBits) {
            mai.memoryTypeIndex = i;
            break;
         }
      }
   }
   if (!(memoryTypeBits & BITFIELD_BIT(mai.memoryTypeIndex))) {
      fprintf(stderr, "Could not successfully allocate described memory type\n");
      abort();
   }
   if (host) {
      alignment = MAX2(alignment, dev->info.props.limits.minMemoryMapAlignment);
      mai.allocationSize = align64(mai.allocationSize, dev->info.props.limits.minMemoryMapAlignment);
   }
   VkDeviceMemory mem;
   VkResult result = VK(AllocateMemory)(dev->dev, &mai, NULL, &mem);
   VK_CHECK("AllocateMemory", result);
   return mem;
}

static VkBuffer
create_buffer(VkDeviceSize size, VkBufferUsageFlags usage)
{
   VkBufferCreateInfo bci = {0};
   bci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
   bci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
   bci.size = size;
   bci.usage = usage;
   if (dev->info.have_KHR_buffer_device_address)
      bci.usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

   VkBuffer buffer;
   VkResult result = VK(CreateBuffer)(dev->dev, &bci, NULL, &buffer);
   VK_CHECK("CreateBuffer", result);
   return buffer;
}

static VkBuffer
create_buffer_bind_mem(VkDeviceSize size, VkBufferUsageFlags usage, bool host, VkDeviceMemory *mem)
{
   VkBuffer buffer = create_buffer(size, usage);
   VkMemoryRequirements reqs = {0};
   VK(GetBufferMemoryRequirements)(dev->dev, buffer, &reqs);
   unsigned alignment;
   if (host)
      alignment = MAX2(reqs.alignment, dev->info.props.limits.minMemoryMapAlignment);
   else
      alignment = MAX2(reqs.alignment, 256);
   *mem = create_memory(reqs.size, reqs.memoryTypeBits, alignment, host);
   VkResult result = VK(BindBufferMemory)(dev->dev, buffer, *mem, 0);
   VK_CHECK("BindBufferMemory", result);
   return buffer;
}

static VkBuffer
create_buffer_bind(VkDeviceSize size, VkBufferUsageFlags usage, bool host)
{
   VkDeviceMemory mem;
   return create_buffer_bind_mem(size, usage, host, &mem);
}

VkBuffer
create_vertex_buffer(void)
{
   float vec4[4];
   /* big enough to handle some randomization */
   VkBuffer buffer = create_buffer_bind(sizeof(vec4) * 16, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);
   return buffer;
}

VkBuffer
create_uniform_buffer(void)
{
   VkBuffer buffer = create_buffer_bind(BUFFER_SIZE, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);
   return buffer;
}

VkBuffer
create_index_buffer(void)
{
   float vec4[4];
   VkBuffer buffer = create_buffer_bind(sizeof(vec4) * 2, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);
   return buffer;
}

VkBuffer
create_texel_buffer(void)
{
   VkBuffer buffer = create_buffer_bind(BUFFER_SIZE, VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);
   return buffer;
}

VkBuffer
create_image_buffer(void)
{
   VkBuffer buffer = create_buffer_bind(BUFFER_SIZE, VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);
   return buffer;
}

VkBuffer
create_storage_buffer(void)
{
   VkBuffer buffer = create_buffer_bind(BUFFER_SIZE, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);
   return buffer;
}

VkBuffer
create_copy_buffer(VkDeviceMemory *mem)
{
   VkBuffer buffer = create_buffer_bind_mem(256 * 1024 * 1024, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false, mem);
   return buffer;
}

VkBuffer
create_descriptor_buffer(void)
{
   VkBufferUsageFlags usage = VK_BUFFER_USAGE_SAMPLER_DESCRIPTOR_BUFFER_BIT_EXT | VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT;
   VkBuffer buffer = create_buffer_bind(DESCRIPTOR_BUFFER_SIZE, usage, true);
   return buffer;
}

VkBufferView
create_bufferview(VkBuffer buffer)
{
   VkBufferViewCreateInfo bci = {0};
   bci.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
   bci.buffer = buffer;
   bci.format = VK_FORMAT_R32G32B32A32_SFLOAT;
   bci.range = VK_WHOLE_SIZE;
   VkBufferView bufferview;
   VkResult result = VK(CreateBufferView)(dev->dev, &bci, NULL, &bufferview);
   VK_CHECK("CreateBufferView", result);
   return bufferview;
}

static VkImage
create_image(VkImageUsageFlags usage, VkSampleCountFlags samples, bool mutable)
{
   VkImageCreateInfo ici = {0};
   ici.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
   if (mutable)
      ici.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
   ici.imageType = VK_IMAGE_TYPE_2D;
   ici.format = VK_FORMAT_R32G32B32A32_SFLOAT;
   ici.extent.width = 100;
   ici.extent.height = 100;
   ici.extent.depth = 1;
   ici.mipLevels = 1;
   ici.arrayLayers = 1;
   ici.samples = MAX2(samples, VK_SAMPLE_COUNT_1_BIT);
   ici.tiling = VK_IMAGE_TILING_OPTIMAL;
   ici.usage = usage;

   VkImage image;
   VkResult result = VK(CreateImage)(dev->dev, &ici, NULL, &image);
   VK_CHECK("CreateImage", result);
   return image;
}

static VkImage
create_image_bind(VkImageUsageFlags usage, VkSampleCountFlags samples, bool mutable)
{
   VkImage image = create_image(usage, samples, mutable);
   VkMemoryRequirements reqs = {0};
   VK(GetImageMemoryRequirements)(dev->dev, image, &reqs);
   unsigned alignment = MAX2(reqs.alignment, 256);
   VkDeviceMemory mem = create_memory(reqs.size, reqs.memoryTypeBits, alignment, false);
   VkResult result = VK(BindImageMemory)(dev->dev, image, mem, 0);
   VK_CHECK("BindImageMemory", result);
   return image;
}

static VkImageView
create_image_helper(VkImage *ret_image, VkImageUsageFlags usage, VkSampleCountFlags samples, bool mutable)
{
   VkImage image = create_image_bind(usage, samples, mutable);
   VkImageViewCreateInfo ivci = {0};
   ivci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
   ivci.image = image;
   ivci.viewType = VK_IMAGE_VIEW_TYPE_2D;
   ivci.format = VK_FORMAT_R32G32B32A32_SFLOAT;
   ivci.components.r = VK_COMPONENT_SWIZZLE_R;
   ivci.components.g = VK_COMPONENT_SWIZZLE_G;
   ivci.components.b = VK_COMPONENT_SWIZZLE_B;
   ivci.components.a = VK_COMPONENT_SWIZZLE_A;
   ivci.subresourceRange = default_subresourcerange();

   VkImageView image_view;
   VkResult result = VK(CreateImageView)(dev->dev, &ivci, NULL, &image_view);
   VK_CHECK("CreateImageView", result);
   if (ret_image)
      *ret_image = image;
   return image_view;
}

VkImageView
create_rt(VkImage *ret_image, bool mutable)
{
   return create_image_helper(ret_image, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, 0, mutable);
}

VkImageView
create_rt_ms(VkImage *ret_image, VkSampleCountFlags samples, bool mutable)
{
   return create_image_helper(ret_image, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, samples, mutable);
}

VkImageView
create_tex(VkImage *ret_image)
{
   return create_image_helper(ret_image, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 0, false);
}

VkImageView
create_storage_image(VkImage *ret_image)
{
   return create_image_helper(ret_image, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT, 0, false);
}
