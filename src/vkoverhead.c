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

#define XXH_INLINE_ALL
#include "vkoverhead.h"
#include "common.h"
#include "hash_table.h"
#include "u_memory.h"
#include "u_queue.h"
#include "u_cpu_detect.h"
#include "xxhash.h"
#include <inttypes.h>

struct vk_device *dev;

static unsigned max_rts;

#define MAX_CMDBUFS 50
#define MAX_CMDBUF_POOLS 3
#define MAX_DRAWS 10000
#define NUM_PIPELINE_VARIANTS 4

struct util_queue queue;

struct pool {
   struct util_queue_fence fence;
   VkCommandPool cmdpool;
   VkCommandBuffer cmdbufs[MAX_CMDBUFS];
   VkFence f;
#if VK_USE_64_BIT_PTR_DEFINES==1
   int64_t *trash_ptrs[MAX_CMDBUFS][MAX_DRAWS];
#else
   void *trash_ptrs[MAX_CMDBUFS][MAX_DRAWS];
#endif
};

static struct pool pools[MAX_CMDBUF_POOLS];
static VkCommandBuffer cmdbuf;
static int cmdbuf_pool_idx;
static int cmdbuf_idx;
static bool cmdbuf_active = false;
static bool rp_active = false;

static VkRenderPass render_pass_multirt_clear;
static VkRenderPass render_pass_multirt_dontcare;
static VkRenderPass render_pass_clear;
static VkRenderPass render_pass_dontcare;
static VkRenderPass render_pass; //this one gets used
static VkFramebuffer fb_basic;
static VkFramebuffer fb_multirt;
static VkFramebuffer fb;
static VkRenderingInfo dyn_basic;
static VkRenderingAttachmentInfo dyn_att_basic[1];
static VkRenderingInfo dyn_multirt;
static VkRenderingAttachmentInfo dyn_att_multirt[MAX_RTS];
static VkRenderingInfo *dyn_render;
static VkPipelineLayout layout_basic;
static VkPipelineLayout layout_ubo;
static VkPipelineLayout layout_ssbo;
static VkPipelineLayout layout_ssbo_many;
static VkPipelineLayout layout_combined_sampler;
static VkPipelineLayout layout_combined_sampler_many;
static VkPipelineLayout layout_sampled_image;
static VkPipelineLayout layout_sampled_image_many;
static VkPipelineLayout layout_tbo;
static VkPipelineLayout layout_tbo_many;
static VkPipelineLayout layout_ibo;
static VkPipelineLayout layout_ibo_many;
static VkPipelineLayout layout_image;
static VkPipelineLayout layout_image_many;
static VkPipelineLayout layout_basic_push;
static VkPipelineLayout layout_ubo_push;
static VkPipelineLayout layout_ssbo_push;
static VkPipelineLayout layout_ssbo_many_push;
static VkPipelineLayout layout_combined_sampler_push;
static VkPipelineLayout layout_combined_sampler_many_push;
static VkPipelineLayout layout_sampled_image_push;
static VkPipelineLayout layout_sampled_image_many_push;
static VkPipelineLayout layout_tbo_push;
static VkPipelineLayout layout_tbo_many_push;
static VkPipelineLayout layout_ibo_push;
static VkPipelineLayout layout_ibo_many_push;
static VkPipelineLayout layout_image_push;
static VkPipelineLayout layout_image_many_push;
/* fewer test variants for descriptor buffer */
static VkPipelineLayout layout_ubo_db;
static VkPipelineLayout layout_ssbo_many_db;
static VkPipelineLayout layout_combined_sampler_many_db;
static VkPipelineLayout layout_image_many_db;
static VkDescriptorUpdateTemplateKHR template_basic;
static VkDescriptorUpdateTemplateKHR template_ubo;
static VkDescriptorUpdateTemplateKHR template_ssbo;
static VkDescriptorUpdateTemplateKHR template_ssbo_many;
static VkDescriptorUpdateTemplateKHR template_combined_sampler;
static VkDescriptorUpdateTemplateKHR template_combined_sampler_many;
static VkDescriptorUpdateTemplateKHR template_sampled_image;
static VkDescriptorUpdateTemplateKHR template_sampled_image_many;
static VkDescriptorUpdateTemplateKHR template_tbo;
static VkDescriptorUpdateTemplateKHR template_tbo_many;
static VkDescriptorUpdateTemplateKHR template_ibo;
static VkDescriptorUpdateTemplateKHR template_ibo_many;
static VkDescriptorUpdateTemplateKHR template_image;
static VkDescriptorUpdateTemplateKHR template_image_many;
static VkDescriptorUpdateTemplateKHR template_basic_push;
static VkDescriptorUpdateTemplateKHR template_ubo_push;
static VkDescriptorUpdateTemplateKHR template_ssbo_push;
static VkDescriptorUpdateTemplateKHR template_ssbo_many_push;
static VkDescriptorUpdateTemplateKHR template_combined_sampler_push;
static VkDescriptorUpdateTemplateKHR template_combined_sampler_many_push;
static VkDescriptorUpdateTemplateKHR template_sampled_image_push;
static VkDescriptorUpdateTemplateKHR template_sampled_image_many_push;
static VkDescriptorUpdateTemplateKHR template_tbo_push;
static VkDescriptorUpdateTemplateKHR template_tbo_many_push;
static VkDescriptorUpdateTemplateKHR template_ibo_push;
static VkDescriptorUpdateTemplateKHR template_ibo_many_push;
static VkDescriptorUpdateTemplateKHR template_image_push;
static VkDescriptorUpdateTemplateKHR template_image_many_push;
static VkPipeline pipelines_basic[NUM_PIPELINE_VARIANTS];
static VkPipeline pipelines_vattrib[NUM_PIPELINE_VARIANTS];
static VkPipeline pipelines_dyn[NUM_PIPELINE_VARIANTS];
static VkPipeline pipeline_vattrib_dynamic;
static VkPipeline pipeline_multirt;
static VkPipeline pipeline_multrt_dyn;
static VkPipeline pipeline_ubo;
static VkPipeline pipeline_ssbo;
static VkPipeline pipeline_ssbo_many;
static VkPipeline pipeline_combined_sampler;
static VkPipeline pipeline_combined_sampler_many;
static VkPipeline pipeline_tbo;
static VkPipeline pipeline_tbo_many;
static VkPipeline pipeline_ibo;
static VkPipeline pipeline_ibo_many;
static VkPipeline pipeline_image;
static VkPipeline pipeline_image_many;
static VkPipeline pipeline_gpl_basic;
static VkPipeline pipeline_gpl_vert[2];
static VkPipeline pipeline_gpl_vert_final[2];
/* descriptor buffer offset binding has fewer tests */
static VkPipeline pipeline_ubo_db;
static VkPipeline pipeline_ssbo_many_db;
static VkPipeline pipeline_combined_sampler_many_db;
static VkPipeline pipeline_image_many_db;
static VkPipeline *pipelines; //this one gets used
static VkDescriptorSet desc_set_basic[2];
static VkDescriptorSet desc_set_ubo[2];
static VkDescriptorSet desc_set_ssbo[2];
static VkDescriptorSet desc_set_ssbo_many[2];
static VkDescriptorSet desc_set_combined_sampler[2];
static VkDescriptorSet desc_set_combined_sampler_many[2];
static VkDescriptorSet desc_set_sampled_image[2];
static VkDescriptorSet desc_set_sampled_image_many[2];
static VkDescriptorSet desc_set_image[2];
static VkDescriptorSet desc_set_image_many[2];
static VkDescriptorSet desc_set_tbo[2];
static VkDescriptorSet desc_set_tbo_many[2];
static VkDescriptorSet desc_set_ibo[2];
static VkDescriptorSet desc_set_ibo_many[2];
/* these are all [host, gpu] */
static VkDescriptorSet desc_set_basic_mutable[2];
static VkDescriptorSet desc_set_ubo_mutable[2];
static VkDescriptorSet desc_set_ssbo_mutable[2];
static VkDescriptorSet desc_set_ssbo_many_mutable[2];
static VkDescriptorSet desc_set_combined_sampler_mutable[2];
static VkDescriptorSet desc_set_combined_sampler_many_mutable[2];
static VkDescriptorSet desc_set_sampled_image_mutable[2];
static VkDescriptorSet desc_set_sampled_image_many_mutable[2];
static VkDescriptorSet desc_set_image_mutable[2];
static VkDescriptorSet desc_set_image_many_mutable[2];
static VkDescriptorSet desc_set_tbo_mutable[2];
static VkDescriptorSet desc_set_tbo_many_mutable[2];
static VkDescriptorSet desc_set_ibo_mutable[2];
static VkDescriptorSet desc_set_ibo_many_mutable[2];
static VkBuffer vbo[16];
static VkBuffer ubo[MAX_UBOS];
static VkBuffer ssbo[MAX_SSBOS];
static VkBuffer tbo[MAX_SAMPLERS];
static VkBuffer ibo[MAX_IMAGES];
static VkBuffer index_bo[2];
static VkDeviceAddress ubo_bda[MAX_UBOS];
static VkDeviceAddress ssbo_bda[MAX_SSBOS];
static VkDeviceAddress tbo_bda[MAX_SAMPLERS];
static VkDeviceAddress ibo_bda[MAX_IMAGES];
static VkImageView tex[MAX_SAMPLERS];
static VkImageView img[MAX_IMAGES];
static VkSampler sampler;
static VkBufferView tbo_views[2][MAX_SAMPLERS];
static VkBufferView ibo_views[2][MAX_SAMPLERS];
static VkDescriptorBufferInfo dbi[2][MAX_UBOS] = {0};
static VkDescriptorBufferInfo dbi_storage[2][MAX_SSBOS] = {0};
static VkDescriptorAddressInfoEXT dai[2][MAX_UBOS] = {0};
static VkDescriptorAddressInfoEXT dai_storage[2][MAX_SSBOS] = {0};
static VkDescriptorAddressInfoEXT dai_tbo[2][MAX_SAMPLERS] = {0};
static VkDescriptorAddressInfoEXT dai_ibo[2][MAX_IMAGES] = {0};
static VkDescriptorImageInfo dii[2][MAX_SAMPLERS] = {0};
static VkDescriptorImageInfo dii_storage[2][MAX_IMAGES] = {0};
static VkImage copy_image_src[2]; //normal, mutable
static VkImage copy_image_dst[2]; //normal, mutable
static VkImage copy_image_ms[2]; //normal, mutable
/* 2 buffers for alternating */
static VkBuffer ubo_db[2];
static VkDeviceAddress ubo_db_bda[2];
static VkBuffer ssbo_db[2];
static VkDeviceAddress ssbo_db_bda[2];
static VkBuffer sampler_db[2];
static VkDeviceAddress sampler_db_bda[2];
static VkBuffer image_db[2];
static VkDeviceAddress image_db_bda[2];

/* special-case variables */
static VkPipeline depthonly_pipelines[4];
static VkPipelineLayout depthonly_layout;
static VkPipeline slow_pipelines[4];
static VkPipelineLayout slow_layout;

static VkMultiDrawInfoEXT draws[500];
static VkMultiDrawIndexedInfoEXT draws_indexed[500];
static uint64_t count = 0;

static bool is_submit = false;
static bool is_dynamic = false;
static bool submit_init = false;
static bool is_descriptor_buffer = false;
static bool is_zerovram = false;

/* cmdline options */
static double duration = 1.0;
static int fixed_iteration_count = 0;
static int test_no = -1;
static int start_no = -1;
static bool csv = false;
static bool color = true;
static bool submit_only = false;
static bool draw_only = false;
static bool descriptor_only = false;
static bool misc_only = false;
static bool output_only = false;

static VkDeviceAddress descriptor_buffer;

static util_queue_execute_func cleanup_func = NULL;

static VkDeviceSize vram_size;
static VkBuffer zerovram_buffers[MAX_CMDBUF_POOLS][MAX_CMDBUFS];
static VkDeviceMemory zerovram_mem[MAX_CMDBUF_POOLS][MAX_CMDBUFS];

static bool
check_multi_draw(void)
{
   return dev->info.have_EXT_multi_draw;
}

static bool
check_dynamic_vertex_input(void)
{
   return dev->info.have_EXT_vertex_input_dynamic_state;
}

static bool
check_graphics_pipeline_library(void)
{
   return dev->info.have_EXT_graphics_pipeline_library;
}

static bool
check_dynamic_rendering(void)
{
   return dev->info.have_KHR_dynamic_rendering;
}

static bool
check_descriptor_template(void)
{
   return dev->info.have_KHR_descriptor_update_template;
}

static bool
check_push_descriptor(void)
{
   return dev->info.have_KHR_push_descriptor && check_descriptor_template();
}

static bool
check_mutable_descriptor(void)
{
   return dev->info.have_EXT_mutable_descriptor_type;
}

static bool
check_descriptor_buffer(void)
{
   return dev->info.have_EXT_descriptor_buffer;
}

static bool
check_dota2(void)
{
   return check_graphics_pipeline_library() &&
          dev->info.props.limits.maxBoundDescriptorSets >= 5 &&
          dev->info.props.limits.maxPerStageDescriptorUniformBuffers >= 20 &&
          dev->info.props.limits.maxPerStageDescriptorSampledImages >= 2185;
}

static bool
check_zerovram(void)
{
   return dev->info.have_EXT_memory_budget;
}

static void
init_submit(unsigned cmdbuf_count, unsigned submit_count, void *si, VkCommandBufferSubmitInfo *csi)
{
   VkCommandBuffer *c = pools[cmdbuf_pool_idx].cmdbufs;
   unsigned cmdbuf_per_submit = cmdbuf_count / submit_count;
   if (dev->info.have_KHR_synchronization2) {
      VkSubmitInfo2 *s = si;
      for (unsigned i = 0; i < submit_count; i++) {
         s[i].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
         s[i].commandBufferInfoCount = cmdbuf_per_submit;
         for (unsigned j = 0; j < cmdbuf_per_submit; j++) {
            csi[i * cmdbuf_per_submit + j].sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
            csi[i * cmdbuf_per_submit + j].commandBuffer = c[i * cmdbuf_per_submit + j];
         }
         s[i].pCommandBufferInfos = csi;
      }
   } else {
      VkSubmitInfo *s = si;
      for (unsigned i = 0; i < submit_count; i++) {
         s[i].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
         s[i].commandBufferCount = cmdbuf_per_submit;
         s[i].pCommandBuffers = &c[i * cmdbuf_per_submit];
      }
   }
}

static void
submit_cmdbufs(unsigned submit_count, void *si)
{
   if (dev->info.have_KHR_synchronization2) {
      VkResult result = VK(QueueSubmit2)(dev->queue, submit_count, si, is_zerovram ? pools[cmdbuf_pool_idx].f : VK_NULL_HANDLE);
      VK_CHECK("QueueSubmit2", result);
   } else {
      VkResult result = VK(QueueSubmit)(dev->queue, submit_count, si, is_zerovram ? pools[cmdbuf_pool_idx].f : VK_NULL_HANDLE);
      VK_CHECK("QueueSubmit", result);
   }
}

static void
submit_cmdbufs_helper(unsigned cmdbuf_count, unsigned submit_count, unsigned iterations)
{
   union {
      VkSubmitInfo2 s2;
      VkSubmitInfo s1;
   } s[MAX_CMDBUFS] = {0};
   VkCommandBufferSubmitInfo csi[MAX_CMDBUFS] = {0};
   init_submit(cmdbuf_count, submit_count, s, csi);
   for (unsigned i = 0; i < iterations; i++) {
      submit_cmdbufs(submit_count, s);
   }
}

static void
reset_gpl(void *data, void *gdata, int thread_idx)
{
   struct pool *p = data;
   for (unsigned j = 0; j < MAX_CMDBUFS; j++) {
      for (unsigned k = 0; k < MAX_DRAWS; k++) {
         VK(DestroyPipeline)(dev->dev, (VkPipeline)p->trash_ptrs[j][k], NULL);
         p->trash_ptrs[j][k] = VK_NULL_HANDLE;
      }
   }
}

static void
reset_zerovram(void *data, void *gdata, int thread_idx)
{
   struct pool *p = data;
   unsigned idx = 0;
   for (unsigned i = 0; i < MAX_CMDBUF_POOLS; i++) {
      if (p == &pools[i]) {
         idx = i;
         break;
      }
   }
   VK(ResetFences)(dev->dev, 1, &p->f);
   for (unsigned i = 0; i < MAX_CMDBUFS; i++) {
      if (zerovram_buffers[idx][i]) {
         VK(DestroyBuffer)(dev->dev, zerovram_buffers[idx][i], NULL);
      }
      if (zerovram_mem[idx][i]) {
         VK(FreeMemory)(dev->dev, zerovram_mem[idx][i], NULL);
      }
   }
   memset(zerovram_buffers[idx], 0, sizeof(zerovram_buffers[idx]));
   memset(zerovram_mem[idx], 0, sizeof(zerovram_mem[idx]));
}

