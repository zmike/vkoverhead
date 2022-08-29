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

#ifndef VK_OVERHEAD_H
#define VK_OVERHEAD_H

#include "vk_device.h"
#include "util/u_math.h"

extern struct vk_device *dev;

#define MAX_RTS 8 //technically spec only mandates 4, but cmon.
#define MAX_UBOS 12 //maxPerStageDescriptorUniformBuffers minimum
#define MAX_SAMPLERS 16 //maxPerStageDescriptorSamplers minimum
#define MAX_SSBOS 8 //technically spec only mandates 4, but cmon.
#define MAX_IMAGES 8 //spec mandates 4, and v3dv only has 4, so use detection

VkDescriptorSetLayout create_descriptor_layout(VkDescriptorSetLayoutBinding *bindings, unsigned num_bindings);
VkDescriptorSet create_descriptor_set(VkDescriptorSetLayout desc_layout, VkDescriptorPoolSize *size);
VkRenderPass create_renderpass(unsigned num_rts, bool clear);
VkFramebuffer create_framebuffer(unsigned num_rts, VkRenderPass render_pass);
VkPipelineLayout create_pipeline_layout(VkDescriptorSetLayout *desc_layouts, unsigned num_desc_layouts);
void create_basic_pipelines(VkRenderPass render_pass, VkPipelineLayout layout, VkPipeline *pipelines);
void create_vattrib_pipelines(VkRenderPass render_pass, VkPipelineLayout layout, VkPipeline *pipelines);
VkPipeline create_multirt_pipeline(VkRenderPass render_pass, VkPipelineLayout layout);
VkPipeline create_ubo_pipeline(VkRenderPass render_pass, VkPipelineLayout layout);
VkPipeline create_sampler_pipeline(VkRenderPass render_pass, VkPipelineLayout layout);
VkPipeline create_sampler_many_pipeline(VkRenderPass render_pass, VkPipelineLayout layout);
VkPipeline create_image_pipeline(VkRenderPass render_pass, VkPipelineLayout layout);
VkPipeline create_image_many_pipeline(VkRenderPass render_pass, VkPipelineLayout layout);
VkPipeline create_tbo_pipeline(VkRenderPass render_pass, VkPipelineLayout layout);
VkPipeline create_tbo_many_pipeline(VkRenderPass render_pass, VkPipelineLayout layout);
VkPipeline create_ibo_pipeline(VkRenderPass render_pass, VkPipelineLayout layout);
VkPipeline create_ibo_many_pipeline(VkRenderPass render_pass, VkPipelineLayout layout);
VkPipeline create_ssbo_pipeline(VkRenderPass render_pass, VkPipelineLayout layout);
VkPipeline create_ssbo_many_pipeline(VkRenderPass render_pass, VkPipelineLayout layout);
VkPipeline create_vattrib_pipeline_dynamic(VkRenderPass render_pass, VkPipelineLayout layout);

VkBuffer create_vertex_buffer(void);
VkBuffer create_uniform_buffer(void);
VkBuffer create_index_buffer(void);
VkBuffer create_texel_buffer(void);
VkBuffer create_image_buffer(void);
VkBuffer create_storage_buffer(void);
VkBufferView create_bufferview(VkBuffer buffer);
VkImageView create_rt(VkImage *ret_image, bool mutable);
VkImageView create_rt_ms(VkImage *ret_image, VkSampleCountFlags samples, bool mutable);
VkImageView create_tex(VkImage *ret_image);
VkImageView create_storage_image(VkImage *ret_image);

static inline VkImageSubresourceRange
default_subresourcerange(void)
{
   VkImageSubresourceRange sub = {0};
   sub.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
   sub.baseMipLevel = 0;
   sub.levelCount = 1;
   sub.baseArrayLayer = 0;
   sub.layerCount = 1;
   return sub;
}

static inline VkImageSubresourceLayers
default_subresourcerangelayers(void)
{
   VkImageSubresourceLayers sub = {0};
   sub.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
   sub.mipLevel = 0;
   sub.baseArrayLayer = 0;
   sub.layerCount = 1;
   return sub;
}

#endif
