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

VkDescriptorSetLayout
create_descriptor_layout(VkDescriptorSetLayoutBinding *bindings, unsigned num_bindings)
{
   VkDescriptorSetLayout desc_layout;
   VkDescriptorSetLayoutCreateInfo dcslci = {0};
   dcslci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
   dcslci.pNext = NULL;
   VkDescriptorSetLayoutBindingFlagsCreateInfo fci = {0};
   VkDescriptorBindingFlags flags[128] = {0};
   dcslci.pNext = &fci;
   fci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
   fci.bindingCount = num_bindings;
   fci.pBindingFlags = flags;
   dcslci.bindingCount = num_bindings;
   dcslci.pBindings = bindings;
   VkDescriptorSetLayoutSupport supp;
   supp.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_SUPPORT;
   supp.pNext = NULL;
   supp.supported = VK_FALSE;
   VK(GetDescriptorSetLayoutSupport)(dev->dev, &dcslci, &supp);
   if (supp.supported == VK_FALSE) {
      fprintf(stderr, "vkGetDescriptorSetLayoutSupport claims layout is unsupported\n");
      abort();
   }
   VkResult result = VK(CreateDescriptorSetLayout)(dev->dev, &dcslci, 0, &desc_layout);
   VK_CHECK("CreateDescriptorSetLayout", result);
   return desc_layout;
}

static VkDescriptorPool
create_descriptor_pool(VkDescriptorPoolSize *sizes, unsigned num_sizes)
{
   VkDescriptorPoolCreateInfo dpci = {0};
   dpci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
   dpci.pPoolSizes = sizes;
   dpci.poolSizeCount = num_sizes;
   dpci.maxSets = 1;
   VkDescriptorPool pool;
   VkResult result = VK(CreateDescriptorPool)(dev->dev, &dpci, 0, &pool);
   VK_CHECK("CreateDescriptorPool", result);
   return pool;
}

VkDescriptorSet
create_descriptor_set(VkDescriptorSetLayout desc_layout, VkDescriptorPoolSize *size)
{
   VkDescriptorSetAllocateInfo dsai = {0};
   dsai.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
   dsai.descriptorPool = create_descriptor_pool(size, 1);
   dsai.descriptorSetCount = 1;
   dsai.pSetLayouts = &desc_layout;

   VkDescriptorSet desc_set;
   VkResult result = VK(AllocateDescriptorSets)(dev->dev, &dsai, &desc_set);
   VK_CHECK("AllocateDescriptorSets", result);
   return desc_set;
}