static void
reset_cmdbuf(void *data, void *gdata, int thread_idx)
{
   struct pool *p = data;
   VkResult result;
   if (is_zerovram) {
      result = VK(WaitForFences)(dev->dev, 1, &p->f, VK_TRUE, UINT64_MAX);
      VK_CHECK("WaitForFences", result);
      /* must be called in thread before fence is signaled */
      reset_zerovram(data, gdata, thread_idx);
   }
   result = VK(ResetCommandPool)(dev->dev, p->cmdpool, 0);
   VK_CHECK("ResetCommandPool", result);
}

static void
next_cmdbuf_pool(void)
{
   if (is_zerovram) {
      submit_cmdbufs_helper(1, 1, 1);
   }
   util_queue_add_job(&queue, &pools[cmdbuf_pool_idx], &pools[cmdbuf_pool_idx].fence, reset_cmdbuf, cleanup_func, 0);
   cmdbuf_pool_idx++;
   cmdbuf_pool_idx %= MAX_CMDBUF_POOLS;
   util_queue_fence_wait(&pools[cmdbuf_pool_idx].fence);
}

static void
next_cmdbuf(void)
{
   if (is_zerovram) {
      /* these only use first cmdbuf per pool */
      next_cmdbuf_pool();
      cmdbuf_idx = 0;
   } else {
      cmdbuf_idx++;
      cmdbuf_idx %= MAX_CMDBUFS;
      if (cmdbuf_idx == 0)
         next_cmdbuf_pool();
   }
   cmdbuf = pools[cmdbuf_pool_idx].cmdbufs[cmdbuf_idx];
}

static void
generate_vattribs_dynamic(VkVertexInputBindingDescription2EXT *bindings, VkVertexInputAttributeDescription2EXT *vattr)
{
   for (unsigned i = 0; i < 16; i++) {
      VkFormat formats[] = {
         VK_FORMAT_R32_SFLOAT,
         VK_FORMAT_R32G32_SFLOAT,
         VK_FORMAT_R32G32B32_SFLOAT,
         VK_FORMAT_R32G32B32A32_SFLOAT,
      };
      /* random format */
      int fmt_idx = rand() % ARRAY_SIZE(formats);
      unsigned num_components = fmt_idx + 1;

      bindings[i].sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT;
      bindings[i].pNext = NULL;
      bindings[i].binding = i;
      /* random stride aligned to element size */
      bindings[i].stride = sizeof(float) * num_components * MAX2(rand() % 3, 1);
      bindings[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
      bindings[i].divisor = 0;

      vattr[i].sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
      vattr[i].pNext = NULL;
      vattr[i].location = i;
      vattr[i].binding = i;
      vattr[i].format = formats[fmt_idx];
      vattr[i].offset = sizeof(float) * (rand() % 4);
   }
}

static void
begin_cmdbuf(void)
{
   VkCommandBufferBeginInfo cbbi = {0};
   cbbi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
   cbbi.flags = is_submit ? VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT : VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

   VkResult result = VK(BeginCommandBuffer)(cmdbuf, &cbbi);
   VK_CHECK("BeginCommandBuffer", result);
   cmdbuf_active = true;

   VkDeviceSize offsets[16] = {0};
   if (pipelines) {
         VK(CmdBindPipeline)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[0]);
      if (!is_dynamic)
         VK(CmdBindVertexBuffers)(cmdbuf, 0, ARRAY_SIZE(vbo), vbo, offsets);
      VK(CmdBindIndexBuffer)(cmdbuf, index_bo[0], 0, VK_INDEX_TYPE_UINT32);
      if (!is_descriptor_buffer)
         VK(CmdBindDescriptorSets)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_basic, 0, 1, desc_set_basic, 0, NULL);
   }

   if (is_descriptor_buffer && check_descriptor_buffer()) {
      VkDescriptorBufferBindingInfoEXT info = {0};
      info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT;
      info.address = descriptor_buffer;
      info.usage = VK_BUFFER_USAGE_SAMPLER_DESCRIPTOR_BUFFER_BIT_EXT | VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT;
      VK(CmdBindDescriptorBuffersEXT)(cmdbuf, 1, &info);
   }

   if (!is_dynamic)
      return;

   if (dev->info.have_EXT_vertex_input_dynamic_state) {
      VkVertexInputBindingDescription2EXT vbindings[16];
      VkVertexInputAttributeDescription2EXT vattr[16];
      generate_vattribs_dynamic(vbindings, vattr);
      VK(CmdSetVertexInputEXT)(cmdbuf, 16, vbindings, 16, vattr);
      VK(CmdBindVertexBuffers)(cmdbuf, 0, ARRAY_SIZE(vbo), vbo, offsets);
   } else if (dev->info.have_EXT_extended_dynamic_state) {
      float vec4[4];
      VkDeviceSize sizes[16];
      VkDeviceSize strides[16] = {0};
      for (unsigned i = 0; i < ARRAY_SIZE(vbo); i++) {
         sizes[i] = sizeof(vec4) * 16; // VK_WHOLE_SIZE is illegal because spec is dumb
         strides[i] = 0;
      }
      VK(CmdBindVertexBuffers2)(cmdbuf, 0, ARRAY_SIZE(vbo), vbo, offsets, sizes, strides);
   } else {
      VK(CmdBindVertexBuffers)(cmdbuf, 0, ARRAY_SIZE(vbo), vbo, offsets);
   }

   VkViewport viewport = {0, 0, 100, 100, 0, 1};
   VkRect2D scissor = {0};
   scissor.extent.width = 100;
   scissor.extent.height = 100;
   if (dev->info.have_EXT_extended_dynamic_state) {
      VK(CmdSetViewportWithCountEXT)(cmdbuf, 1, &viewport);
      VK(CmdSetScissorWithCountEXT)(cmdbuf, 1, &scissor);

      VK(CmdSetDepthBoundsTestEnableEXT)(cmdbuf, VK_FALSE);
      VK(CmdSetDepthBounds)(cmdbuf, 0.0, 1.0);
      VK(CmdSetDepthTestEnableEXT)(cmdbuf, VK_FALSE);
      VK(CmdSetDepthCompareOpEXT)(cmdbuf, VK_COMPARE_OP_ALWAYS);
      VK(CmdSetDepthWriteEnableEXT)(cmdbuf, VK_FALSE);
      VK(CmdSetStencilTestEnableEXT)(cmdbuf, VK_FALSE);

      VK(CmdSetStencilOpEXT)(cmdbuf, VK_STENCIL_FACE_FRONT_AND_BACK, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS);
      VK(CmdSetStencilWriteMask)(cmdbuf, VK_STENCIL_FACE_FRONT_AND_BACK, 255);
      VK(CmdSetStencilCompareMask)(cmdbuf, VK_STENCIL_FACE_FRONT_AND_BACK, 255);
      VK(CmdSetFrontFaceEXT)(cmdbuf, VK_FRONT_FACE_CLOCKWISE);
      VK(CmdSetCullModeEXT)(cmdbuf, VK_CULL_MODE_NONE);
      VK(CmdSetDepthBias)(cmdbuf, 0.0f, 0.0f, 0.0f);
   } else {
      VK(CmdSetViewport)(cmdbuf, 0, 1, &viewport);
      VK(CmdSetScissor)(cmdbuf, 0, 1, &scissor);
   }

   if (dev->info.have_EXT_extended_dynamic_state2) {
      VK(CmdSetPrimitiveTopologyEXT)(cmdbuf, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
      VK(CmdSetPrimitiveRestartEnableEXT)(cmdbuf, VK_FALSE);
      VK(CmdSetRasterizerDiscardEnableEXT)(cmdbuf, VK_FALSE);
      if (dev->info.dynamic_state2_feats.extendedDynamicState2PatchControlPoints)
         VK(CmdSetPatchControlPointsEXT)(cmdbuf, 3);
   }

   float blend_constants[4] = {0};
   VK(CmdSetBlendConstants)(cmdbuf, blend_constants);
   VK(CmdSetStencilReference)(cmdbuf, VK_STENCIL_FACE_FRONT_BIT, 0);
   VK(CmdSetStencilReference)(cmdbuf, VK_STENCIL_FACE_BACK_BIT, 0);
}

static void
end_rp(void)
{
   if (render_pass)
      VK(CmdEndRenderPass)(cmdbuf);
   else
      VK(CmdEndRendering)(cmdbuf);
   rp_active = false;
}

static void
end_cmdbuf(void)
{
   if (rp_active)
      end_rp();
   VkResult result = VK(EndCommandBuffer)(cmdbuf);
   VK_CHECK("EndCommandBuffer", result);
   cmdbuf_active = false;
}

static void
begin_rp(void)
{
   if (rp_active)
      return;
   if (!cmdbuf_active)
      begin_cmdbuf();
   if (render_pass) {
      VkRenderPassBeginInfo rpbi = {0};
      rpbi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      rpbi.renderPass = render_pass;
      rpbi.renderArea.extent.width = 100;
      rpbi.renderArea.extent.height = 100;
      rpbi.framebuffer = fb;
      rpbi.clearValueCount = fb == fb_basic ? 1 : max_rts;
      VkClearValue clear[MAX_RTS] = {0};
      rpbi.pClearValues = clear;
      VK(CmdBeginRenderPass)(cmdbuf, &rpbi, VK_SUBPASS_CONTENTS_INLINE);
   } else {
      VK(CmdBeginRendering)(cmdbuf, dyn_render);
   }
   rp_active = true;
}

static unsigned
filter_overflow(perf_rate_func func, unsigned iterations, unsigned divisor)
{
   /* zerovram needs 256mb-sized buffers */
   unsigned limit = is_zerovram ? MIN2(vram_size / 256 / MAX_CMDBUF_POOLS, MAX_CMDBUFS) : MAX_DRAWS;
   while (count + iterations * divisor > limit) {
      unsigned remain = (limit - count) / divisor;
      func(remain);
      end_cmdbuf();
      next_cmdbuf();
      count = 0;
      iterations -= remain;
   }
   return iterations;
}

static void
draw(unsigned iterations)
{
   iterations = filter_overflow(draw, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++)
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
}

static void
draw_multi(unsigned iterations)
{
   iterations = filter_overflow(draw_multi, iterations, ARRAY_SIZE(draws_indexed));
   render_pass = render_pass_dontcare;
   begin_rp();
   int32_t offset = 0;
   for (unsigned i = 0; i < iterations; i++, count += ARRAY_SIZE(draws_indexed)) {
      VK(CmdDrawMultiIndexedEXT)(cmdbuf, ARRAY_SIZE(draws_indexed), draws_indexed, 1, 0, sizeof(VkMultiDrawIndexedInfoEXT), &offset);
   }
}

static void
draw_vertex(unsigned iterations)
{
   iterations = filter_overflow(draw_vertex, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++)
      VK(CmdDraw)(cmdbuf, 3, 1, 0, 0);
}

static void
draw_multi_vertex(unsigned iterations)
{
   iterations = filter_overflow(draw_multi_vertex, iterations, ARRAY_SIZE(draws));
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count += ARRAY_SIZE(draws)) {
      VK(CmdDrawMultiEXT)(cmdbuf, ARRAY_SIZE(draws), draws, 1, 0, sizeof(VkMultiDrawInfoEXT));
   }
}

static void
draw_index_change(unsigned iterations)
{
   iterations = filter_overflow(draw_index_change, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdBindIndexBuffer)(cmdbuf, index_bo[i & 1], 0, VK_INDEX_TYPE_UINT32);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_index_offset_change(unsigned iterations)
{
   iterations = filter_overflow(draw_index_offset_change, iterations, 1);
   begin_rp();
   VkDeviceSize offsets[] = {0, 16};
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdBindIndexBuffer)(cmdbuf, index_bo[0], offsets[i & 1], VK_INDEX_TYPE_UINT32);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_vbo_change(unsigned iterations)
{
   iterations = filter_overflow(draw_vbo_change, iterations, 1);
   begin_rp();
   VkDeviceSize offset[] = {0, 16};
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdBindVertexBuffers)(cmdbuf, 0, 1, &vbo[i & 1], &offset[i & 1]);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_rp_begin_end(unsigned iterations)
{
   iterations = filter_overflow(draw_rp_begin_end, iterations, 1);
   for (unsigned i = 0; i < iterations; i++, count++) {
      begin_rp();
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
      end_rp();
   }
}

static void
draw_rp_begin_end_dontcare(unsigned iterations)
{
   iterations = filter_overflow(draw_rp_begin_end_dontcare, iterations, 1);
   for (unsigned i = 0; i < iterations; i++, count++) {
      begin_rp();
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
      end_rp();
   }
}

static void
draw_rp_begin_end_dynrender(unsigned iterations)
{
   iterations = filter_overflow(draw_rp_begin_end_dynrender, iterations, 1);
   for (unsigned i = 0; i < iterations; i++, count++) {
      begin_rp();
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
      end_rp();
   }
}

static void
draw_rp_begin_end_dontcare_dynrender(unsigned iterations)
{
   iterations = filter_overflow(draw_rp_begin_end_dontcare_dynrender, iterations, 1);
   for (unsigned i = 0; i < iterations; i++, count++) {
      begin_rp();
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
      end_rp();
   }
}

static void
draw_multirt(unsigned iterations)
{
   iterations = filter_overflow(draw_multirt, iterations, 1);
   render_pass = render_pass_multirt_dontcare;
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++)
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
}

static void
draw_multirt_begin_end(unsigned iterations)
{
   iterations = filter_overflow(draw_multirt_begin_end, iterations, 1);
   for (unsigned i = 0; i < iterations; i++, count++) {
      begin_rp();
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
      end_rp();
   }
}

static void
draw_multirt_dynrender(unsigned iterations)
{
   iterations = filter_overflow(draw_multirt_dynrender, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++)
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
}

static void
draw_multirt_begin_end_dynrender(unsigned iterations)
{
   iterations = filter_overflow(draw_multirt_begin_end_dynrender, iterations, 1);
   for (unsigned i = 0; i < iterations; i++, count++) {
      begin_rp();
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
      end_rp();
   }
}

static void
draw_multirt_begin_end_dontcare(unsigned iterations)
{
   iterations = filter_overflow(draw_multirt_begin_end_dontcare, iterations, 1);
   for (unsigned i = 0; i < iterations; i++, count++) {
      begin_rp();
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
      end_rp();
   }
}

static void
draw_multirt_begin_end_dontcare_dynrender(unsigned iterations)
{
   iterations = filter_overflow(draw_multirt_begin_end_dontcare_dynrender, iterations, 1);
   for (unsigned i = 0; i < iterations; i++, count++) {
      begin_rp();
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
      end_rp();
   }
}

static void
draw_1vattrib_change(unsigned iterations)
{
   iterations = filter_overflow(draw_1vattrib_change, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdBindPipeline)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[i % NUM_PIPELINE_VARIANTS]);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_16vattrib(unsigned iterations)
{
   iterations = filter_overflow(draw_16vattrib, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++)
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
}

static void
draw_16vattrib_16vbo_change(unsigned iterations)
{
   iterations = filter_overflow(draw_16vattrib_16vbo_change, iterations, 1);
   begin_rp();
   VkDeviceSize offsets[] = {0, 4, 8, 12, 16};
   VkDeviceSize new_offsets[2][16];
   VkBuffer new_vbos[2][16];
   for (unsigned i = 0; i < 2; i++) {
      for (unsigned j = 0; j < 16; j++) {
         new_offsets[i][j] = offsets[rand() % ARRAY_SIZE(offsets)];
         new_vbos[i][j] = vbo[rand() % ARRAY_SIZE(vbo)];
      }
   }
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdBindVertexBuffers)(cmdbuf, 0, 16, new_vbos[i & 1], new_offsets[i & 1]);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_16vattrib_change(unsigned iterations)
{
   iterations = filter_overflow(draw_16vattrib_change, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdBindPipeline)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[i % NUM_PIPELINE_VARIANTS]);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_16vattrib_change_dynamic(unsigned iterations)
{
   iterations = filter_overflow(draw_16vattrib_change_dynamic, iterations, 1);
   begin_rp();
   VkVertexInputBindingDescription2EXT vbindings[2][16];
   VkVertexInputAttributeDescription2EXT vattr[2][16];
   for (unsigned i = 0; i < 2; i++)
      generate_vattribs_dynamic(vbindings[i], vattr[i]);
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdSetVertexInputEXT)(cmdbuf, 16, vbindings[i & 1], 16, vattr[i & 1]);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

