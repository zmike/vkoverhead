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
create_memory(VkDeviceSize size, unsigned alignment, bool host)
{
   VkMemoryAllocateInfo mai = {0};
   mai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
   mai.allocationSize = size;
   mai.memoryTypeIndex = host ? dev->host_mem_idx : dev->vram_mem_idx;
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

   VkBuffer buffer;
   VkResult result = VK(CreateBuffer)(dev->dev, &bci, NULL, &buffer);
   VK_CHECK("CreateBuffer", result);
   return buffer;
}

static VkBuffer
create_buffer_bind(VkDeviceSize size, VkBufferUsageFlags usage, bool host)
{
   VkBuffer buffer = create_buffer(size, usage);
   VkMemoryRequirements reqs = {0};
   VK(GetBufferMemoryRequirements)(dev->dev, buffer, &reqs);
   unsigned alignment;
   if (host)
      alignment = MAX2(reqs.alignment, dev->info.props.limits.minMemoryMapAlignment);
   else
      alignment = MAX2(reqs.alignment, 256);
   VkDeviceMemory mem = create_memory(reqs.size, alignment, host);
   VkResult result = VK(BindBufferMemory)(dev->dev, buffer, mem, 0);
   VK_CHECK("BindBufferMemory", result);
   return buffer;
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
   float vec4[4];
   VkBuffer buffer = create_buffer_bind(sizeof(vec4) * 2, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);
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
   float vec4[4];
   VkBuffer buffer = create_buffer_bind(sizeof(vec4), VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);
   return buffer;
}

VkBuffer
create_image_buffer(void)
{
   float vec4[4];
   VkBuffer buffer = create_buffer_bind(sizeof(vec4), VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);
   return buffer;
}

VkBuffer
create_storage_buffer(void)
{
   float vec4[4];
   VkBuffer buffer = create_buffer_bind(sizeof(vec4) * 2, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);
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
create_image(VkImageUsageFlags usage)
{
   VkImageCreateInfo ici = {0};
   ici.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
   ici.imageType = VK_IMAGE_TYPE_2D;
   ici.format = VK_FORMAT_R32G32B32A32_SFLOAT;
   ici.extent.width = 100;
   ici.extent.height = 100;
   ici.extent.depth = 1;
   ici.mipLevels = 1;
   ici.arrayLayers = 1;
   ici.samples = VK_SAMPLE_COUNT_1_BIT;
   ici.tiling = VK_IMAGE_TILING_OPTIMAL;
   ici.usage = usage;

   VkImage image;
   VkResult result = VK(CreateImage)(dev->dev, &ici, NULL, &image);
   VK_CHECK("CreateImage", result);
   return image;
}

static VkImage
create_image_bind(VkImageUsageFlags usage)
{
   VkImage image = create_image(usage);
   VkMemoryRequirements reqs = {0};
   VK(GetImageMemoryRequirements)(dev->dev, image, &reqs);
   unsigned alignment = MAX2(reqs.alignment, 256);
   VkDeviceMemory mem = create_memory(reqs.size, alignment, false);
   VkResult result = VK(BindImageMemory)(dev->dev, image, mem, 0);
   VK_CHECK("BindImageMemory", result);
   return image;
}

static VkImageView
create_image_helper(VkImage *ret_image, VkImageUsageFlags usage)
{
   VkImage image = create_image_bind(usage);
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
create_rt(VkImage *ret_image)
{
   return create_image_helper(ret_image, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
}

VkImageView
create_tex(VkImage *ret_image)
{
   return create_image_helper(ret_image, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
}

VkImageView
create_storage_image(VkImage *ret_image)
{
   return create_image_helper(ret_image, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT);
}