/* static to avoid runtime overhead */
static VkGraphicsPipelineCreateInfo draw_16vattrib_change_gpl_pci = {0};

static void
draw_16vattrib_change_gpl(unsigned iterations)
{
   iterations = filter_overflow(draw_16vattrib_change_gpl, iterations, 1);
   VkPipeline libraries[2];
   libraries[0] = pipeline_gpl_basic;
   /* not zero-initialized to avoid potential memset overhead */
   VkPipelineLibraryCreateInfoKHR libstate = {
      VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR,
      NULL,
      ARRAY_SIZE(libraries),
      libraries
   };
   draw_16vattrib_change_gpl_pci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
   draw_16vattrib_change_gpl_pci.pNext = &libstate;
   draw_16vattrib_change_gpl_pci.layout = layout_basic;
   cleanup_func = reset_gpl;
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++) {
      libraries[1] = pipeline_gpl_vert[i & 1];
      VkPipeline pipeline;
      VkResult result = VK(CreateGraphicsPipelines)(dev->dev, VK_NULL_HANDLE, 1, &draw_16vattrib_change_gpl_pci, NULL, &pipeline);
      VK_CHECK("CreateGraphicsPipelines", result);
      VK(CmdBindPipeline)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
#if VK_USE_64_BIT_PTR_DEFINES==1
      pools[cmdbuf_pool_idx].trash_ptrs[cmdbuf_idx][count] = (int64_t*)pipeline;
#else
      pools[cmdbuf_pool_idx].trash_ptrs[cmdbuf_idx][count] = (void*)pipeline;
#endif
   }
}

static VkVertexInputBindingDescription gpl_vbinding[2][16];
static VkVertexInputAttributeDescription gpl_vattr[2][16];
struct hash_table gpl_pipeline_table;

static uint32_t
gpl_hash_vi(const void *data)
{
   uint32_t hash = 0;
   const VkPipelineVertexInputStateCreateInfo *key = data;
   hash = XXH32(&key->vertexBindingDescriptionCount, sizeof(key->vertexBindingDescriptionCount), hash);
   hash = XXH32(&key->vertexAttributeDescriptionCount, sizeof(key->vertexAttributeDescriptionCount), hash);
   hash = XXH32(key->pVertexBindingDescriptions, key->vertexBindingDescriptionCount * sizeof(VkVertexInputBindingDescription), hash);
   hash = XXH32(key->pVertexAttributeDescriptions, key->vertexAttributeDescriptionCount * sizeof(VkVertexInputAttributeDescription), hash);
   return hash;
}

static bool
gpl_equals_vi(const void *a, const void *b)
{
   const VkPipelineVertexInputStateCreateInfo *key_a = a;
   const VkPipelineVertexInputStateCreateInfo *key_b = b;
   return key_a->vertexBindingDescriptionCount == key_b->vertexBindingDescriptionCount &&
          key_a->vertexAttributeDescriptionCount == key_b->vertexAttributeDescriptionCount &&
          !memcmp(key_a->pVertexBindingDescriptions, key_b->pVertexBindingDescriptions, key_a->vertexBindingDescriptionCount * sizeof(VkVertexInputBindingDescription)) &&
          !memcmp(key_a->pVertexAttributeDescriptions, key_b->pVertexAttributeDescriptions, key_a->vertexAttributeDescriptionCount * sizeof(VkVertexInputAttributeDescription));
}

static void
draw_16vattrib_change_gpl_hashncache(unsigned iterations)
{
   iterations = filter_overflow(draw_16vattrib_change_gpl_hashncache, iterations, 1);
   begin_rp();
   VkPipelineVertexInputStateCreateInfo vertex_input_state = {0};
   vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
   vertex_input_state.vertexBindingDescriptionCount = 16;
   vertex_input_state.vertexAttributeDescriptionCount = 16;
   for (unsigned i = 0; i < iterations; i++, count++) {
      vertex_input_state.pVertexBindingDescriptions = gpl_vbinding[i & 1];
      vertex_input_state.pVertexAttributeDescriptions = gpl_vattr[i & 1];
      struct hash_entry *he = _mesa_hash_table_search(&gpl_pipeline_table, &vertex_input_state);
      VkPipeline pipeline = he->data;
      VK(CmdBindPipeline)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_1ubo_change(unsigned iterations)
{
   iterations = filter_overflow(draw_1ubo_change, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdBindDescriptorSets)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_basic, 0, 1, &desc_set_basic[i & 1], 0, NULL);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_12ubo_change(unsigned iterations)
{
   iterations = filter_overflow(draw_12ubo_change, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdBindDescriptorSets)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_ubo, 0, 1, &desc_set_ubo[i & 1], 0, NULL);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_1ssbo_change(unsigned iterations)
{
   iterations = filter_overflow(draw_1ssbo_change, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdBindDescriptorSets)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_ssbo, 0, 1, &desc_set_ssbo[i & 1], 0, NULL);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_8ssbo_change(unsigned iterations)
{
   iterations = filter_overflow(draw_8ssbo_change, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdBindDescriptorSets)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_ssbo_many, 0, 1, &desc_set_ssbo_many[i & 1], 0, NULL);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_1combined_sampler_change(unsigned iterations)
{
   iterations = filter_overflow(draw_1combined_sampler_change, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdBindDescriptorSets)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_combined_sampler, 0, 1, &desc_set_combined_sampler[i & 1], 0, NULL);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_16combined_sampler_change(unsigned iterations)
{
   iterations = filter_overflow(draw_16combined_sampler_change, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdBindDescriptorSets)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_combined_sampler_many, 0, 1, &desc_set_combined_sampler_many[i & 1], 0, NULL);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_1texelbuffer_change(unsigned iterations)
{
   iterations = filter_overflow(draw_1texelbuffer_change, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdBindDescriptorSets)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_tbo, 0, 1, &desc_set_tbo[i & 1], 0, NULL);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_16texelbuffer_change(unsigned iterations)
{
   iterations = filter_overflow(draw_16texelbuffer_change, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdBindDescriptorSets)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_tbo_many, 0, 1, &desc_set_tbo_many[i & 1], 0, NULL);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_1image_change(unsigned iterations)
{
   iterations = filter_overflow(draw_1image_change, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdBindDescriptorSets)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_image, 0, 1, &desc_set_image[i & 1], 0, NULL);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_16image_change(unsigned iterations)
{
   iterations = filter_overflow(draw_16image_change, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdBindDescriptorSets)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_image_many, 0, 1, &desc_set_image_many[i & 1], 0, NULL);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_1imagebuffer_change(unsigned iterations)
{
   iterations = filter_overflow(draw_1imagebuffer_change, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdBindDescriptorSets)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_ibo, 0, 1, &desc_set_ibo[i & 1], 0, NULL);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_16imagebuffer_change(unsigned iterations)
{
   iterations = filter_overflow(draw_16imagebuffer_change, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdBindDescriptorSets)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_ibo_many, 0, 1, &desc_set_ibo_many[i & 1], 0, NULL);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_ubo_db_change(unsigned iterations)
{
   iterations = filter_overflow(draw_ubo_db_change, iterations, 1);
   begin_rp();
   VkDeviceSize offsets[] = {0, DESCRIPTOR_BUFFER_SIZE / 2};
   uint32_t zero = 0;
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdSetDescriptorBufferOffsetsEXT)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_ubo_db, 0, 1, &zero, &offsets[i & 1]);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_ssbo_db_change(unsigned iterations)
{
   iterations = filter_overflow(draw_ssbo_db_change, iterations, 1);
   begin_rp();
   VkDeviceSize offsets[] = {0, DESCRIPTOR_BUFFER_SIZE / 2};
   uint32_t zero = 0;
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdSetDescriptorBufferOffsetsEXT)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_ssbo_many_db, 0, 1, &zero, &offsets[i & 1]);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_combined_sampler_db_change(unsigned iterations)
{
   iterations = filter_overflow(draw_combined_sampler_db_change, iterations, 1);
   begin_rp();
   VkDeviceSize offsets[] = {0, DESCRIPTOR_BUFFER_SIZE / 2};
   uint32_t zero = 0;
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdSetDescriptorBufferOffsetsEXT)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_combined_sampler_many_db, 0, 1, &zero, &offsets[i & 1]);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_image_db_change(unsigned iterations)
{
   iterations = filter_overflow(draw_image_db_change, iterations, 1);
   begin_rp();
   VkDeviceSize offsets[] = {0, DESCRIPTOR_BUFFER_SIZE / 2};
   uint32_t zero = 0;
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdSetDescriptorBufferOffsetsEXT)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_image_many_db, 0, 1, &zero, &offsets[i & 1]);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
submit_noop(unsigned iterations)
{
   submit_cmdbufs_helper(0, 1, iterations);
}

static void
submit_50noop(unsigned iterations)
{
   submit_cmdbufs_helper(0, MAX_CMDBUFS, iterations);
}

static void
submit_1cmdbuf(unsigned iterations)
{
   submit_cmdbufs_helper(1, 1, iterations);
}

static void
submit_50cmdbuf(unsigned iterations)
{
   submit_cmdbufs_helper(MAX_CMDBUFS, 1, iterations);
}

static void
submit_50cmdbuf_50submit(unsigned iterations)
{
   submit_cmdbufs_helper(MAX_CMDBUFS, MAX_CMDBUFS, iterations);
}

static void
descriptor_noop(unsigned iterations)
{
   for (unsigned i = 0; i < iterations; i++)
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 0, NULL);
}

static void
descriptor_1ubo(unsigned iterations)
{
   VkWriteDescriptorSet wds = {0};
   wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   wds.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
   wds.descriptorCount = 1;
   wds.dstSet = desc_set_basic[0];
   for (unsigned i = 0; i < iterations; i++) {
      wds.pBufferInfo = dbi[i & 1];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }
}

static void
descriptor_12ubo(unsigned iterations)
{
   VkWriteDescriptorSet wds = {0};
   wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   wds.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
   wds.descriptorCount = MAX_UBOS;
   wds.dstSet = desc_set_ubo[0];
   for (unsigned i = 0; i < iterations; i++) {
      wds.pBufferInfo = dbi[i & 1];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }
}

static void
descriptor_1ssbo(unsigned iterations)
{
   VkWriteDescriptorSet wds = {0};
   wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   wds.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
   wds.descriptorCount = 1;
   wds.dstSet = desc_set_ssbo[0];
   for (unsigned i = 0; i < iterations; i++) {
      wds.pBufferInfo = dbi_storage[i & 1];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }
}

static void
descriptor_8ssbo(unsigned iterations)
{
   VkWriteDescriptorSet wds = {0};
   wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   wds.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
   wds.descriptorCount = MAX_SSBOS;
   wds.dstSet = desc_set_ssbo_many[0];
   for (unsigned i = 0; i < iterations; i++) {
      wds.pBufferInfo = dbi_storage[i & 1];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }
}

static void
descriptor_1combined_sampler(unsigned iterations)
{
   VkWriteDescriptorSet wds = {0};
   wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   wds.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
   wds.descriptorCount = 1;
   wds.dstSet = desc_set_combined_sampler[0];
   for (unsigned i = 0; i < iterations; i++) {
      wds.pImageInfo = dii[i & 1];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }
}

static void
descriptor_16combined_sampler(unsigned iterations)
{
   VkWriteDescriptorSet wds = {0};
   wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   wds.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
   wds.descriptorCount = MAX_SAMPLERS;
   wds.dstSet = desc_set_combined_sampler_many[0];
   for (unsigned i = 0; i < iterations; i++) {
      wds.pImageInfo = dii[i & 1];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }
}

static void
descriptor_1sampled_image(unsigned iterations)
{
   VkWriteDescriptorSet wds = {0};
   wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   wds.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
   wds.descriptorCount = 1;
   wds.dstSet = desc_set_sampled_image[0];
   for (unsigned i = 0; i < iterations; i++) {
      wds.pImageInfo = dii[i & 1];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }
}

static void
descriptor_16sampled_image(unsigned iterations)
{
   VkWriteDescriptorSet wds = {0};
   wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   wds.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
   wds.descriptorCount = MAX_SAMPLERS;
   wds.dstSet = desc_set_sampled_image_many[0];
   for (unsigned i = 0; i < iterations; i++) {
      wds.pImageInfo = dii[i & 1];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }
}

static void
descriptor_1image(unsigned iterations)
{
   VkWriteDescriptorSet wds = {0};
   wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   wds.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
   wds.descriptorCount = 1;
   wds.dstSet = desc_set_image[0];
   for (unsigned i = 0; i < iterations; i++) {
      wds.pImageInfo = dii_storage[i & 1];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }
}

static void
descriptor_16image(unsigned iterations)
{
   VkWriteDescriptorSet wds = {0};
   wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   wds.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
   wds.descriptorCount = MIN2(dev->info.props.limits.maxPerStageDescriptorStorageImages, MAX_IMAGES);
   wds.dstSet = desc_set_image_many[0];
   for (unsigned i = 0; i < iterations; i++) {
      wds.pImageInfo = dii_storage[i & 1];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }
}

static void
descriptor_1texelbuffer(unsigned iterations)
{
   VkWriteDescriptorSet wds = {0};
   wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   wds.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
   wds.descriptorCount = 1;
   wds.dstSet = desc_set_tbo[0];
   for (unsigned i = 0; i < iterations; i++) {
      wds.pTexelBufferView = tbo_views[i & 1];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }
}

static void
descriptor_16texelbuffer(unsigned iterations)
{
   VkWriteDescriptorSet wds = {0};
   wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   wds.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
   wds.descriptorCount = MAX_SAMPLERS;
   wds.dstSet = desc_set_tbo_many[0];
   for (unsigned i = 0; i < iterations; i++) {
      wds.pTexelBufferView = tbo_views[i & 1];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }
}

static void
descriptor_1imagebuffer(unsigned iterations)
{
   VkWriteDescriptorSet wds = {0};
   wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   wds.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
   wds.descriptorCount = 1;
   wds.dstSet = desc_set_ibo[0];
   for (unsigned i = 0; i < iterations; i++) {
      wds.pTexelBufferView = ibo_views[i & 1];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }
}

static void
descriptor_16imagebuffer(unsigned iterations)
{
   VkWriteDescriptorSet wds = {0};
   wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   wds.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
   wds.descriptorCount = MIN2(dev->info.props.limits.maxPerStageDescriptorStorageImages, MAX_IMAGES);
   wds.dstSet = desc_set_ibo_many[0];
   for (unsigned i = 0; i < iterations; i++) {
      wds.pTexelBufferView = ibo_views[i & 1];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }
}

#define TEMPLATE_DESCRIPTOR_CASE(NAME, SUFFIX, DATA) \
static void \
descriptor_template_##NAME(unsigned iterations) \
{ \
   for (unsigned i = 0; i < iterations; i++) { \
      VK(UpdateDescriptorSetWithTemplate)(dev->dev, desc_set_##SUFFIX[0], template_##SUFFIX, DATA[i & 1]); \
   } \
}

TEMPLATE_DESCRIPTOR_CASE(1ubo, basic, dbi)
TEMPLATE_DESCRIPTOR_CASE(12ubo, ubo, dbi)
TEMPLATE_DESCRIPTOR_CASE(1ssbo, ssbo, dbi_storage)
TEMPLATE_DESCRIPTOR_CASE(8ssbo, ssbo_many, dbi_storage)
TEMPLATE_DESCRIPTOR_CASE(1combined_sampler, combined_sampler, dii)
TEMPLATE_DESCRIPTOR_CASE(16combined_sampler, combined_sampler_many, dii)
TEMPLATE_DESCRIPTOR_CASE(1sampled_image, sampled_image, dii)
TEMPLATE_DESCRIPTOR_CASE(16sampled_image, sampled_image_many, dii)
TEMPLATE_DESCRIPTOR_CASE(1image, image, dii_storage)
TEMPLATE_DESCRIPTOR_CASE(16image, image_many, dii_storage)
TEMPLATE_DESCRIPTOR_CASE(1texelbuffer, tbo, tbo_views)
TEMPLATE_DESCRIPTOR_CASE(16texelbuffer, tbo_many, tbo_views)
TEMPLATE_DESCRIPTOR_CASE(1imagebuffer, ibo, ibo_views)
TEMPLATE_DESCRIPTOR_CASE(16imagebuffer, ibo_many, ibo_views)

#define PUSH_DESCRIPTOR_CASE(NAME, SUFFIX, DATA) \
static void \
descriptor_template_##NAME##_push(unsigned iterations) \
{ \
   iterations = filter_overflow(descriptor_template_##NAME##_push, iterations, 1); \
   if (!cmdbuf_active) \
      begin_cmdbuf(); \
   for (unsigned i = 0; i < iterations; i++) { \
      VK(CmdPushDescriptorSetWithTemplateKHR)(cmdbuf, template_##SUFFIX##_push, layout_##SUFFIX##_push, 0, DATA[i & 1]); \
   } \
}

PUSH_DESCRIPTOR_CASE(1ubo, basic, dbi)
PUSH_DESCRIPTOR_CASE(12ubo, ubo, dbi)
PUSH_DESCRIPTOR_CASE(1ssbo, ssbo, dbi_storage)
PUSH_DESCRIPTOR_CASE(8ssbo, ssbo_many, dbi_storage)
PUSH_DESCRIPTOR_CASE(1combined_sampler, combined_sampler, dii)
PUSH_DESCRIPTOR_CASE(16combined_sampler, combined_sampler_many, dii)
PUSH_DESCRIPTOR_CASE(1sampled_image, sampled_image, dii)
PUSH_DESCRIPTOR_CASE(16sampled_image, sampled_image_many, dii)
PUSH_DESCRIPTOR_CASE(1image, image, dii_storage)
PUSH_DESCRIPTOR_CASE(16image, image_many, dii_storage)
PUSH_DESCRIPTOR_CASE(1texelbuffer, tbo, tbo_views)
PUSH_DESCRIPTOR_CASE(16texelbuffer, tbo_many, tbo_views)
PUSH_DESCRIPTOR_CASE(1imagebuffer, ibo, ibo_views)
PUSH_DESCRIPTOR_CASE(16imagebuffer, ibo_many, ibo_views)

#define DESCRIPTOR_BUFFER_CASE(NAME, DATA, SIZE, TYPE) \
static void \
descriptor_buffer_offset_##NAME(unsigned iterations) \
{ \
   VkDescriptorGetInfoEXT info = {0}; \
   info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT; \
   info.type = TYPE; \
   /* this should be more than big enough for any single descriptor */ \
   char buf[1024]; \
   for (unsigned i = 0; i < iterations; i++) { \
      /* this is a union of pointers, so the type is irrelevant */ \
      info.data.pSampler = (void*)&DATA[0][i & 1]; \
      VK(GetDescriptorEXT)(dev->dev, &info, dev->info.db_props.SIZE, buf); \
   } \
}

DESCRIPTOR_BUFFER_CASE(ubo, dai, uniformBufferDescriptorSize, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
DESCRIPTOR_BUFFER_CASE(ssbo, dai_storage, storageBufferDescriptorSize, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
DESCRIPTOR_BUFFER_CASE(sampled_image, dii, sampledImageDescriptorSize, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
DESCRIPTOR_BUFFER_CASE(image, dii_storage, storageImageDescriptorSize, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
DESCRIPTOR_BUFFER_CASE(texelbuffer, dai_tbo, uniformTexelBufferDescriptorSize, VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER)
DESCRIPTOR_BUFFER_CASE(imagebuffer, dai_ibo, storageTexelBufferDescriptorSize, VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER)

static void
descriptor_buffer_offset_combined_sampler(unsigned iterations)
{
   size_t combinedSize = dev->info.db_props.combinedImageSamplerDescriptorSize;
   VkDescriptorGetInfoEXT info = {0};
   info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT;
   info.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
   char buf[1024]; //this should be more than big enough for any single descriptor
   if (dev->info.db_props.combinedImageSamplerDescriptorSingleArray) {
      for (unsigned i = 0; i < iterations; i++) {
         /* this is a union of pointers, so the type is irrelevant */
         info.data.pSampler = (void*)&dii[0][i & 1];
         VK(GetDescriptorEXT)(dev->dev, &info, combinedSize, buf);
      }
   } else {
      /* hope you like reduced perf! */
      char buf2[1024];
      for (unsigned i = 0; i < iterations; i++) {
         /* this is a union of pointers, so the type is irrelevant */
         info.data.pSampler = (void*)&dii[0][i & 1];
         VK(GetDescriptorEXT)(dev->dev, &info, combinedSize, buf2);
         memcpy(buf, buf2, dev->info.db_props.samplerDescriptorSize);
         memcpy(&buf[dev->info.db_props.samplerDescriptorSize],
                &buf2[dev->info.db_props.samplerDescriptorSize],
                dev->info.db_props.sampledImageDescriptorSize);
      }
   }
}

static void
descriptor_copy_1ubo(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = 1;
   cds.dstSet = desc_set_basic[0];
   cds.srcSet = desc_set_basic[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_12ubo(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = MAX_UBOS;
   cds.dstSet = desc_set_ubo[0];
   cds.srcSet = desc_set_ubo[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_1ssbo(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = 1;
   cds.dstSet = desc_set_ssbo[0];
   cds.srcSet = desc_set_ssbo[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_8ssbo(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = MAX_SSBOS;
   cds.dstSet = desc_set_ssbo_many[0];
   cds.srcSet = desc_set_ssbo_many[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_1combined_sampler(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = 1;
   cds.dstSet = desc_set_combined_sampler[0];
   cds.srcSet = desc_set_combined_sampler[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_16combined_sampler(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = MAX_SAMPLERS;
   cds.dstSet = desc_set_combined_sampler_many[0];
   cds.srcSet = desc_set_combined_sampler_many[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_1sampled_image(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = 1;
   cds.dstSet = desc_set_sampled_image[0];
   cds.srcSet = desc_set_sampled_image[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_16sampled_image(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = MAX_SAMPLERS;
   cds.dstSet = desc_set_sampled_image_many[0];
   cds.srcSet = desc_set_sampled_image_many[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_1image(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = 1;
   cds.dstSet = desc_set_image[0];
   cds.srcSet = desc_set_image[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_16image(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = MIN2(dev->info.props.limits.maxPerStageDescriptorStorageImages, MAX_IMAGES);
   cds.dstSet = desc_set_image_many[0];
   cds.srcSet = desc_set_image_many[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_1texelbuffer(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = 1;
   cds.dstSet = desc_set_tbo[0];
   cds.srcSet = desc_set_tbo[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_16texelbuffer(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = MAX_SAMPLERS;
   cds.dstSet = desc_set_tbo_many[0];
   cds.srcSet = desc_set_tbo_many[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_1imagebuffer(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = 1;
   cds.dstSet = desc_set_ibo[0];
   cds.srcSet = desc_set_ibo[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_16imagebuffer(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = MIN2(dev->info.props.limits.maxPerStageDescriptorStorageImages, MAX_IMAGES);
   cds.dstSet = desc_set_ibo_many[0];
   cds.srcSet = desc_set_ibo_many[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_mutable_1ubo(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = 1;
   cds.dstSet = desc_set_basic_mutable[0];
   cds.srcSet = desc_set_basic_mutable[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_mutable_12ubo(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = MAX_UBOS;
   cds.dstSet = desc_set_ubo_mutable[0];
   cds.srcSet = desc_set_ubo_mutable[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_mutable_1ssbo(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = 1;
   cds.dstSet = desc_set_ssbo_mutable[0];
   cds.srcSet = desc_set_ssbo_mutable[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_mutable_8ssbo(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = MAX_SSBOS;
   cds.dstSet = desc_set_ssbo_many_mutable[0];
   cds.srcSet = desc_set_ssbo_many_mutable[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_mutable_1combined_sampler(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = 1;
   cds.dstSet = desc_set_combined_sampler_mutable[0];
   cds.srcSet = desc_set_combined_sampler_mutable[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_mutable_16combined_sampler(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = MAX_SAMPLERS;
   cds.dstSet = desc_set_combined_sampler_many_mutable[0];
   cds.srcSet = desc_set_combined_sampler_many_mutable[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_mutable_1sampled_image(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = 1;
   cds.dstSet = desc_set_sampled_image_mutable[0];
   cds.srcSet = desc_set_sampled_image_mutable[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_mutable_16sampled_image(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = MAX_SAMPLERS;
   cds.dstSet = desc_set_sampled_image_many_mutable[0];
   cds.srcSet = desc_set_sampled_image_many_mutable[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_mutable_1image(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = 1;
   cds.dstSet = desc_set_image_mutable[0];
   cds.srcSet = desc_set_image_mutable[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_mutable_16image(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = MIN2(dev->info.props.limits.maxPerStageDescriptorStorageImages, MAX_IMAGES);
   cds.dstSet = desc_set_image_many_mutable[0];
   cds.srcSet = desc_set_image_many_mutable[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_mutable_1texelbuffer(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = 1;
   cds.dstSet = desc_set_tbo_mutable[0];
   cds.srcSet = desc_set_tbo_mutable[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_mutable_16texelbuffer(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = MAX_SAMPLERS;
   cds.dstSet = desc_set_tbo_many_mutable[0];
   cds.srcSet = desc_set_tbo_many_mutable[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_mutable_1imagebuffer(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = 1;
   cds.dstSet = desc_set_ibo_mutable[0];
   cds.srcSet = desc_set_ibo_mutable[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}

static void
descriptor_copy_mutable_16imagebuffer(unsigned iterations)
{
   VkCopyDescriptorSet cds = {0};
   cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
   cds.descriptorCount = MIN2(dev->info.props.limits.maxPerStageDescriptorStorageImages, MAX_IMAGES);
   cds.dstSet = desc_set_ibo_many_mutable[0];
   cds.srcSet = desc_set_ibo_many_mutable[1];
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSets)(dev->dev, 0, NULL, 1, &cds);
   }
}


static void
resolve(unsigned iterations, bool mutable, bool multiple_regions, bool mismatched_regions)
{
   VkImageResolve2 resolve[4] = {0};
   unsigned region_count = multiple_regions ? ARRAY_SIZE(resolve) : 1;
   for (unsigned i = 0; i < region_count; i++) {
      resolve[i].sType = VK_STRUCTURE_TYPE_IMAGE_RESOLVE_2;
      resolve[i].dstSubresource = resolve[i].srcSubresource = default_subresourcerangelayers();
      resolve[i].srcOffset.z = 0;
      resolve[i].srcOffset.x = i * (100 / region_count);
      resolve[i].srcOffset.y = i * (100 / region_count);
      if (mismatched_regions) {
         resolve[i].srcOffset.x = (region_count - i - 1) * (100 / region_count);
         resolve[i].srcOffset.y = (region_count - i - 1) * (100 / region_count);
      } else {
         resolve[i].dstOffset = resolve[i].srcOffset;
      }
      resolve[i].extent.width = 100 / region_count;
      resolve[i].extent.height = 100 / region_count;
      resolve[i].extent.depth = 1;
   }

   VkResolveImageInfo2 r = {0};
   r.sType = VK_STRUCTURE_TYPE_RESOLVE_IMAGE_INFO_2;
   r.srcImage = copy_image_ms[mutable];
   r.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
   r.dstImage = copy_image_dst[mutable];
   r.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
   r.regionCount = region_count;
   r.pRegions = resolve;
   if (!cmdbuf_active)
      begin_cmdbuf();

   VkImageMemoryBarrier imb[2] = {0};
   imb[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
   imb[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
   imb[0].image = copy_image_ms[mutable];
   imb[1].image = copy_image_dst[mutable];
   imb[1].subresourceRange = imb[0].subresourceRange = default_subresourcerange();

   for (unsigned i = 0; i < iterations; i++, count++) {
      imb[0].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      imb[0].dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
      imb[0].oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      imb[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      imb[1].srcAccessMask = 0;
      imb[1].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      imb[1].oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
      imb[1].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      VK(CmdPipelineBarrier)(cmdbuf, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 2, imb);
      VK(CmdResolveImage2)(cmdbuf, &r);
      imb[0].srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
      imb[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      imb[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      imb[0].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      imb[1].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      imb[1].dstAccessMask = 0;
      imb[1].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      imb[1].newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
      VK(CmdPipelineBarrier)(cmdbuf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 2, imb);
   }
}

static void
misc_resolve(unsigned iterations)
{
   iterations = filter_overflow(misc_resolve, iterations, 1);
   if (!cmdbuf_active)
      begin_cmdbuf();
   resolve(iterations, false, false, false);
}

static void
misc_resolve_4region(unsigned iterations)
{
   iterations = filter_overflow(misc_resolve_4region, iterations, 1);
   if (!cmdbuf_active)
      begin_cmdbuf();
   resolve(iterations, false, true, false);
}

static void
misc_resolve_4region_mismatched(unsigned iterations)
{
   iterations = filter_overflow(misc_resolve_4region_mismatched, iterations, 1);
   if (!cmdbuf_active)
      begin_cmdbuf();
   resolve(iterations, false, true, true);
}

static void
misc_resolve_mutable(unsigned iterations)
{
   iterations = filter_overflow(misc_resolve_mutable, iterations, 1);
   if (!cmdbuf_active)
      begin_cmdbuf();
   resolve(iterations, true, false, false);
}

static void
misc_resolve_mutable_4region(unsigned iterations)
{
   iterations = filter_overflow(misc_resolve_mutable_4region, iterations, 1);
   if (!cmdbuf_active)
      begin_cmdbuf();
   resolve(iterations, true, true, false);
}

static void
misc_resolve_mutable_4region_mismatched(unsigned iterations)
{
   iterations = filter_overflow(misc_resolve_mutable_4region_mismatched, iterations, 1);
   if (!cmdbuf_active)
      begin_cmdbuf();
   resolve(iterations, true, true, true);
}


static void
copy(unsigned iterations, bool mutable, bool multiple_regions, bool mismatched_regions)
{
   VkImageCopy2 copy[4] = {0};
   unsigned region_count = multiple_regions ? ARRAY_SIZE(copy) : 1;
   for (unsigned i = 0; i < region_count; i++) {
      copy[i].sType = VK_STRUCTURE_TYPE_IMAGE_COPY_2;
      copy[i].dstSubresource = copy[i].srcSubresource = default_subresourcerangelayers();
      copy[i].srcOffset.z = 0;
      copy[i].srcOffset.x = i * (100 / region_count);
      copy[i].srcOffset.y = i * (100 / region_count);
      if (mismatched_regions) {
         copy[i].srcOffset.x = (region_count - i - 1) * (100 / region_count);
         copy[i].srcOffset.y = (region_count - i - 1) * (100 / region_count);
      } else {
         copy[i].dstOffset = copy[i].srcOffset;
      }
      copy[i].extent.width = 100 / region_count;
      copy[i].extent.height = 100 / region_count;
      copy[i].extent.depth = 1;
   }

   VkCopyImageInfo2 r = {0};
   r.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2;
   r.srcImage = copy_image_src[mutable];
   r.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
   r.dstImage = copy_image_dst[mutable];
   r.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
   r.regionCount = region_count;
   r.pRegions = copy;
   if (!cmdbuf_active)
      begin_cmdbuf();

   VkImageMemoryBarrier imb[2] = {0};
   imb[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
   imb[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
   imb[0].image = copy_image_src[mutable];
   imb[1].image = copy_image_dst[mutable];
   imb[1].subresourceRange = imb[0].subresourceRange = default_subresourcerange();

   for (unsigned i = 0; i < iterations; i++, count++) {
      imb[0].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      imb[0].dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
      imb[0].oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      imb[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      imb[1].srcAccessMask = 0;
      imb[1].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      imb[1].oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
      imb[1].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      VK(CmdPipelineBarrier)(cmdbuf, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 2, imb);
      VK(CmdCopyImage2)(cmdbuf, &r);
      imb[0].srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
      imb[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      imb[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      imb[0].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      imb[1].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      imb[1].dstAccessMask = 0;
      imb[1].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      imb[1].newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
      VK(CmdPipelineBarrier)(cmdbuf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 2, imb);
   }
}

static void
misc_copy(unsigned iterations)
{
   iterations = filter_overflow(misc_copy, iterations, 1);
   if (!cmdbuf_active)
      begin_cmdbuf();
   copy(iterations, false, false, false);
}

static void
misc_copy_4region(unsigned iterations)
{
   iterations = filter_overflow(misc_copy_4region, iterations, 1);
   if (!cmdbuf_active)
      begin_cmdbuf();
   copy(iterations, false, true, false);
}

static void
misc_copy_4region_mismatched(unsigned iterations)
{
   iterations = filter_overflow(misc_copy_4region_mismatched, iterations, 1);
   if (!cmdbuf_active)
      begin_cmdbuf();
   copy(iterations, false, true, true);
}

static void
misc_copy_mutable(unsigned iterations)
{
   iterations = filter_overflow(misc_copy_mutable, iterations, 1);
   if (!cmdbuf_active)
      begin_cmdbuf();
   copy(iterations, true, false, false);
}

static void
misc_copy_mutable_4region(unsigned iterations)
{
   iterations = filter_overflow(misc_copy_mutable_4region, iterations, 1);
   if (!cmdbuf_active)
      begin_cmdbuf();
   copy(iterations, true, true, false);
}

static void
misc_copy_mutable_4region_mismatched(unsigned iterations)
{
   iterations = filter_overflow(misc_copy_mutable_4region_mismatched, iterations, 1);
   if (!cmdbuf_active)
      begin_cmdbuf();
   copy(iterations, true, true, true);
}

static VkGraphicsPipelineCreateInfo depthonly_pci;
static void
misc_compile_fastlink_depthonly(unsigned iterations)
{
   iterations = filter_overflow(misc_compile_fastlink_depthonly, iterations, 1);
   if (!cmdbuf_active)
      begin_cmdbuf();
   cleanup_func = reset_gpl;

   depthonly_pci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
   depthonly_pci.layout = depthonly_layout;
   VkPipelineLibraryCreateInfoKHR PIPELINE_LIBRARY_CREATE_INFO_KHR;
   depthonly_pci.pNext = &PIPELINE_LIBRARY_CREATE_INFO_KHR;
   PIPELINE_LIBRARY_CREATE_INFO_KHR.sType = 1000290000;
   PIPELINE_LIBRARY_CREATE_INFO_KHR.pNext = NULL;
   PIPELINE_LIBRARY_CREATE_INFO_KHR.libraryCount = 4;
   PIPELINE_LIBRARY_CREATE_INFO_KHR.pLibraries = depthonly_pipelines;

   for (unsigned i = 0; i < iterations; i++, count++) {
      VkPipeline pipeline;
      VkResult result = VK(CreateGraphicsPipelines)(dev->dev, VK_NULL_HANDLE, 1, &depthonly_pci, NULL, &pipeline);
      VK_CHECK("CreateGraphicsPipelines", result);
#if VK_USE_64_BIT_PTR_DEFINES==1
      pools[cmdbuf_pool_idx].trash_ptrs[cmdbuf_idx][count] = (int64_t*)pipeline;
#else
      pools[cmdbuf_pool_idx].trash_ptrs[cmdbuf_idx][count] = (void*)pipeline;
#endif
   }
}

static VkGraphicsPipelineCreateInfo slow_pci;
static void
misc_compile_fastlink_slow(unsigned iterations)
{
   iterations = filter_overflow(misc_compile_fastlink_slow, iterations, 1);
   if (!cmdbuf_active)
      begin_cmdbuf();
   cleanup_func = reset_gpl;

   slow_pci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
   slow_pci.layout = slow_layout;
   VkPipelineLibraryCreateInfoKHR PIPELINE_LIBRARY_CREATE_INFO_KHR;
   slow_pci.pNext = &PIPELINE_LIBRARY_CREATE_INFO_KHR;
   PIPELINE_LIBRARY_CREATE_INFO_KHR.sType = 1000290000;
   PIPELINE_LIBRARY_CREATE_INFO_KHR.pNext = NULL;
   PIPELINE_LIBRARY_CREATE_INFO_KHR.libraryCount = 4;
   PIPELINE_LIBRARY_CREATE_INFO_KHR.pLibraries = slow_pipelines;

   for (unsigned i = 0; i < iterations; i++, count++) {
      VkPipeline pipeline;
      VkResult result = VK(CreateGraphicsPipelines)(dev->dev, VK_NULL_HANDLE, 1, &slow_pci, NULL, &pipeline);
      VK_CHECK("CreateGraphicsPipelines", result);
#if VK_USE_64_BIT_PTR_DEFINES==1
      pools[cmdbuf_pool_idx].trash_ptrs[cmdbuf_idx][count] = (int64_t*)pipeline;
#else
      pools[cmdbuf_pool_idx].trash_ptrs[cmdbuf_idx][count] = (void*)pipeline;
#endif
   }
}

static void
misc_zerovram(unsigned iterations)
{
   iterations = filter_overflow(misc_zerovram, iterations, 1);
   if (!cmdbuf_active)
      begin_cmdbuf();

   assert(cmdbuf_idx == 0);

   VkBufferCopy region = {
      32,
      0,
      4
   };

   for (unsigned i = 0; i < iterations; i++, count++) {
      zerovram_buffers[cmdbuf_pool_idx][i] = create_copy_buffer(&zerovram_mem[cmdbuf_pool_idx][i]);
      VK(CmdCopyBuffer)(cmdbuf, zerovram_buffers[cmdbuf_pool_idx][i], ssbo[cmdbuf_pool_idx], 1, &region);
   }
}

static void
misc_zerovram_manual(unsigned iterations)
{
   iterations = filter_overflow(misc_zerovram_manual, iterations, 1);
   if (!cmdbuf_active)
      begin_cmdbuf();

   assert(cmdbuf_idx == 0);
   for (unsigned i = 0; i < iterations; i++, count++) {
      zerovram_buffers[cmdbuf_pool_idx][i] = create_copy_buffer(&zerovram_mem[cmdbuf_pool_idx][i]);
      VK(CmdFillBuffer)(cmdbuf, zerovram_buffers[cmdbuf_pool_idx][i], 0, 256 * 1024 * 1024, 0);
   }
}


struct perf_case {
   const char *name;
   perf_rate_func func;
   VkPipeline *pipelines;
   bool (*check_func)(void);
};

#define CASE_BASIC(name, ...) {#name, name, pipelines_basic, __VA_ARGS__}
#define CASE_DYN_BASIC(name, ...) {#name, name, pipelines_dyn, __VA_ARGS__}
#define CASE_MULTIRT(name, ...) {#name, name, &pipeline_multirt, __VA_ARGS__}
#define CASE_DYN_MULTIRT(name, ...) {#name, name, &pipeline_multrt_dyn, __VA_ARGS__}
#define CASE_VATTRIB(name, ...) {#name, name, pipelines_vattrib, __VA_ARGS__}
#define CASE_VATTRIB_DYNAMIC(name, ...) {#name, name, &pipeline_vattrib_dynamic, __VA_ARGS__}
#define CASE_VATTRIB_GPL(name, ...) {#name, name, pipelines_basic, __VA_ARGS__}
#define CASE_UBO(name, ...) {#name, name, &pipeline_ubo, __VA_ARGS__}
#define CASE_SSBO(name, ...) {#name, name, &pipeline_ssbo, __VA_ARGS__}
#define CASE_SSBO_MANY(name, ...) {#name, name, &pipeline_ssbo_many, __VA_ARGS__}
#define CASE_SAMPLER(name, ...) {#name, name, &pipeline_combined_sampler, __VA_ARGS__}
#define CASE_SAMPLER_MANY(name, ...) {#name, name, &pipeline_combined_sampler_many, __VA_ARGS__}
#define CASE_IMAGE(name, ...) {#name, name, &pipeline_image, __VA_ARGS__}
#define CASE_IMAGE_MANY(name, ...) {#name, name, &pipeline_image_many, __VA_ARGS__}
#define CASE_TBO(name, ...) {#name, name, &pipeline_tbo, __VA_ARGS__}
#define CASE_TBO_MANY(name, ...) {#name, name, &pipeline_tbo_many, __VA_ARGS__}
#define CASE_IBO(name, ...) {#name, name, &pipeline_ibo, __VA_ARGS__}
#define CASE_IBO_MANY(name, ...) {#name, name, &pipeline_ibo_many, __VA_ARGS__}
#define CASE_DB(name, pipeline) {#name, name, &pipeline, check_descriptor_buffer}

static struct perf_case cases_draw[] = {
   CASE_BASIC(draw),
   CASE_BASIC(draw_multi, check_multi_draw),
   CASE_BASIC(draw_vertex),
   CASE_BASIC(draw_multi_vertex, check_multi_draw),
   CASE_BASIC(draw_index_change),
   CASE_BASIC(draw_index_offset_change),
   CASE_BASIC(draw_rp_begin_end),
   CASE_DYN_BASIC(draw_rp_begin_end_dynrender, check_dynamic_rendering),
   CASE_BASIC(draw_rp_begin_end_dontcare),
   CASE_DYN_BASIC(draw_rp_begin_end_dontcare_dynrender, check_dynamic_rendering),
   CASE_MULTIRT(draw_multirt),
   CASE_DYN_MULTIRT(draw_multirt_dynrender, check_dynamic_rendering),
   CASE_MULTIRT(draw_multirt_begin_end),
   CASE_DYN_MULTIRT(draw_multirt_begin_end_dynrender, check_dynamic_rendering),
   CASE_MULTIRT(draw_multirt_begin_end_dontcare),
   CASE_DYN_MULTIRT(draw_multirt_begin_end_dontcare_dynrender, check_dynamic_rendering),
   CASE_BASIC(draw_vbo_change),
   CASE_BASIC(draw_1vattrib_change),
   CASE_VATTRIB(draw_16vattrib),
   CASE_VATTRIB(draw_16vattrib_16vbo_change),
   CASE_VATTRIB(draw_16vattrib_change),
   CASE_VATTRIB_DYNAMIC(draw_16vattrib_change_dynamic, check_dynamic_vertex_input),
   CASE_VATTRIB_GPL(draw_16vattrib_change_gpl, check_graphics_pipeline_library),
   CASE_VATTRIB_GPL(draw_16vattrib_change_gpl_hashncache, check_graphics_pipeline_library),
   CASE_BASIC(draw_1ubo_change),
   CASE_UBO(draw_12ubo_change),
   CASE_SAMPLER(draw_1combined_sampler_change),
   CASE_SAMPLER_MANY(draw_16combined_sampler_change),
   CASE_TBO(draw_1texelbuffer_change),
   CASE_TBO_MANY(draw_16texelbuffer_change),
   CASE_SSBO(draw_1ssbo_change),
   CASE_SSBO_MANY(draw_8ssbo_change),
   CASE_IMAGE(draw_1image_change),
   CASE_IMAGE_MANY(draw_16image_change),
   CASE_IBO(draw_1imagebuffer_change),
   CASE_IBO_MANY(draw_16imagebuffer_change),
   CASE_DB(draw_ubo_db_change, pipeline_ubo_db),
   CASE_DB(draw_ssbo_db_change, pipeline_ssbo_many_db),
   CASE_DB(draw_combined_sampler_db_change, pipeline_combined_sampler_many_db),
   CASE_DB(draw_image_db_change, pipeline_image_many_db),

};

#define CASE_SUBMIT(name, ...) {#name, name, pipelines_basic, __VA_ARGS__}

static struct perf_case cases_submit[] = {
   CASE_SUBMIT(submit_noop),
   CASE_SUBMIT(submit_50noop),
   CASE_SUBMIT(submit_1cmdbuf),
   CASE_SUBMIT(submit_50cmdbuf),
   CASE_SUBMIT(submit_50cmdbuf_50submit),
};

#define CASE_DESCRIPTOR(name, ...) {#name, name, pipelines_basic, __VA_ARGS__}
#define CASE_DESCRIPTOR_TEMPLATE(name) {#name, name, pipelines_basic, check_descriptor_template}
#define CASE_DESCRIPTOR_PUSH(name) {#name, name, pipelines_basic, check_push_descriptor}
#define CASE_DESCRIPTOR_BUFFER(name) {#name, name, pipelines_basic, check_descriptor_buffer}

static struct perf_case cases_descriptor[] = {
   CASE_DESCRIPTOR(descriptor_noop),
   CASE_DESCRIPTOR(descriptor_1ubo),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_1ubo),
   CASE_DESCRIPTOR_PUSH(descriptor_template_1ubo_push),
   CASE_DESCRIPTOR(descriptor_12ubo),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_12ubo),
   CASE_DESCRIPTOR_PUSH(descriptor_template_12ubo_push),
   CASE_DESCRIPTOR(descriptor_1combined_sampler),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_1combined_sampler),
   CASE_DESCRIPTOR_PUSH(descriptor_template_1combined_sampler_push),
   CASE_DESCRIPTOR(descriptor_16combined_sampler),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_16combined_sampler),
   CASE_DESCRIPTOR_PUSH(descriptor_template_16combined_sampler_push),
   CASE_DESCRIPTOR(descriptor_1sampled_image),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_1sampled_image),
   CASE_DESCRIPTOR_PUSH(descriptor_template_1sampled_image_push),
   CASE_DESCRIPTOR(descriptor_16sampled_image),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_16sampled_image),
   CASE_DESCRIPTOR_PUSH(descriptor_template_16sampled_image_push),
   CASE_DESCRIPTOR(descriptor_1texelbuffer),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_1texelbuffer),
   CASE_DESCRIPTOR_PUSH(descriptor_template_1texelbuffer_push),
   CASE_DESCRIPTOR(descriptor_16texelbuffer),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_16texelbuffer),
   CASE_DESCRIPTOR_PUSH(descriptor_template_16texelbuffer_push),
   CASE_DESCRIPTOR(descriptor_1ssbo),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_1ssbo),
   CASE_DESCRIPTOR_PUSH(descriptor_template_1ssbo_push),
   CASE_DESCRIPTOR(descriptor_8ssbo),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_8ssbo),
   CASE_DESCRIPTOR_PUSH(descriptor_template_8ssbo_push),
   CASE_DESCRIPTOR(descriptor_1image),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_1image),
   CASE_DESCRIPTOR_PUSH(descriptor_template_1image_push),
   CASE_DESCRIPTOR(descriptor_16image),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_16image),
   CASE_DESCRIPTOR_PUSH(descriptor_template_16image_push),
   CASE_DESCRIPTOR(descriptor_1imagebuffer),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_1imagebuffer),
   CASE_DESCRIPTOR_PUSH(descriptor_template_1imagebuffer_push),
   CASE_DESCRIPTOR(descriptor_16imagebuffer),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_16imagebuffer),
   CASE_DESCRIPTOR_PUSH(descriptor_template_16imagebuffer_push),
   CASE_DESCRIPTOR_BUFFER(descriptor_buffer_offset_ubo),
   CASE_DESCRIPTOR_BUFFER(descriptor_buffer_offset_combined_sampler),
   CASE_DESCRIPTOR_BUFFER(descriptor_buffer_offset_sampled_image),
   CASE_DESCRIPTOR_BUFFER(descriptor_buffer_offset_texelbuffer),
   CASE_DESCRIPTOR_BUFFER(descriptor_buffer_offset_ssbo),
   CASE_DESCRIPTOR_BUFFER(descriptor_buffer_offset_image),
   CASE_DESCRIPTOR_BUFFER(descriptor_buffer_offset_imagebuffer),
   CASE_DESCRIPTOR(descriptor_copy_1ubo),
   CASE_DESCRIPTOR(descriptor_copy_12ubo),
   CASE_DESCRIPTOR(descriptor_copy_1combined_sampler),
   CASE_DESCRIPTOR(descriptor_copy_16combined_sampler),
   CASE_DESCRIPTOR(descriptor_copy_1sampled_image),
   CASE_DESCRIPTOR(descriptor_copy_16sampled_image),
   CASE_DESCRIPTOR(descriptor_copy_1texelbuffer),
   CASE_DESCRIPTOR(descriptor_copy_16texelbuffer),
   CASE_DESCRIPTOR(descriptor_copy_1ssbo),
   CASE_DESCRIPTOR(descriptor_copy_8ssbo),
   CASE_DESCRIPTOR(descriptor_copy_1image),
   CASE_DESCRIPTOR(descriptor_copy_16image),
   CASE_DESCRIPTOR(descriptor_copy_1imagebuffer),
   CASE_DESCRIPTOR(descriptor_copy_16imagebuffer),
   CASE_DESCRIPTOR(descriptor_copy_mutable_1ubo, check_mutable_descriptor),
   CASE_DESCRIPTOR(descriptor_copy_mutable_12ubo, check_mutable_descriptor),
   CASE_DESCRIPTOR(descriptor_copy_mutable_1combined_sampler, check_mutable_descriptor),
   CASE_DESCRIPTOR(descriptor_copy_mutable_16combined_sampler, check_mutable_descriptor),
   CASE_DESCRIPTOR(descriptor_copy_mutable_1sampled_image, check_mutable_descriptor),
   CASE_DESCRIPTOR(descriptor_copy_mutable_16sampled_image, check_mutable_descriptor),
   CASE_DESCRIPTOR(descriptor_copy_mutable_1texelbuffer, check_mutable_descriptor),
   CASE_DESCRIPTOR(descriptor_copy_mutable_16texelbuffer, check_mutable_descriptor),
   CASE_DESCRIPTOR(descriptor_copy_mutable_1ssbo, check_mutable_descriptor),
   CASE_DESCRIPTOR(descriptor_copy_mutable_8ssbo, check_mutable_descriptor),
   CASE_DESCRIPTOR(descriptor_copy_mutable_1image, check_mutable_descriptor),
   CASE_DESCRIPTOR(descriptor_copy_mutable_16image, check_mutable_descriptor),
   CASE_DESCRIPTOR(descriptor_copy_mutable_1imagebuffer, check_mutable_descriptor),
   CASE_DESCRIPTOR(descriptor_copy_mutable_16imagebuffer, check_mutable_descriptor),
};

#define CASE_MISC(name, ...) {#name, name, NULL, __VA_ARGS__}
static struct perf_case cases_misc[] = {
   CASE_MISC(misc_resolve),
   CASE_MISC(misc_resolve_4region),
   CASE_MISC(misc_resolve_4region_mismatched),
   CASE_MISC(misc_resolve_mutable),
   CASE_MISC(misc_resolve_mutable_4region),
   CASE_MISC(misc_resolve_mutable_4region_mismatched),
   CASE_MISC(misc_copy),
   CASE_MISC(misc_copy_4region),
   CASE_MISC(misc_copy_4region_mismatched),
   CASE_MISC(misc_copy_mutable),
   CASE_MISC(misc_copy_mutable_4region),
   CASE_MISC(misc_copy_mutable_4region_mismatched),
   CASE_MISC(misc_compile_fastlink_depthonly, check_dota2),
   CASE_MISC(misc_compile_fastlink_slow, check_dota2),
   CASE_MISC(misc_zerovram, check_zerovram),
   CASE_MISC(misc_zerovram_manual, check_zerovram),
};

#define TOTAL_CASES (ARRAY_SIZE(cases_draw) + ARRAY_SIZE(cases_submit) + ARRAY_SIZE(cases_descriptor) + ARRAY_SIZE(cases_misc))

static void
set_image_layout(VkImage image, VkImageLayout ly)
{
   VkImageMemoryBarrier imb = {0};
   imb.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
   imb.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
   imb.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
   imb.newLayout = ly;
   imb.image = image;
   imb.subresourceRange = default_subresourcerange();
   VK(CmdPipelineBarrier)(cmdbuf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &imb);
}

static void
init_dyn_att(VkRenderingAttachmentInfo *att)
{
   VkImage image;
   att->sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
   att->imageView = create_rt(&image, false);
   att->imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
   att->storeOp = VK_ATTACHMENT_STORE_OP_STORE;

   set_image_layout(image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

static void
setup_image(VkImage image, VkImageLayout final_layout)
{
   VkClearColorValue clear = {0};
   VkImageSubresourceRange range = default_subresourcerange();
   set_image_layout(image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
   VK(CmdClearColorImage)(cmdbuf, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear, 1, &range);
   set_image_layout(image, final_layout);
}

static VkDeviceAddress
get_bda(VkBuffer b)
{
   VkBufferDeviceAddressInfo info = {
      VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
      NULL,
      b
   };
   return VK(GetBufferDeviceAddress)(dev->dev, &info);
}

static void
setup(void)
{
   if (submit_only)
      goto only_submit;
   /* initialize textures */
   VkImage tex_image[MAX_SAMPLERS];
   for (unsigned i = 0; i < ARRAY_SIZE(tex); i++) {
      tex[i] = create_tex(&tex_image[i]);
      setup_image(tex_image[i], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
   }
   /* initialize storage images */
   VkImage storage_image[MAX_IMAGES];
   for (unsigned i = 0; i < ARRAY_SIZE(storage_image); i++) {
      img[i] = create_storage_image(&storage_image[i]);
      setup_image(storage_image[i], VK_IMAGE_LAYOUT_GENERAL);
   }
   for (unsigned i = 0; i < ARRAY_SIZE(copy_image_src); i++) {
      create_rt(&copy_image_src[i], i);
      setup_image(copy_image_src[i], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
   }
   for (unsigned i = 0; i < ARRAY_SIZE(copy_image_dst); i++) {
      create_rt(&copy_image_dst[i], i);
      setup_image(copy_image_dst[i], VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
   }
   for (unsigned i = 0; i < ARRAY_SIZE(copy_image_ms); i++) {
      create_rt_ms(&copy_image_ms[i], VK_SAMPLE_COUNT_4_BIT, false);
      setup_image(copy_image_ms[i], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
   }
   /* zero the storage buffers */
   for (unsigned i = 0; i < ARRAY_SIZE(ssbo); i++)
      VK(CmdFillBuffer)(cmdbuf, ssbo[i], 0, VK_WHOLE_SIZE, 0);
   /* zero the texel buffers */
   for (unsigned i = 0; i < ARRAY_SIZE(tbo); i++)
      VK(CmdFillBuffer)(cmdbuf, tbo[i], 0, VK_WHOLE_SIZE, 0);
   /* zero the image buffers */
   for (unsigned i = 0; i < ARRAY_SIZE(ibo); i++)
      VK(CmdFillBuffer)(cmdbuf, ibo[i], 0, VK_WHOLE_SIZE, 0);
only_submit:
   /* zero the vertex buffers */
   for (unsigned i = 0; i < ARRAY_SIZE(vbo); i++)
      VK(CmdFillBuffer)(cmdbuf, vbo[i], 0, VK_WHOLE_SIZE, 0);
   /* zero the uniform buffers */
   for (unsigned i = 0; i < ARRAY_SIZE(ubo); i++)
      VK(CmdFillBuffer)(cmdbuf, ubo[i], 0, VK_WHOLE_SIZE, 0);
   /* zero the index buffer */
   for (unsigned i = 0; i < ARRAY_SIZE(index_bo); i++)
      VK(CmdFillBuffer)(cmdbuf, index_bo[i], 0, VK_WHOLE_SIZE, 0);
   /* synchronize */
   VkMemoryBarrier bmb = {0};
   bmb.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
   bmb.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
   bmb.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT;
   VK(CmdPipelineBarrier)(cmdbuf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 1, &bmb, 0, NULL, 0, NULL);

   for (unsigned i = 0; i < MAX_UBOS; i++) {
      dbi[0][i].range = VK_WHOLE_SIZE;
      dbi[1][i].range = VK_WHOLE_SIZE;
      dbi[0][i].buffer = ubo[i];
      dbi[1][MAX_UBOS - 1 - i].buffer = ubo[i];
      if (dev->info.have_KHR_buffer_device_address) {
         ubo_bda[i] = get_bda(ubo[i]);
         for (unsigned j = 0; j < ARRAY_SIZE(dai); j++) {
            dai[j][i].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_ADDRESS_INFO_EXT;
            dai[j][i].range = BUFFER_SIZE;
         }
         dai[0][i].address = ubo_bda[i];
         dai[1][MAX_UBOS - 1 - i].address = ubo_bda[i];
      }
   }

   VkWriteDescriptorSet wds = {0};
   wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

   wds.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
   wds.descriptorCount = 1;
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_basic); i++) {
      wds.pBufferInfo = &dbi[0][i];
      wds.dstSet = desc_set_basic[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      if (dev->info.have_EXT_mutable_descriptor_type) {
         wds.dstSet = desc_set_basic_mutable[i];
         VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      }
   }

   if (submit_only)
      return;

   for (unsigned i = 0; i < MAX_SSBOS; i++) {
      dbi_storage[0][i].range = VK_WHOLE_SIZE;
      dbi_storage[1][i].range = VK_WHOLE_SIZE;
      dbi_storage[0][i].buffer = ssbo[i];
      dbi_storage[1][MAX_SSBOS - 1 - i].buffer = ssbo[i];
      if (dev->info.have_KHR_buffer_device_address) {
         ssbo_bda[i] = get_bda(ssbo[i]);
         for (unsigned j = 0; j < ARRAY_SIZE(dai_storage); j++) {
            dai_storage[j][i].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_ADDRESS_INFO_EXT;
            dai_storage[j][i].range = BUFFER_SIZE;
         }
         dai_storage[0][i].address = ssbo_bda[i];
         dai_storage[1][MAX_SSBOS - 1 - i].address = ssbo_bda[i];
      }
   }
   for (unsigned i = 0; i < MAX_SAMPLERS; i++) {
      dii[1][i].sampler = dii[0][i].sampler = sampler;
      dii[1][MAX_SAMPLERS - 1 - i].imageView = dii[0][i].imageView = tex[i];
      dii[1][i].imageLayout = dii[0][i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      tbo_views[1][MAX_SAMPLERS - 1 - i] = tbo_views[0][i] = create_bufferview(tbo[i]);
      if (dev->info.have_KHR_buffer_device_address) {
         tbo_bda[i] = get_bda(tbo[i]);
         for (unsigned j = 0; j < ARRAY_SIZE(dai_tbo); j++) {
            dai_tbo[j][i].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_ADDRESS_INFO_EXT;
            dai_tbo[j][i].range = BUFFER_SIZE;
            dai_tbo[j][i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
         }
         dai_tbo[0][i].address = tbo_bda[i];
         dai_tbo[1][MAX_SAMPLERS - 1 - i].address = tbo_bda[i];
      }
   }
   for (unsigned i = 0; i < MAX_IMAGES; i++) {
      dii_storage[1][MAX_IMAGES - 1 - i].imageView = dii_storage[0][i].imageView = img[i];
      dii_storage[1][i].imageLayout = dii_storage[0][i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
      ibo_views[1][MAX_IMAGES - 1 - i] = ibo_views[0][i] = create_bufferview(ibo[i]);
      if (dev->info.have_KHR_buffer_device_address) {
         ibo_bda[i] = get_bda(ibo[i]);
         for (unsigned j = 0; j < ARRAY_SIZE(dai_ibo); j++) {
            dai_ibo[j][i].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_ADDRESS_INFO_EXT;
            dai_ibo[j][i].range = BUFFER_SIZE;
            dai_ibo[j][i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
         }
         dai_ibo[0][i].address = ibo_bda[i];
         dai_ibo[1][MAX_IMAGES - 1 - i].address = ibo_bda[i];
      }
   }

   wds.descriptorCount = MAX_UBOS;
   wds.pBufferInfo = dbi[0];
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_ubo); i++) {
      wds.dstSet = desc_set_ubo[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      if (dev->info.have_EXT_mutable_descriptor_type) {
         wds.dstSet = desc_set_ubo_mutable[i];
         VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      }
   }

   wds.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
   wds.descriptorCount = 1;
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_ssbo); i++) {
      wds.pBufferInfo = dbi_storage[i];
      wds.dstSet = desc_set_ssbo[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      if (dev->info.have_EXT_mutable_descriptor_type) {
         wds.dstSet = desc_set_ssbo_mutable[i];
         VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      }
   }

   wds.descriptorCount = MAX_SSBOS;
   wds.pBufferInfo = dbi_storage[0];
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_ssbo_many); i++) {
      wds.dstSet = desc_set_ssbo_many[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      if (dev->info.have_EXT_mutable_descriptor_type) {
         wds.dstSet = desc_set_ssbo_many_mutable[i];
         VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      }
   }

   wds.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
   wds.descriptorCount = 1;
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_combined_sampler); i++) {
      wds.pImageInfo = dii[i];
      wds.dstSet = desc_set_combined_sampler[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      if (dev->info.have_EXT_mutable_descriptor_type) {
         wds.dstSet = desc_set_combined_sampler_mutable[i];
         VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      }
   }

   wds.descriptorCount = MAX_SAMPLERS;
   wds.pImageInfo = dii[0];
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_combined_sampler_many); i++) {
      wds.dstSet = desc_set_combined_sampler_many[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      if (dev->info.have_EXT_mutable_descriptor_type) {
         wds.dstSet = desc_set_combined_sampler_many_mutable[i];
         VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      }
   }

   wds.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
   wds.descriptorCount = 1;
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_tbo); i++) {
      wds.pTexelBufferView = tbo_views[i];
      wds.dstSet = desc_set_tbo[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      if (dev->info.have_EXT_mutable_descriptor_type) {
         wds.dstSet = desc_set_tbo_mutable[i];
         VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      }
   }

   wds.descriptorCount = MAX_SAMPLERS;
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_tbo_many); i++) {
      wds.pTexelBufferView = tbo_views[i];
      wds.dstSet = desc_set_tbo_many[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      if (dev->info.have_EXT_mutable_descriptor_type) {
         wds.dstSet = desc_set_tbo_many_mutable[i];
         VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      }
   }

   wds.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
   wds.descriptorCount = 1;
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_image); i++) {
      wds.pImageInfo = dii_storage[i];
      wds.dstSet = desc_set_image[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      if (dev->info.have_EXT_mutable_descriptor_type) {
         wds.dstSet = desc_set_image_mutable[i];
         VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      }
   }

   wds.descriptorCount = MIN2(dev->info.props.limits.maxPerStageDescriptorStorageImages, MAX_IMAGES);
   wds.pImageInfo = dii_storage[0];
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_image_many); i++) {
      wds.dstSet = desc_set_image_many[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      if (dev->info.have_EXT_mutable_descriptor_type) {
         wds.dstSet = desc_set_image_many_mutable[i];
         VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      }
   }

   wds.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
   wds.descriptorCount = 1;
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_ibo); i++) {
      wds.pTexelBufferView = ibo_views[i];
      wds.dstSet = desc_set_ibo[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      if (dev->info.have_EXT_mutable_descriptor_type) {
         wds.dstSet = desc_set_ibo_mutable[i];
         VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      }
   }

   wds.descriptorCount = MIN2(dev->info.props.limits.maxPerStageDescriptorStorageImages, MAX_IMAGES);
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_ibo_many); i++) {
      wds.pTexelBufferView = ibo_views[i];
      wds.dstSet = desc_set_ibo_many[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      if (dev->info.have_EXT_mutable_descriptor_type) {
         wds.dstSet = desc_set_ibo_many_mutable[i];
         VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
      }
   }

   if (!dev->info.have_KHR_dynamic_rendering)
      return;

   for (unsigned i = 0; i < ARRAY_SIZE(dyn_att_basic); i++)
      init_dyn_att(&dyn_att_basic[i]);
   for (unsigned i = 0; i < ARRAY_SIZE(dyn_att_multirt); i++)
      init_dyn_att(&dyn_att_multirt[i]);
}

#define COLOR_RESET	"\033[0m"
#define COLOR_RED	"\033[31m"
#define COLOR_GREEN	"\033[1;32m"
#define COLOR_YELLOW	"\033[1;33m"
#define COLOR_CYAN	"\033[1;36m"

void
print_table_header(bool csv, char* column_1, char* column_2, char* column_3, char* column_4)
{
   if (csv) {
      printf(" %3s, %-51s, %8s, %5s\n", column_1, column_2, column_3, column_4);
   } else { // Markdown
      printf("\n| %3s | %-51s | %8s | %5s |\n", column_1, column_2, column_3, column_4);
      printf("| --: | --------------------------------------------------- | -------: | -----: |\n");
   }
}

void
print_table_row(bool csv, bool color, unsigned int number, const char* name, uint64_t result, const char* ratio_color, double ratio)
{
   if (csv)
      printf(" %3u, %-51s, %s%8"PRIu64"%s, %s%5.1f%%%s\n",
             number,
             name,
             color ? COLOR_CYAN : "",
             result,
             color ? COLOR_RESET : "",
             color ? ratio_color : "",
             ratio,
             color ? COLOR_RESET : "");
   else
      printf("| %3u | %-51s | %s%8"PRIu64"%s | %s%5.1f%%%s |\n",
             number,
             name,
             color ? COLOR_CYAN : "",
             result,
             color ? COLOR_RESET : "",
             color ? ratio_color : "",
             ratio,
             color ? COLOR_RESET : "");
   return;
}

void
print_table_row_unsupported(bool csv, bool color, unsigned int number, const char* name, const char* driver_name)
{
   if (csv)
      printf(" %3u, %-51s, %s%8s%s, %ssmh %s%s\n",
             number,
             name,
             color ? COLOR_YELLOW : "",
             "Unsupported",
             color ? COLOR_RESET : "",
             color ? COLOR_YELLOW : "",
             driver_name,
             color ? COLOR_RESET : "");
   else
      printf("| %3u | %-51s | %s%8s%s | %ssmh %s%s |\n",
             number,
             name,
             color ? COLOR_YELLOW : "",
             "Unsupported",
             color ? COLOR_RESET : "",
             color ? COLOR_YELLOW : "",
             driver_name,
             color ? COLOR_RESET : "");
   return;
}

void
print_warning(bool csv, bool color, char* warning)
{
   if (csv)
      fprintf(stderr, " !!!, %s%-51s%s, !!!!!!!!, !!!!!!\n",
              COLOR_YELLOW, warning, COLOR_RESET);
   else
      fprintf(stderr, "| !!! | %s%-51s%s | !!!!!!!! | !!!!!! |\n",
              COLOR_YELLOW, warning, COLOR_RESET);
}

static void
setup_submit(void)
{
   if (submit_init)
      return;

   /* ensure fresh pool */
   next_cmdbuf_pool();
   cmdbuf_idx = 0;
   count = 0;
   cmdbuf = pools[cmdbuf_pool_idx].cmdbufs[cmdbuf_idx];
   /* fill every cmdbuf in pool with MAX_DRAWS */
   for (unsigned i = 0; i < MAX_CMDBUFS; i++) {
      begin_rp();
      VK(CmdDraw)(cmdbuf, 3, 1, 0, 0);
      end_cmdbuf();
      count = 0;
      /* ensure the pool doesn't roll over */
      if (i < MAX_CMDBUFS - 1)
         next_cmdbuf();
   }
   cmdbuf_idx = 0;
   cmdbuf = pools[cmdbuf_pool_idx].cmdbufs[cmdbuf_idx];
   submit_init = true;
}

static void
set_dyn_att_loadops(VkAttachmentLoadOp op)
{
   for (unsigned i = 0; i < dyn_render->colorAttachmentCount; i++) {
      VkRenderingAttachmentInfo *att = (VkRenderingAttachmentInfo*)&dyn_render->pColorAttachments[i];
      att->loadOp = op;
   }
}

static void
set_render_info(const struct perf_case *p, bool multirt)
{
   if (multirt) {
      fb = fb_multirt;
      dyn_render = &dyn_multirt;
      if (strstr(p->name, "dontcare")) {
         render_pass = render_pass_multirt_dontcare;
         set_dyn_att_loadops(VK_ATTACHMENT_LOAD_OP_DONT_CARE);
      } else {
         render_pass = render_pass_multirt_clear;
         set_dyn_att_loadops(VK_ATTACHMENT_LOAD_OP_CLEAR);
      }
   } else {
      fb = fb_basic;
      dyn_render = &dyn_basic;
      if (strstr(p->name, "dontcare")) {
         render_pass = render_pass_dontcare;
         set_dyn_att_loadops(VK_ATTACHMENT_LOAD_OP_DONT_CARE);
      } else {
         render_pass = render_pass_clear;
         set_dyn_att_loadops(VK_ATTACHMENT_LOAD_OP_CLEAR);
      }
   }
   if (strstr(p->name, "dynrender"))
      render_pass = VK_NULL_HANDLE;
}

static double
perf_run(unsigned case_idx, double base_rate, double duration)
{
   /* avoid clobbering in-use resources / clobbering perf */
   VkResult result = VK(QueueWaitIdle)(dev->queue);
   VK_CHECK("QueueWaitIdle", result);

   struct perf_case *p;
   cleanup_func = NULL;
   is_submit = false;
   is_zerovram = false;
   if (case_idx < ARRAY_SIZE(cases_draw)) {
      p = &cases_draw[case_idx];
   } else if (case_idx < ARRAY_SIZE(cases_draw) + ARRAY_SIZE(cases_submit)) {
      p = &cases_submit[case_idx - ARRAY_SIZE(cases_draw)];
      is_submit = true;
   } else if (case_idx < ARRAY_SIZE(cases_draw) + ARRAY_SIZE(cases_submit) + ARRAY_SIZE(cases_descriptor)) {
      p = &cases_descriptor[case_idx - ARRAY_SIZE(cases_draw) - ARRAY_SIZE(cases_submit)];
   } else {
      p = &cases_misc[case_idx - ARRAY_SIZE(cases_draw) - ARRAY_SIZE(cases_submit) - ARRAY_SIZE(cases_descriptor)];
   }
   bool was_descriptor_buffer = is_descriptor_buffer;
   unsigned name_len = strlen(p->name);
   is_descriptor_buffer = !strcmp(&p->name[name_len - 1 - 3], "_db") || strstr(p->name, "_db_");
   if (cmdbuf_active && (!is_submit || !submit_init || was_descriptor_buffer != is_descriptor_buffer)) {
      end_cmdbuf();
      next_cmdbuf();
   }
   bool unsupported = p->check_func && !p->check_func();
   is_dynamic = !!strstr(p->name, "dynamic");
   pipelines = p->pipelines;
   bool multirt = !!strstr(p->name, "multirt");
   if (is_descriptor_buffer && !unsupported) {
      if (strstr(p->name, "ubo"))
         descriptor_buffer = ubo_db_bda[0];
      else if (strstr(p->name, "ssbo"))
         descriptor_buffer = ssbo_db_bda[0];
      else if (strstr(p->name, "combined_sampler"))
         descriptor_buffer = sampler_db_bda[0];
      else if (strstr(p->name, "image"))
         descriptor_buffer = image_db_bda[0];
   }
   if (strstr(p->name, "zerovram")) {
      if (!fixed_iteration_count) {
         /* these tests can't naturally terminate themselves */
         print_warning(csv, color, "zerovram tests must be used with -fixed");
         unsupported = true;
      } else {
         is_zerovram = true;
         if (cmdbuf_idx)
            next_cmdbuf();
      }
   }
   set_render_info(p, multirt);
   if (is_submit)
      setup_submit();

   double rate = 0.0;

   if (!unsupported) {
      if (fixed_iteration_count == 0) {
         // duration mode
         rate = perf_measure_cpu_rate(p->func, duration);
      } else {
         // fixed iteration count mode
         rate = perf_measure_cpu_fixed(p->func, fixed_iteration_count);
      }
   }

   if (!multirt && strstr(p->name, "draw_multi"))
      rate *= ARRAY_SIZE(draws);
   double ratio = base_rate ? rate / base_rate : 1;

   const char *ratio_color = base_rate == 0 ? COLOR_RESET :
                                              ratio > 0.7 ? COLOR_GREEN :
                                              ratio > 0.4 ? COLOR_YELLOW : COLOR_RED;

   char space[50];
   memset(space, ' ', sizeof(space));
   space[sizeof(space) - strlen(p->name)] = 0;
   char buf[128];
   uint64_t r = is_submit || is_zerovram ? (uint64_t)rate : (uint64_t)(rate / 1000lu);
   snprintf(buf, sizeof(buf), "%"PRIu64, r);
   if (unsupported) {
      char name[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
      memcpy(name, dev->info.props.deviceName, sizeof(dev->info.props.deviceName));
      for (unsigned i = 0; i < sizeof(dev->info.props.deviceName); i++) {
         if ((name[i] >= 'A' && name[i] <= 'Z') ||
             (name[i] >= 'a' && name[i] <= 'z'))
            continue;
         name[i] = 0;
         break;
      }
      print_table_row_unsupported(csv, color, case_idx, p->name, name);
   } else {
      if (output_only)
         printf("%5"PRIu64"\n", r);
      else
         print_table_row(csv, color, case_idx, p->name, r, ratio_color, 100 * ratio);
   }
   return rate;
}

static void
init_cmdbufs(void)
{
   VkResult result;
   VkCommandPoolCreateInfo cpci = {0};
   cpci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
   cpci.queueFamilyIndex = dev->gfx_queue;
   VkCommandBufferAllocateInfo cbai = {0};
   cbai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
   cbai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
   cbai.commandBufferCount = MAX_CMDBUFS;
   VkFenceCreateInfo fci = {0};
   fci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

   for (unsigned i = 0; i < MAX_CMDBUF_POOLS; i++) {
      util_queue_fence_init(&pools[i].fence);
      result = VK(CreateCommandPool)(dev->dev, &cpci, NULL, &pools[i].cmdpool);
      VK_CHECK("CreateCommandPool", result);
      cbai.commandPool = pools[i].cmdpool;
      result = VK(AllocateCommandBuffers)(dev->dev, &cbai, pools[i].cmdbufs);
      VK_CHECK("AllocateCommandBuffers", result);
      result = VK(CreateFence)(dev->dev, &fci, NULL, &pools[i].f);
      VK_CHECK("CreateFence", result);
   }
}

static void
init_dyn_render(VkRenderingInfo *info)
{
   info->sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
   info->renderArea.extent.width = 100;
   info->renderArea.extent.height = 100;
   info->layerCount = 1;
}

static void
parse_args(int argc, const char **argv)
{
   bool next_arg_is_test_no = false;
   bool next_arg_is_start_no = false;
   bool next_arg_is_duration = false;
   bool next_arg_is_fixed = false;
   for (int i = 0; i < argc; i++) {
      if (next_arg_is_test_no || next_arg_is_start_no) {
         errno = 0;
         int val = strtol(argv[i], NULL, 10);
         if (errno || val < 0 || val >= TOTAL_CASES) {
            fprintf(stderr, "Invalid test number specified\n");
            abort();
         }
         if (next_arg_is_test_no) {
            test_no = val;
            if (test_no < ARRAY_SIZE(cases_draw))
               draw_only = true;
            else if (test_no < ARRAY_SIZE(cases_draw) + ARRAY_SIZE(cases_submit))
               submit_only = true;
            else
               descriptor_only = true;
         } else if (next_arg_is_start_no) {
            start_no = val;
         }
      } else if (next_arg_is_duration) {
         errno = 0;
         double val = strtod(argv[i], NULL);
         if (errno || val < 0) {
            fprintf(stderr, "Invalid duration specified\n");
            abort();
         }
         duration = val;
      } else if (next_arg_is_fixed) {
          fixed_iteration_count = 0;
          int val = strtol(argv[i], NULL, 10);
          if (errno || val < 0) {
             fprintf(stderr, "Invalid fixed number of iterations specified\n");
             abort();
          }
          fixed_iteration_count = val;
      }
      next_arg_is_test_no = false;
      next_arg_is_start_no = false;
      next_arg_is_duration = false;
      next_arg_is_fixed = false;
      if (argv[i][0] != '-')
         continue;
      const char *arg = &argv[i][1];
      if (!strcmp(arg, "csv"))
         csv = true;
      if (!strcmp(arg, "test"))
         next_arg_is_test_no = true;
      if (!strcmp(arg, "start"))
         next_arg_is_start_no = true;
      else if (!strcmp(arg, "duration"))
         next_arg_is_duration = true;
      else if (!strcmp(arg, "fixed"))
         next_arg_is_fixed = true;
      else if (!strcmp(arg, "nocolor"))
         color = false;
      else if (!strcmp(arg, "output-only"))
         output_only = true;
      else if (!strcmp(arg, "submit-only"))
         submit_only = true;
      else if (!strcmp(arg, "draw-only"))
         draw_only = true;
      else if (!strcmp(arg, "descriptor-only"))
         descriptor_only = true;
      else if (!strcmp(arg, "misc-only"))
         misc_only = true;
      else if (!strcmp(arg, "list")) {
         for (unsigned i = 0; i < ARRAY_SIZE(cases_draw); i++)
            printf(" %3u, %s\n", i, cases_draw[i].name);
         for (unsigned i = 0; i < ARRAY_SIZE(cases_submit); i++)
            printf(" %3u, %s\n", i + (unsigned)ARRAY_SIZE(cases_draw), cases_submit[i].name);
         for (unsigned i = 0; i < ARRAY_SIZE(cases_descriptor); i++)
            printf(" %3u, %s\n", i + (unsigned)(ARRAY_SIZE(cases_draw) + ARRAY_SIZE(cases_submit)), cases_descriptor[i].name);
         for (unsigned i = 0; i < ARRAY_SIZE(cases_misc); i++)
            printf(" %3u, %s\n", i + (unsigned)(ARRAY_SIZE(cases_draw) + ARRAY_SIZE(cases_submit) + ARRAY_SIZE(cases_descriptor)), cases_misc[i].name);
         exit(0);
      } else if (!strcmp(arg, "help") || !strcmp(arg, "h")) {
         fprintf(stderr, "vkoverhead [-list] [-test/start TESTNUM] [-duration SECONDS] [-nocolor] [-output-only] [-draw-only] [-submit-only] [-descriptor-only] [-misc-only] [-fixed ITERATIONS] [-csv]\n");
         exit(0);
      }
   }

   if (fixed_iteration_count != 0 && test_no == -1 && start_no == -1) {
       fprintf(stderr, "Fixed iteration count specified but test|start not specified\n");
       abort();
   }
}

static void
init_descriptor_state(VkDescriptorType descriptorType, unsigned descriptorCount,
                      VkPipelineLayout *layout, VkDescriptorUpdateTemplate *template,
                      VkPipelineLayout *push_layout, VkDescriptorUpdateTemplate *push_template,
                      VkDescriptorSet *sets, VkDescriptorSet *mutable_sets, unsigned set_count)
{
   VkDescriptorUpdateTemplateEntry template_entry = {0};
   VkMutableDescriptorTypeListEXT mutable = {0};
   VkDescriptorType types[] = {
      VK_DESCRIPTOR_TYPE_SAMPLER,
      VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
      VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
      VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
      VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
   };
   mutable.descriptorTypeCount = ARRAY_SIZE(types);
   mutable.pDescriptorTypes = types;
   VkMutableDescriptorTypeCreateInfoEXT minfo = {0};
   minfo.sType = VK_STRUCTURE_TYPE_MUTABLE_DESCRIPTOR_TYPE_CREATE_INFO_EXT;
   minfo.mutableDescriptorTypeListCount = 1;
   minfo.pMutableDescriptorTypeLists = &mutable;

   VkDescriptorUpdateTemplateCreateInfo tci = {0};
   tci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO;
   tci.descriptorUpdateEntryCount = 1;
   tci.templateType = VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_DESCRIPTOR_SET;
   tci.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
   tci.pDescriptorUpdateEntries = &template_entry;

   VkDescriptorSetLayoutBinding binding = {0};
   binding.descriptorType = descriptorType;
   binding.descriptorCount = descriptorCount;
   binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
   VkDescriptorSetLayout desc_layout = create_descriptor_layout(&binding, 1, false, false, set_count == 0, NULL);
   VkDescriptorPoolSize size = {0};
   size.type = descriptorType;
   size.descriptorCount = descriptorCount;
   for (unsigned i = 0; i < set_count; i++)
      sets[i] = create_descriptor_set(desc_layout, &size, false, NULL);
   if (dev->info.have_EXT_mutable_descriptor_type) {
      for (unsigned i = 0; i < set_count; i++) {
         VkDescriptorSetLayout mutable_desc_layout = create_descriptor_layout(&binding, 1, false, !i, false, &minfo);
         mutable_sets[i] = create_descriptor_set(mutable_desc_layout, &size, !i, &minfo);
      }
   }
   *layout = create_pipeline_layout(&desc_layout, 1);
   if (dev->info.have_KHR_descriptor_update_template && set_count) {
      tci.descriptorSetLayout = desc_layout;
      tci.pipelineLayout = *layout;
      template_entry.descriptorCount = descriptorCount;
      template_entry.descriptorType = descriptorType;
      switch (descriptorType) {
      case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
      case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
      case VK_DESCRIPTOR_TYPE_SAMPLER:
      case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
      case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
         template_entry.stride = sizeof(VkDescriptorImageInfo);
         break;
      case VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK:
      case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
      case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
         template_entry.stride = sizeof(VkDescriptorBufferInfo);
         break;
      case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
      case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
         template_entry.stride = sizeof(VkBufferView);
         break;
      default:
         unreachable("???");
      }
      VkResult result = VK(CreateDescriptorUpdateTemplate)(dev->dev, &tci, NULL, template);
      VK_CHECK("CreateDescriptorUpdateTemplate", result);

      if (dev->info.have_KHR_push_descriptor) {
         desc_layout = create_descriptor_layout(&binding, 1, true, false, false, NULL);
         *push_layout = create_pipeline_layout(&desc_layout, 1);
         tci.descriptorSetLayout = desc_layout;
         tci.pipelineLayout = *push_layout;
         tci.templateType = VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_PUSH_DESCRIPTORS_KHR;
         VkResult result = VK(CreateDescriptorUpdateTemplate)(dev->dev, &tci, NULL, push_template);
         VK_CHECK("CreateDescriptorUpdateTemplate", result);
      }
   }
}

int
main(int argc, char *argv[])
{
   parse_args(argc, (const char**)argv);
   util_cpu_detect();
   dev = vk_device_create();
   max_rts = dev->info.props.limits.maxColorAttachments == 8 ? 8 : 4;
   init_cmdbufs();
   render_pass_clear = create_renderpass(1, true);
   render_pass_dontcare = create_renderpass(1, false);
   render_pass_multirt_clear = create_renderpass(max_rts, true);
   render_pass_multirt_dontcare = create_renderpass(max_rts, false);

   VkSamplerCreateInfo sci = {0};
   sci.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
   VkResult result = VK(CreateSampler)(dev->dev, &sci, NULL, &sampler);
   VK_CHECK("CreateSampler", result);

   init_dyn_render(&dyn_basic);
   dyn_basic.colorAttachmentCount = 1;
   dyn_basic.pColorAttachments = dyn_att_basic;
   init_dyn_render(&dyn_multirt);
   dyn_multirt.colorAttachmentCount = max_rts;
   dyn_multirt.pColorAttachments = dyn_att_multirt;

   unsigned max_images = MIN2(dev->info.props.limits.maxPerStageDescriptorStorageImages, MAX_IMAGES);
   init_descriptor_state(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, &layout_basic, &template_basic, &layout_basic_push, &template_basic_push, desc_set_basic, desc_set_basic_mutable, ARRAY_SIZE(desc_set_basic));
   init_descriptor_state(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_UBOS, &layout_ubo, &template_ubo, &layout_ubo_push, &template_ubo_push, desc_set_ubo, desc_set_ubo_mutable, ARRAY_SIZE(desc_set_ubo));
   #define INIT_DESCRIPTOR(TYPE, SUFFIX, LIMIT) \
   init_descriptor_state(TYPE, 1, &layout_##SUFFIX, &template_##SUFFIX, &layout_##SUFFIX##_push, &template_##SUFFIX##_push, desc_set_##SUFFIX, desc_set_##SUFFIX##_mutable, ARRAY_SIZE(desc_set_##SUFFIX)); \
   init_descriptor_state(TYPE, LIMIT, &layout_##SUFFIX##_many, &template_##SUFFIX##_many, &layout_##SUFFIX##_many_push, &template_##SUFFIX##_many_push, desc_set_##SUFFIX##_many, desc_set_##SUFFIX##_many_mutable, ARRAY_SIZE(desc_set_##SUFFIX##_many));

   INIT_DESCRIPTOR(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, ssbo, MAX_SSBOS);
   INIT_DESCRIPTOR(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, combined_sampler, MAX_SAMPLERS);
   INIT_DESCRIPTOR(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, sampled_image, MAX_SAMPLERS);
   INIT_DESCRIPTOR(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, image, max_images);
   INIT_DESCRIPTOR(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, tbo, MAX_SAMPLERS);
   INIT_DESCRIPTOR(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, ibo, max_images);

   create_basic_pipelines(render_pass_clear, layout_basic, pipelines_basic);
   if (check_dynamic_rendering())
      create_basic_pipelines(VK_NULL_HANDLE, layout_basic, pipelines_dyn);
   create_vattrib_pipelines(render_pass_clear, layout_basic, pipelines_vattrib);
   if (check_dynamic_vertex_input())
      pipeline_vattrib_dynamic = create_vattrib_pipeline_dynamic(render_pass_clear, layout_basic);
   pipeline_multirt = create_multirt_pipeline(render_pass_multirt_clear, layout_basic);
   if (check_dynamic_rendering())
      pipeline_multrt_dyn = create_multirt_pipeline(VK_NULL_HANDLE, layout_basic);
   pipeline_ubo = create_ubo_pipeline(render_pass_clear, layout_ubo, 0);
   pipeline_ssbo = create_ssbo_pipeline(render_pass_clear, layout_ssbo);
   pipeline_ssbo_many = create_ssbo_many_pipeline(render_pass_clear, layout_ssbo_many, 0);
   pipeline_combined_sampler = create_sampler_pipeline(render_pass_clear, layout_combined_sampler);
   pipeline_combined_sampler_many = create_sampler_many_pipeline(render_pass_clear, layout_combined_sampler_many, 0);
   pipeline_image = create_image_pipeline(render_pass_clear, layout_image);
   pipeline_image_many = create_image_many_pipeline(render_pass_clear, layout_image_many, 0);
   pipeline_tbo = create_tbo_pipeline(render_pass_clear, layout_tbo);
   pipeline_tbo_many = create_tbo_many_pipeline(render_pass_clear, layout_tbo_many);
   pipeline_ibo = create_ibo_pipeline(render_pass_clear, layout_ibo);
   pipeline_ibo_many = create_ibo_many_pipeline(render_pass_clear, layout_ibo_many);
   if (check_graphics_pipeline_library()) {
      pipeline_gpl_basic = create_gpl_basic_pipeline(render_pass_clear, layout_basic);
      VkGraphicsPipelineCreateInfo pci = {0};
      VkPipeline libraries[2];
      libraries[0] = pipeline_gpl_basic;
      VkPipelineLibraryCreateInfoKHR libstate = {
         VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR,
         NULL,
         ARRAY_SIZE(libraries),
         libraries
      };
      pci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      pci.pNext = &libstate;
      pci.layout = layout_basic;
      _mesa_hash_table_init(&gpl_pipeline_table, NULL, gpl_hash_vi, gpl_equals_vi);
      VkPipelineVertexInputStateCreateInfo vertex_input_state = {0};
      vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
      vertex_input_state.vertexBindingDescriptionCount = 16;
      vertex_input_state.vertexAttributeDescriptionCount = 16;
      for (unsigned i = 0; i < ARRAY_SIZE(pipeline_gpl_vert); i++) {
         vertex_input_state.pVertexBindingDescriptions = gpl_vbinding[i];
         vertex_input_state.pVertexAttributeDescriptions = gpl_vattr[i];
         pipeline_gpl_vert[i] = create_gpl_vert_pipeline(render_pass_clear, layout_basic, &vertex_input_state);
         libraries[1] = pipeline_gpl_vert[i];
         result = VK(CreateGraphicsPipelines)(dev->dev, VK_NULL_HANDLE, 1, &pci, NULL, &pipeline_gpl_vert_final[i]);
         VK_CHECK("CreateGraphicsPipelines", result);
         {
            VkPipelineVertexInputStateCreateInfo *key = mem_dup(&vertex_input_state, sizeof(VkPipelineVertexInputStateCreateInfo));
            key->pVertexBindingDescriptions = mem_dup(vertex_input_state.pVertexBindingDescriptions, vertex_input_state.vertexBindingDescriptionCount * sizeof(VkVertexInputBindingDescription));
            key->pVertexAttributeDescriptions = mem_dup(vertex_input_state.pVertexAttributeDescriptions, vertex_input_state.vertexAttributeDescriptionCount * sizeof(VkVertexInputAttributeDescription));
            _mesa_hash_table_insert(&gpl_pipeline_table, key, pipeline_gpl_vert_final[i]);
         }
      }
      if (check_dota2()) {
         depthonly_layout = depthonly_init(depthonly_pipelines);
         slow_layout = slow_init(slow_pipelines);
      }
   }

   cmdbuf = pools[0].cmdbufs[0];
   pipelines = pipelines_basic;

   util_queue_init(&queue, "reset", 8, 1, UTIL_QUEUE_INIT_RESIZE_IF_FULL, NULL);

   for (unsigned i = 0; i < ARRAY_SIZE(index_bo); i++)
      index_bo[i] = create_index_buffer();
   fb_basic = create_framebuffer(1, render_pass_clear);
   for (unsigned i = 0; i < ARRAY_SIZE(vbo); i++)
      vbo[i] = create_vertex_buffer();
   for (unsigned i = 0; i < ARRAY_SIZE(ubo); i++)
      ubo[i] = create_uniform_buffer();
   if (!submit_only) {
      fb_multirt = create_framebuffer(8, render_pass_multirt_clear);
      for (unsigned i = 0; i < ARRAY_SIZE(ssbo); i++)
         ssbo[i] = create_storage_buffer();
      for (unsigned i = 0; i < ARRAY_SIZE(tbo); i++)
         tbo[i] = create_texel_buffer();
      for (unsigned i = 0; i < ARRAY_SIZE(ibo); i++)
         ibo[i] = create_image_buffer();
   }

   for (unsigned i = 0; i < ARRAY_SIZE(draws); i++) {
      draws[i].firstVertex = 0;
      draws[i].vertexCount = 3;
   }

   for (unsigned i = 0; i < ARRAY_SIZE(draws_indexed); i++) {
      draws_indexed[i].firstIndex = 0;
      draws_indexed[i].indexCount = 3;
      draws_indexed[i].vertexOffset = 0;
   }

   begin_cmdbuf();
   setup();
   end_cmdbuf();

   if (check_zerovram()) {
      VkDeviceSize avail_vram = 0;
      VkPhysicalDeviceMemoryProperties2 mem = {0};
      mem.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;

      VkPhysicalDeviceMemoryBudgetPropertiesEXT budget = {0};
      budget.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;
      mem.pNext = &budget;
      VK(GetPhysicalDeviceMemoryProperties2)(dev->pdev, &mem);

      for (unsigned i = 0; i < mem.memoryProperties.memoryHeapCount; i++) {
         if (mem.memoryProperties.memoryHeaps[i].flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
            /* VRAM */
            vram_size += mem.memoryProperties.memoryHeaps[i].size / 1024 / 1024;
            avail_vram += (mem.memoryProperties.memoryHeaps[i].size - budget.heapUsage[i]) / 1024;
         }
      }
      /* be conservative: require only half of total vram */
      if (avail_vram < vram_size / 2) {
         print_warning(csv, color, "Disabling zerovam tests: half of total vram required");
         dev->info.have_EXT_memory_budget = false;
      }
   }

   if (check_descriptor_buffer()) {
      /* descriptor buffer offset binding tests don't actually use the descriptor buffers
       * if descriptor buffers will ever be accessed by the GPU, this block should also use vkGetDescriptorEXT
       * to populate the buffers
       */
      for (unsigned i = 0; i < 2; i++) {
         ubo_db[i] = create_descriptor_buffer();
         ubo_db_bda[i] = get_bda(ubo_db[i]);
         ssbo_db[i] = create_descriptor_buffer();
         ssbo_db_bda[i] = get_bda(ssbo_db[i]);
         sampler_db[i] = create_descriptor_buffer();
         sampler_db_bda[i] = get_bda(sampler_db[i]);
         image_db[i] = create_descriptor_buffer();
         image_db_bda[i] = get_bda(image_db[i]);
      }
      init_descriptor_state(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_UBOS, &layout_ubo_db, NULL, NULL, NULL, NULL, NULL, 0);
      init_descriptor_state(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, MAX_SSBOS, &layout_ssbo_many_db, NULL, NULL, NULL, NULL, NULL, 0);
      init_descriptor_state(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_SAMPLERS, &layout_combined_sampler_many_db, NULL, NULL, NULL, NULL, NULL, 0);
      init_descriptor_state(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, max_images, &layout_image_many_db, NULL, NULL, NULL, NULL, NULL, 0);
      pipeline_ubo_db = create_ubo_pipeline(render_pass_clear, layout_ubo_db, VK_PIPELINE_CREATE_DESCRIPTOR_BUFFER_BIT_EXT);
      pipeline_ssbo_many_db = create_ssbo_many_pipeline(render_pass_clear, layout_ssbo_many_db, VK_PIPELINE_CREATE_DESCRIPTOR_BUFFER_BIT_EXT);
      pipeline_combined_sampler_many_db = create_sampler_many_pipeline(render_pass_clear, layout_combined_sampler_many_db, VK_PIPELINE_CREATE_DESCRIPTOR_BUFFER_BIT_EXT);
      pipeline_image_many_db = create_image_many_pipeline(render_pass_clear, layout_image_many_db, VK_PIPELINE_CREATE_DESCRIPTOR_BUFFER_BIT_EXT);
   }

   /* ensure that setup completes before proceeding */
   {
      VkSubmitInfo s = {0};
      s.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      s.commandBufferCount = 1;
      s.pCommandBuffers = &cmdbuf;
      VkResult result = VK(QueueSubmit)(dev->queue, 1, &s, VK_NULL_HANDLE);
      VK_CHECK("QueueSubmit", result);
      result = VK(QueueWaitIdle)(dev->queue);
      VK_CHECK("QueueWaitIdle", result);
      result = VK(ResetCommandPool)(dev->dev, pools[0].cmdpool, 0);
      VK_CHECK("ResetCommandPool", result);
   }

   if (!output_only) {
      printf("\n| %-*s | %-*s | %-*s |\n",
             MAX2((int)strlen(dev->info.props.deviceName)       , 11), "Device Name",
             MAX2((int)strlen(dev->info.driver_props.driverName), 11), "Driver Name",
             MAX2((int)strlen(dev->info.driver_props.driverInfo), 11), "Driver Info");
      printf("| ");
      for (int i=0; i<MAX2((int)strlen(dev->info.props.deviceName)       , 11); i++)
        printf("-");
      printf(" | ");
      for (int i=0; i<MAX2((int)strlen(dev->info.driver_props.driverName), 11); i++)
        printf("-");
      printf(" | ");
      for (int i=0; i<MAX2((int)strlen(dev->info.driver_props.driverInfo), 11); i++)
        printf("-");
      printf(" |\n");
      printf("| %-*s | %-*s | %-*s |\n\n",
             MAX2((int)strlen(dev->info.props.deviceName)       , 11),
             dev->info.props.deviceName,
             MAX2((int)strlen(dev->info.driver_props.driverName), 11),
             dev->info.driver_props.driverName,
             MAX2((int)strlen(dev->info.driver_props.driverInfo), 11),
             dev->info.driver_props.driverInfo );
   }
   if (!submit_only && !descriptor_only && !misc_only && !output_only && start_no < (int)ARRAY_SIZE(cases_draw)) {
      print_table_header(csv, "#", "Draw Tests", "1000op/s", "% relative to 'draw'");
   }
   double base_rate = 0;
   if (test_no > -1) {
      if (!output_only) {
         if (!draw_only && !descriptor_only && !misc_only)
            print_table_header(csv, "#", "Submit Tests", "op/s", "% relative to 'submit_noop");
         if (!draw_only && !submit_only && !descriptor_only)
            print_table_header(csv, "#", "Descriptor Tests", "1000op/s", "% relative to 'descriptor_noop'");
         if (!draw_only && !submit_only && !misc_only)
            print_table_header(csv, "#", "Misc Tests", "1000op/s (besides zerovram)", "% (ignore)");
      }
      perf_run(test_no, base_rate, duration);
   } else {
      if (!submit_only && !descriptor_only && !misc_only && start_no < (int)ARRAY_SIZE(cases_draw)) {
         base_rate = perf_run(0, 0, duration);
         unsigned start = start_no == -1 ? 1 : start_no;
         for (unsigned i = start; i < ARRAY_SIZE(cases_draw); i++)
            perf_run(i, base_rate, duration);
         if (start != 1)
            start_no = -1;
      }
      if (!draw_only && !descriptor_only && !misc_only && start_no < (int)(ARRAY_SIZE(cases_draw) + ARRAY_SIZE(cases_submit))) {
         if (!output_only)
            print_table_header(csv, "#", "Submit Tests", "op/s", "% relative to 'submit_noop");
         base_rate = perf_run(ARRAY_SIZE(cases_draw), 0, duration);
         unsigned start = start_no == -1 ? 1 : (start_no - ARRAY_SIZE(cases_draw));
         for (unsigned i = start; i < ARRAY_SIZE(cases_submit); i++)
            perf_run(ARRAY_SIZE(cases_draw) + i, base_rate, duration);
         if (!submit_only) {
            next_cmdbuf();
            while (cmdbuf_idx || cmdbuf_pool_idx)
               next_cmdbuf();
         }
         if (start != 1)
            start_no = -1;
      }
      if (!draw_only && !submit_only && !misc_only && start_no < (int)(ARRAY_SIZE(cases_draw) + ARRAY_SIZE(cases_submit) + ARRAY_SIZE(cases_descriptor))) {
         if (!output_only)
            print_table_header(csv, "#", "Descriptor Tests", "1000op/s", "% relative to 'descriptor_noop'");
         base_rate = perf_run(ARRAY_SIZE(cases_draw) + ARRAY_SIZE(cases_submit), 0, duration);
         unsigned start = start_no == -1 ? 1 : (start_no - (ARRAY_SIZE(cases_draw) + ARRAY_SIZE(cases_submit)));
         for (unsigned i = start; i < ARRAY_SIZE(cases_descriptor); i++)
            perf_run(ARRAY_SIZE(cases_draw) + ARRAY_SIZE(cases_submit) + i, base_rate, duration);
         if (start != 1)
            start_no = -1;
      }
      if (!draw_only && !submit_only && !descriptor_only) {
         if (!output_only)
            print_table_header(csv, "#", "Misc Tests", "1000op/s (besides zerovram)", "% (ignore)");
         base_rate = 0;
         unsigned start = start_no == -1 ? 0 : (start_no - (ARRAY_SIZE(cases_draw) + ARRAY_SIZE(cases_submit) + ARRAY_SIZE(cases_descriptor)));
         for (unsigned i = start; i < ARRAY_SIZE(cases_misc); i++)
            perf_run(ARRAY_SIZE(cases_draw) + ARRAY_SIZE(cases_submit) + ARRAY_SIZE(cases_descriptor) + i, base_rate, duration);
         if (start != 0)
            start_no = -1;
      }
   }

   printf("\n");
   return 0;
}
