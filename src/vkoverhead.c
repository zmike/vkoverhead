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
#include "common.h"
#include "u_queue.h"
#include "u_cpu_detect.h"
#include <inttypes.h>

struct vk_device *dev;

#define MAX_CMDBUFS 50
#define MAX_CMDBUF_POOLS 3
#define MAX_DRAWS 10000
#define NUM_PIPELINE_VARIANTS 4

struct util_queue queue;

struct util_queue_fence fence[MAX_CMDBUF_POOLS];
static VkCommandPool cmdpool[MAX_CMDBUF_POOLS];
static VkCommandBuffer cmdbufs[MAX_CMDBUF_POOLS][MAX_CMDBUFS];
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
static VkPipelineLayout layout_sampler;
static VkPipelineLayout layout_sampler_many;
static VkPipelineLayout layout_tbo;
static VkPipelineLayout layout_tbo_many;
static VkPipelineLayout layout_ibo;
static VkPipelineLayout layout_ibo_many;
static VkPipelineLayout layout_image;
static VkPipelineLayout layout_image_many;
static VkDescriptorUpdateTemplateKHR template_basic;
static VkDescriptorUpdateTemplateKHR template_ubo;
static VkDescriptorUpdateTemplateKHR template_ssbo;
static VkDescriptorUpdateTemplateKHR template_ssbo_many;
static VkDescriptorUpdateTemplateKHR template_sampler;
static VkDescriptorUpdateTemplateKHR template_sampler_many;
static VkDescriptorUpdateTemplateKHR template_tbo;
static VkDescriptorUpdateTemplateKHR template_tbo_many;
static VkDescriptorUpdateTemplateKHR template_ibo;
static VkDescriptorUpdateTemplateKHR template_ibo_many;
static VkDescriptorUpdateTemplateKHR template_image;
static VkDescriptorUpdateTemplateKHR template_image_many;
static VkPipeline pipelines_basic[NUM_PIPELINE_VARIANTS];
static VkPipeline pipelines_vattrib[NUM_PIPELINE_VARIANTS];
static VkPipeline pipelines_dyn[NUM_PIPELINE_VARIANTS];
static VkPipeline pipeline_vattrib_dynamic;
static VkPipeline pipeline_multirt;
static VkPipeline pipeline_multrt_dyn;
static VkPipeline pipeline_ubo;
static VkPipeline pipeline_ssbo;
static VkPipeline pipeline_ssbo_many;
static VkPipeline pipeline_sampler;
static VkPipeline pipeline_sampler_many;
static VkPipeline pipeline_tbo;
static VkPipeline pipeline_tbo_many;
static VkPipeline pipeline_ibo;
static VkPipeline pipeline_ibo_many;
static VkPipeline pipeline_image;
static VkPipeline pipeline_image_many;
static VkPipeline *pipelines; //this one gets used
static VkDescriptorSet desc_set_basic[2];
static VkDescriptorSet desc_set_ubo[2];
static VkDescriptorSet desc_set_ssbo[2];
static VkDescriptorSet desc_set_ssbo_many[2];
static VkDescriptorSet desc_set_sampler[2];
static VkDescriptorSet desc_set_sampler_many[2];
static VkDescriptorSet desc_set_image[2];
static VkDescriptorSet desc_set_image_many[2];
static VkDescriptorSet desc_set_tbo[2];
static VkDescriptorSet desc_set_tbo_many[2];
static VkDescriptorSet desc_set_ibo[2];
static VkDescriptorSet desc_set_ibo_many[2];
static VkBuffer vbo[16];
static VkBuffer ubo[MAX_UBOS];
static VkBuffer ssbo[MAX_SSBOS];
static VkBuffer tbo[MAX_SAMPLERS];
static VkBuffer ibo[MAX_IMAGES];
static VkBuffer index_bo[2];
static VkImageView tex[MAX_SAMPLERS];
static VkImageView img[MAX_IMAGES];
static VkSampler sampler;
static VkBufferView tbo_views[2][MAX_SAMPLERS];
static VkBufferView ibo_views[2][MAX_SAMPLERS];
static VkDescriptorBufferInfo dbi[2][MAX_UBOS] = {0};
static VkDescriptorBufferInfo dbi_storage[2][MAX_SSBOS] = {0};
static VkDescriptorImageInfo dii[2][MAX_SAMPLERS] = {0};
static VkDescriptorImageInfo dii_storage[2][MAX_IMAGES] = {0};

static VkMultiDrawInfoEXT draws[500];
static VkMultiDrawIndexedInfoEXT draws_indexed[500];
static uint64_t count = 0;

static bool is_submit = false;
static bool is_dynamic = false;
static bool submit_init = false;

/* cmdline options */
static double duration = 1.0;
static int test_no = -1;
static bool color = true;
static bool submit_only = false;
static bool draw_only = false;
static bool descriptor_only = false;

static void
reset_cmdbuf(void *data, void *gdata, int thread_idx)
{
   VkResult result = VK(ResetCommandPool)(dev->dev, data, 0);
   VK_CHECK("ResetCommandPool", result);
}

static void
next_cmdbuf_pool(void)
{
   util_queue_add_job(&queue, cmdpool[cmdbuf_pool_idx], &fence[cmdbuf_pool_idx], reset_cmdbuf, NULL, 0);
   cmdbuf_pool_idx++;
   cmdbuf_pool_idx %= MAX_CMDBUF_POOLS;
   util_queue_fence_wait(&fence[cmdbuf_pool_idx]);
}

static void
next_cmdbuf(void)
{
   cmdbuf_idx++;
   cmdbuf_idx %= MAX_CMDBUFS;
   if (cmdbuf_idx == 0)
      next_cmdbuf_pool();
   cmdbuf = cmdbufs[cmdbuf_pool_idx][cmdbuf_idx];
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
   VK(CmdBindPipeline)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[0]);
   if (!is_dynamic)
      VK(CmdBindVertexBuffers)(cmdbuf, 0, ARRAY_SIZE(vbo), vbo, offsets);
   VK(CmdBindIndexBuffer)(cmdbuf, index_bo[0], 0, VK_INDEX_TYPE_UINT32);
   VK(CmdBindDescriptorSets)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_basic, 0, 1, desc_set_basic, 0, NULL);

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
      rpbi.clearValueCount = fb == fb_basic ? 1 : MAX_RTS;
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
   while (count + iterations * divisor > MAX_DRAWS) {
      unsigned remain = (MAX_DRAWS - count) / divisor;
      func(remain);
      end_cmdbuf();
      next_cmdbuf();
      count = 0;
      iterations -= remain;
   }
   return iterations;
}

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
check_dynamic_rendering(void)
{
   return dev->info.have_KHR_dynamic_rendering;
}

static bool
check_descriptor_template(void)
{
   return dev->info.have_KHR_descriptor_update_template;
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
draw_1sampler_change(unsigned iterations)
{
   iterations = filter_overflow(draw_1sampler_change, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdBindDescriptorSets)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_sampler, 0, 1, &desc_set_sampler[i & 1], 0, NULL);
      VK(CmdDrawIndexed)(cmdbuf, 3, 1, 0, 0, 0);
   }
}

static void
draw_16sampler_change(unsigned iterations)
{
   iterations = filter_overflow(draw_16sampler_change, iterations, 1);
   begin_rp();
   for (unsigned i = 0; i < iterations; i++, count++) {
      VK(CmdBindDescriptorSets)(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_sampler_many, 0, 1, &desc_set_sampler_many[i & 1], 0, NULL);
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
init_submit(unsigned cmdbuf_count, unsigned submit_count, void *si, VkCommandBufferSubmitInfo *csi)
{
   VkCommandBuffer *c = cmdbufs[cmdbuf_pool_idx];
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

      VkResult result = VK(QueueSubmit2)(dev->queue, submit_count, s, VK_NULL_HANDLE);
      VK_CHECK("QueueSubmit2", result);
   } else {
      VkSubmitInfo *s = si;
      for (unsigned i = 0; i < submit_count; i++) {
         s[i].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
         s[i].commandBufferCount = cmdbuf_per_submit;
         s[i].pCommandBuffers = &c[i * cmdbuf_per_submit];
      }

      VkResult result = VK(QueueSubmit)(dev->queue, submit_count, s, VK_NULL_HANDLE);
      VK_CHECK("QueueSubmit", result);
   }
}

static void
submit_cmdbufs(unsigned submit_count, void *si)
{
   if (dev->info.have_KHR_synchronization2) {
      VkResult result = VK(QueueSubmit2)(dev->queue, submit_count, si, VK_NULL_HANDLE);
      VK_CHECK("QueueSubmit2", result);
   } else {
      VkResult result = VK(QueueSubmit)(dev->queue, submit_count, si, VK_NULL_HANDLE);
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
descriptor_1sampler(unsigned iterations)
{
   VkWriteDescriptorSet wds = {0};
   wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   wds.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
   wds.descriptorCount = 1;
   wds.dstSet = desc_set_sampler[0];
   for (unsigned i = 0; i < iterations; i++) {
      wds.pImageInfo = dii[i & 1];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }
}

static void
descriptor_16sampler(unsigned iterations)
{
   VkWriteDescriptorSet wds = {0};
   wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   wds.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
   wds.descriptorCount = MAX_SAMPLERS;
   wds.dstSet = desc_set_sampler_many[0];
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

static void
descriptor_template_1ubo(unsigned iterations)
{
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSetWithTemplate)(dev->dev, desc_set_basic[0], template_basic, dbi[i & 1]);
   }
}

static void
descriptor_template_12ubo(unsigned iterations)
{
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSetWithTemplate)(dev->dev, desc_set_ubo[0], template_ubo, dbi[i & 1]);
   }
}

static void
descriptor_template_1ssbo(unsigned iterations)
{
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSetWithTemplate)(dev->dev, desc_set_ssbo[0], template_ssbo, dbi_storage[i & 1]);
   }
}

static void
descriptor_template_8ssbo(unsigned iterations)
{
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSetWithTemplate)(dev->dev, desc_set_ssbo_many[0], template_ssbo_many, dbi_storage[i & 1]);
   }
}

static void
descriptor_template_1sampler(unsigned iterations)
{
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSetWithTemplate)(dev->dev, desc_set_sampler[0], template_sampler, dii[i & 1]);
   }
}

static void
descriptor_template_16sampler(unsigned iterations)
{
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSetWithTemplate)(dev->dev, desc_set_sampler_many[0], template_sampler_many, dii[i & 1]);
   }
}

static void
descriptor_template_1image(unsigned iterations)
{
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSetWithTemplate)(dev->dev, desc_set_image[0], template_image, dii_storage[i & 1]);
   }
}

static void
descriptor_template_16image(unsigned iterations)
{
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSetWithTemplate)(dev->dev, desc_set_image_many[0], template_image_many, dii_storage[i & 1]);
   }
}

static void
descriptor_template_1texelbuffer(unsigned iterations)
{
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSetWithTemplate)(dev->dev, desc_set_tbo[0], template_tbo, tbo_views[i & 1]);
   }
}

static void
descriptor_template_16texelbuffer(unsigned iterations)
{
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSetWithTemplate)(dev->dev, desc_set_tbo_many[0], template_tbo_many, tbo_views[i & 1]);
   }
}

static void
descriptor_template_1imagebuffer(unsigned iterations)
{
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSetWithTemplate)(dev->dev, desc_set_ibo[0], template_ibo, ibo_views[i & 1]);
   }
}

static void
descriptor_template_16imagebuffer(unsigned iterations)
{
   for (unsigned i = 0; i < iterations; i++) {
      VK(UpdateDescriptorSetWithTemplate)(dev->dev, desc_set_ibo_many[0], template_ibo_many, ibo_views[i & 1]);
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
#define CASE_UBO(name, ...) {#name, name, &pipeline_ubo, __VA_ARGS__}
#define CASE_SSBO(name, ...) {#name, name, &pipeline_ssbo, __VA_ARGS__}
#define CASE_SSBO_MANY(name, ...) {#name, name, &pipeline_ssbo_many, __VA_ARGS__}
#define CASE_SAMPLER(name, ...) {#name, name, &pipeline_sampler, __VA_ARGS__}
#define CASE_SAMPLER_MANY(name, ...) {#name, name, &pipeline_sampler_many, __VA_ARGS__}
#define CASE_IMAGE(name, ...) {#name, name, &pipeline_image, __VA_ARGS__}
#define CASE_IMAGE_MANY(name, ...) {#name, name, &pipeline_image_many, __VA_ARGS__}
#define CASE_TBO(name, ...) {#name, name, &pipeline_tbo, __VA_ARGS__}
#define CASE_TBO_MANY(name, ...) {#name, name, &pipeline_tbo_many, __VA_ARGS__}
#define CASE_IBO(name, ...) {#name, name, &pipeline_ibo, __VA_ARGS__}
#define CASE_IBO_MANY(name, ...) {#name, name, &pipeline_ibo_many, __VA_ARGS__}

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
   CASE_BASIC(draw_1ubo_change),
   CASE_UBO(draw_12ubo_change),
   CASE_SAMPLER(draw_1sampler_change),
   CASE_SAMPLER_MANY(draw_16sampler_change),
   CASE_TBO(draw_1texelbuffer_change),
   CASE_TBO_MANY(draw_16texelbuffer_change),
   CASE_SSBO(draw_1ssbo_change),
   CASE_SSBO_MANY(draw_8ssbo_change),
   CASE_IMAGE(draw_1image_change),
   CASE_IMAGE_MANY(draw_16image_change),
   CASE_IBO(draw_1imagebuffer_change),
   CASE_IBO_MANY(draw_16imagebuffer_change),
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
#define CASE_DESCRIPTOR_TEMPLATE(name, ...) {#name, name, pipelines_basic, check_descriptor_template}

static struct perf_case cases_descriptor[] = {
   CASE_DESCRIPTOR(descriptor_noop),
   CASE_DESCRIPTOR(descriptor_1ubo),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_1ubo),
   CASE_DESCRIPTOR(descriptor_12ubo),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_12ubo),
   CASE_DESCRIPTOR(descriptor_1sampler),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_1sampler),
   CASE_DESCRIPTOR(descriptor_16sampler),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_16sampler),
   CASE_DESCRIPTOR(descriptor_1texelbuffer),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_1texelbuffer),
   CASE_DESCRIPTOR(descriptor_16texelbuffer),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_16texelbuffer),
   CASE_DESCRIPTOR(descriptor_1ssbo),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_1ssbo),
   CASE_DESCRIPTOR(descriptor_8ssbo),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_8ssbo),
   CASE_DESCRIPTOR(descriptor_1image),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_1image),
   CASE_DESCRIPTOR(descriptor_16image),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_16image),
   CASE_DESCRIPTOR(descriptor_1imagebuffer),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_1imagebuffer),
   CASE_DESCRIPTOR(descriptor_16imagebuffer),
   CASE_DESCRIPTOR_TEMPLATE(descriptor_template_16imagebuffer),
};

#define TOTAL_CASES (ARRAY_SIZE(cases_draw) + ARRAY_SIZE(cases_submit) + ARRAY_SIZE(cases_descriptor))

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
   att->imageView = create_rt(&image);
   att->imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
   att->storeOp = VK_ATTACHMENT_STORE_OP_STORE;

   set_image_layout(image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

static void
setup(void)
{
   if (submit_only)
      goto only_submit;
   /* initialize textures */
   VkImage tex_image[MAX_SAMPLERS];
   VkClearColorValue clear = {0};
   VkImageSubresourceRange range = default_subresourcerange();
   for (unsigned i = 0; i < ARRAY_SIZE(tex); i++) {
      tex[i] = create_tex(&tex_image[i]);
      set_image_layout(tex_image[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
      VK(CmdClearColorImage)(cmdbuf, tex_image[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear, 1, &range);
      set_image_layout(tex_image[i], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
   }
   /* initialize storage images */
   VkImage storage_image[MAX_IMAGES];
   for (unsigned i = 0; i < ARRAY_SIZE(storage_image); i++) {
      img[i] = create_storage_image(&storage_image[i]);
      set_image_layout(storage_image[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
      VK(CmdClearColorImage)(cmdbuf, storage_image[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear, 1, &range);
      set_image_layout(storage_image[i], VK_IMAGE_LAYOUT_GENERAL);
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
      dbi[1][MAX_UBOS - i].buffer = ubo[i];
   }

   VkWriteDescriptorSet wds = {0};
   wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

   wds.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
   wds.descriptorCount = 1;
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_basic); i++) {
      wds.dstSet = desc_set_basic[i];
      wds.pBufferInfo = &dbi[0][i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }

   if (submit_only)
      return;

   for (unsigned i = 0; i < MAX_SSBOS; i++) {
      dbi_storage[0][i].range = VK_WHOLE_SIZE;
      dbi_storage[1][i].range = VK_WHOLE_SIZE;
      dbi_storage[0][i].buffer = ssbo[i];
      dbi_storage[1][MAX_SSBOS - i].buffer = ssbo[i];
   }
   for (unsigned i = 0; i < MAX_SAMPLERS; i++) {
      dii[1][i].sampler = dii[0][i].sampler = sampler;
      dii[1][MAX_SAMPLERS - i].imageView = dii[0][i].imageView = tex[i];
      dii[1][i].imageLayout = dii[0][i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      tbo_views[1][MAX_SAMPLERS - i] = tbo_views[0][i] = create_bufferview(tbo[i]);
   }
   for (unsigned i = 0; i < MAX_IMAGES; i++) {
      dii_storage[1][MAX_IMAGES - i].imageView = dii_storage[0][i].imageView = img[i];
      dii_storage[1][i].imageLayout = dii_storage[0][i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
      ibo_views[1][MAX_IMAGES - i] = ibo_views[0][i] = create_bufferview(ibo[i]);
   }

   wds.descriptorCount = MAX_UBOS;
   wds.pBufferInfo = dbi[0];
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_ubo); i++) {
      wds.dstSet = desc_set_ubo[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }

   wds.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
   wds.descriptorCount = 1;
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_ssbo); i++) {
      wds.dstSet = desc_set_ssbo[i];
      wds.pBufferInfo = dbi_storage[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }

   wds.descriptorCount = MAX_SSBOS;
   wds.pBufferInfo = dbi_storage[0];
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_ssbo_many); i++) {
      wds.dstSet = desc_set_ssbo_many[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }

   wds.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
   wds.descriptorCount = 1;
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_sampler); i++) {
      wds.dstSet = desc_set_sampler[i];
      wds.pImageInfo = dii[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }

   wds.descriptorCount = MAX_SAMPLERS;
   wds.pImageInfo = dii[0];
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_sampler_many); i++) {
      wds.dstSet = desc_set_sampler_many[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }

   wds.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
   wds.descriptorCount = 1;
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_tbo); i++) {
      wds.dstSet = desc_set_tbo[i];
      wds.pTexelBufferView = tbo_views[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }

   wds.descriptorCount = MAX_SAMPLERS;
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_tbo_many); i++) {
      wds.dstSet = desc_set_tbo_many[i];
      wds.pTexelBufferView = tbo_views[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }

   wds.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
   wds.descriptorCount = 1;
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_image); i++) {
      wds.dstSet = desc_set_image[i];
      wds.pImageInfo = dii_storage[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }

   wds.descriptorCount = MIN2(dev->info.props.limits.maxPerStageDescriptorStorageImages, MAX_IMAGES);
   wds.pImageInfo = dii_storage[0];
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_image_many); i++) {
      wds.dstSet = desc_set_image_many[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }

   wds.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
   wds.descriptorCount = 1;
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_ibo); i++) {
      wds.dstSet = desc_set_ibo[i];
      wds.pTexelBufferView = ibo_views[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
   }

   wds.descriptorCount = MIN2(dev->info.props.limits.maxPerStageDescriptorStorageImages, MAX_IMAGES);
   for (unsigned i = 0; i < ARRAY_SIZE(desc_set_ibo_many); i++) {
      wds.dstSet = desc_set_ibo_many[i];
      wds.pTexelBufferView = ibo_views[i];
      VK(UpdateDescriptorSets)(dev->dev, 1, &wds, 0, NULL);
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

static void
setup_submit(void)
{
   if (submit_init)
      return;

   /* ensure fresh pool */
   next_cmdbuf_pool();
   cmdbuf_idx = 0;
   count = 0;
   cmdbuf = cmdbufs[cmdbuf_pool_idx][cmdbuf_idx];
   /* fill every cmdbuf in pool with MAX_DRAWS */
   for (unsigned i = 0; i < MAX_CMDBUFS; i++) {
      begin_rp();
      draw(MAX_DRAWS);
      end_cmdbuf();
      count = 0;
      /* ensure the pool doesn't roll over */
      if (i < MAX_CMDBUFS - 1)
         next_cmdbuf();
   }
   cmdbuf_idx = 0;
   cmdbuf = cmdbufs[cmdbuf_pool_idx][cmdbuf_idx];
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
   struct perf_case *p;
   is_submit = false;
   if (case_idx < ARRAY_SIZE(cases_draw)) {
      p = &cases_draw[case_idx];
   } else if (case_idx < ARRAY_SIZE(cases_draw) + ARRAY_SIZE(cases_submit)) {
      p = &cases_submit[case_idx - ARRAY_SIZE(cases_draw)];
      is_submit = true;
   } else {
      p = &cases_descriptor[case_idx - ARRAY_SIZE(cases_draw) - ARRAY_SIZE(cases_submit)];
   }
   if (cmdbuf_active && (!is_submit || !submit_init)) {
      end_cmdbuf();
      next_cmdbuf();
   }
   bool unsupported = p->check_func && !p->check_func();
   is_dynamic = !!strstr(p->name, "dynamic");
   pipelines = p->pipelines;
   bool multirt = !!strstr(p->name, "multirt");
   set_render_info(p, multirt);
   if (is_submit)
      setup_submit();
   double rate = unsupported ? 0.0 : perf_measure_cpu_rate(p->func, duration);
   if (!multirt && strstr(p->name, "draw_multi"))
      rate *= ARRAY_SIZE(draws);
   double ratio = base_rate ? rate / base_rate : 1;

   const char *ratio_color = base_rate == 0 ? COLOR_RESET :
                                              ratio > 0.7 ? COLOR_GREEN :
                                              ratio > 0.4 ? COLOR_YELLOW : COLOR_RED;

   char space[50];
   memset(space, ' ', sizeof(space));
   space[sizeof(space) - strlen(p->name)] = 0;
   char space2[12];
   char buf[128];
   uint64_t r = is_submit ? (uint64_t)rate : (uint64_t)(rate / 1000lu);
   snprintf(buf, sizeof(buf), "%"PRIu64, r);
   memset(space2, ' ', sizeof(space2));
   space2[sizeof(space2) - strlen(buf)] = 0;
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
      printf(" %3u, %s,%s %s%5"PRIu64"%s, %s%sunsupported (smh %s)%s\n",
             case_idx, p->name,
             space,
             color ? COLOR_CYAN : "",
             r,
             color ? COLOR_RESET : "",
             "",
             color ? ratio_color : "",
             name,
             color ? COLOR_RESET : "");
   } else {
      printf(" %3u, %s,%s %s%5"PRIu64"%s, %s%s%.1f%%%s\n",
             case_idx, p->name,
             space,
             color ? COLOR_CYAN : "",
             r,
             color ? COLOR_RESET : "",
             space2,
             color ? ratio_color : "",
             100 * ratio,
             color ? COLOR_RESET : "");
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

   for (unsigned i = 0; i < MAX_CMDBUF_POOLS; i++) {
      util_queue_fence_init(&fence[i]);
      result = VK(CreateCommandPool)(dev->dev, &cpci, NULL, &cmdpool[i]);
      VK_CHECK("CreateCommandPool", result);
      cbai.commandPool = cmdpool[i];
      result = VK(AllocateCommandBuffers)(dev->dev, &cbai, cmdbufs[i]);
      VK_CHECK("AllocateCommandBuffers", result);
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
   bool next_arg_is_duration = false;
   for (int i = 0; i < argc; i++) {
      if (next_arg_is_test_no) {
         errno = 0;
         int val = strtol(argv[i], NULL, 10);
         if (errno || val < 0 || val >= TOTAL_CASES) {
            fprintf(stderr, "Invalid test number specified\n");
            abort();
         }
         test_no = val;
         if (test_no < ARRAY_SIZE(cases_draw))
            draw_only = true;
         else if (test_no < ARRAY_SIZE(cases_draw) + ARRAY_SIZE(cases_submit))
            submit_only = true;
         else
            descriptor_only = true;
      } else if (next_arg_is_duration) {
         errno = 0;
         double val = strtod(argv[i], NULL);
         if (errno || val < 0) {
            fprintf(stderr, "Invalid duration specified\n");
            abort();
         }
         duration = val;
      }
      next_arg_is_test_no = false;
      next_arg_is_duration = false;
      if (argv[i][0] != '-')
         continue;
      const char *arg = &argv[i][1];
      if (!strcmp(arg, "test"))
         next_arg_is_test_no = true;
      else if (!strcmp(arg, "duration"))
         next_arg_is_duration = true;
      else if (!strcmp(arg, "nocolor"))
         color = false;
      else if (!strcmp(arg, "submit-only"))
         submit_only = true;
      else if (!strcmp(arg, "draw-only"))
         draw_only = true;
      else if (!strcmp(arg, "descriptor-only"))
         descriptor_only = true;
      else if (!strcmp(arg, "list")) {
         for (unsigned i = 0; i < ARRAY_SIZE(cases_draw); i++)
            printf(" %3u, %s\n", i, cases_draw[i].name);
         for (unsigned i = 0; i < ARRAY_SIZE(cases_submit); i++)
            printf(" %3u, %s\n", i + (unsigned)ARRAY_SIZE(cases_draw), cases_submit[i].name);
         for (unsigned i = 0; i < ARRAY_SIZE(cases_descriptor); i++)
            printf(" %3u, %s\n", i + (unsigned)(ARRAY_SIZE(cases_draw) + ARRAY_SIZE(cases_descriptor)), cases_descriptor[i].name);
         exit(0);
      } else if (!strcmp(arg, "help") || !strcmp(arg, "h")) {
         fprintf(stderr, "vkoverhead [-list] [-test TESTNUM] [-nocolor] [-draw-only] [-submit-only] [-descriptor-only]\n");
         exit(0);
      }
   }
}

int
main(int argc, char *argv[])
{
   parse_args(argc, (const char**)argv);
   util_cpu_detect();
   dev = vk_device_create();
   init_cmdbufs();
   render_pass_clear = create_renderpass(1, true);
   render_pass_dontcare = create_renderpass(1, false);
   render_pass_multirt_clear = create_renderpass(8, true);
   render_pass_multirt_dontcare = create_renderpass(8, false);

   VkSamplerCreateInfo sci = {0};
   sci.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
   VkResult result = VK(CreateSampler)(dev->dev, &sci, NULL, &sampler);
   VK_CHECK("CreateSampler", result);

   init_dyn_render(&dyn_basic);
   dyn_basic.colorAttachmentCount = 1;
   dyn_basic.pColorAttachments = dyn_att_basic;
   init_dyn_render(&dyn_multirt);
   dyn_multirt.colorAttachmentCount = MAX_RTS;
   dyn_multirt.pColorAttachments = dyn_att_multirt;

   VkDescriptorUpdateTemplateEntry template_entry = {0};
   VkDescriptorUpdateTemplateCreateInfo tci = {0};
   tci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO;
   tci.descriptorUpdateEntryCount = 1;
   tci.templateType = VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_DESCRIPTOR_SET;
   tci.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
   tci.pDescriptorUpdateEntries = &template_entry;

   {
      VkDescriptorSetLayoutBinding binding = {0};
      binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      binding.descriptorCount = 1;
      binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      VkDescriptorSetLayout desc_layout = create_descriptor_layout(&binding, 1);
      VkDescriptorPoolSize size = {0};
      size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      size.descriptorCount = 1;
      for (unsigned i = 0; i < ARRAY_SIZE(desc_set_basic); i++)
         desc_set_basic[i] = create_descriptor_set(desc_layout, &size);
      layout_basic = create_pipeline_layout(&desc_layout, 1);
      if (dev->info.have_KHR_descriptor_update_template) {
         tci.descriptorSetLayout = desc_layout;
         tci.pipelineLayout = layout_basic;
         template_entry.descriptorCount = 1;
         template_entry.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
         template_entry.stride = sizeof(VkDescriptorBufferInfo);
         result = VK(CreateDescriptorUpdateTemplate)(dev->dev, &tci, NULL, &template_basic);
         VK_CHECK("CreateDescriptorUpdateTemplate", result);
      }
   }

   {
      VkDescriptorSetLayoutBinding binding = {0};
      binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      binding.descriptorCount = MAX_UBOS;
      binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      VkDescriptorSetLayout desc_layout = create_descriptor_layout(&binding, 1);
      VkDescriptorPoolSize size = {0};
      size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      size.descriptorCount = MAX_UBOS;
      for (unsigned i = 0; i < ARRAY_SIZE(desc_set_ubo); i++)
         desc_set_ubo[i] = create_descriptor_set(desc_layout, &size);
      layout_ubo = create_pipeline_layout(&desc_layout, 1);
      if (dev->info.have_KHR_descriptor_update_template) {
         tci.descriptorSetLayout = desc_layout;
         tci.pipelineLayout = layout_ubo;
         template_entry.descriptorCount = MAX_UBOS;
         template_entry.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
         template_entry.stride = sizeof(VkDescriptorBufferInfo);
         result = VK(CreateDescriptorUpdateTemplate)(dev->dev, &tci, NULL, &template_ubo);
         VK_CHECK("CreateDescriptorUpdateTemplate", result);
      }
   }

   {
      VkDescriptorSetLayoutBinding binding = {0};
      binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
      binding.descriptorCount = 1;
      binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      VkDescriptorSetLayout desc_layout = create_descriptor_layout(&binding, 1);
      VkDescriptorPoolSize size = {0};
      size.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
      size.descriptorCount = 1;
      for (unsigned i = 0; i < ARRAY_SIZE(desc_set_ssbo); i++)
         desc_set_ssbo[i] = create_descriptor_set(desc_layout, &size);
      layout_ssbo = create_pipeline_layout(&desc_layout, 1);
      if (dev->info.have_KHR_descriptor_update_template) {
         tci.descriptorSetLayout = desc_layout;
         tci.pipelineLayout = layout_ssbo;
         template_entry.descriptorCount = 1;
         template_entry.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
         template_entry.stride = sizeof(VkDescriptorBufferInfo);
         result = VK(CreateDescriptorUpdateTemplate)(dev->dev, &tci, NULL, &template_ssbo);
         VK_CHECK("CreateDescriptorUpdateTemplate", result);
      }
   }

   {
      VkDescriptorSetLayoutBinding binding = {0};
      binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
      binding.descriptorCount = MAX_SSBOS;
      binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      VkDescriptorSetLayout desc_layout = create_descriptor_layout(&binding, 1);
      VkDescriptorPoolSize size = {0};
      size.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
      size.descriptorCount = MAX_SSBOS;
      for (unsigned i = 0; i < ARRAY_SIZE(desc_set_ssbo_many); i++)
         desc_set_ssbo_many[i] = create_descriptor_set(desc_layout, &size);
      layout_ssbo_many = create_pipeline_layout(&desc_layout, 1);
      if (dev->info.have_KHR_descriptor_update_template) {
         tci.descriptorSetLayout = desc_layout;
         tci.pipelineLayout = layout_ssbo_many;
         template_entry.descriptorCount = MAX_SSBOS;
         template_entry.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
         template_entry.stride = sizeof(VkDescriptorBufferInfo);
         result = VK(CreateDescriptorUpdateTemplate)(dev->dev, &tci, NULL, &template_ssbo_many);
         VK_CHECK("CreateDescriptorUpdateTemplate", result);
      }
   }

   {
      VkDescriptorSetLayoutBinding binding = {0};
      binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      binding.descriptorCount = 1;
      binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      VkDescriptorSetLayout desc_layout = create_descriptor_layout(&binding, 1);
      VkDescriptorPoolSize size = {0};
      size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      size.descriptorCount = 1;
      for (unsigned i = 0; i < ARRAY_SIZE(desc_set_sampler); i++)
         desc_set_sampler[i] = create_descriptor_set(desc_layout, &size);
      layout_sampler = create_pipeline_layout(&desc_layout, 1);
      if (dev->info.have_KHR_descriptor_update_template) {
         tci.descriptorSetLayout = desc_layout;
         tci.pipelineLayout = layout_sampler;
         template_entry.descriptorCount = 1;
         template_entry.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
         template_entry.stride = sizeof(VkDescriptorImageInfo);
         result = VK(CreateDescriptorUpdateTemplate)(dev->dev, &tci, NULL, &template_sampler);
         VK_CHECK("CreateDescriptorUpdateTemplate", result);
      }
   }

   {
      VkDescriptorSetLayoutBinding binding = {0};
      binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      binding.descriptorCount = MAX_SAMPLERS;
      binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      VkDescriptorSetLayout desc_layout = create_descriptor_layout(&binding, 1);
      VkDescriptorPoolSize size = {0};
      size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      size.descriptorCount = MAX_SAMPLERS;
      for (unsigned i = 0; i < ARRAY_SIZE(desc_set_sampler_many); i++)
         desc_set_sampler_many[i] = create_descriptor_set(desc_layout, &size);
      layout_sampler_many = create_pipeline_layout(&desc_layout, 1);
      if (dev->info.have_KHR_descriptor_update_template) {
         tci.descriptorSetLayout = desc_layout;
         tci.pipelineLayout = layout_sampler_many;
         template_entry.descriptorCount = MAX_SAMPLERS;
         template_entry.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
         template_entry.stride = sizeof(VkDescriptorImageInfo);
         result = VK(CreateDescriptorUpdateTemplate)(dev->dev, &tci, NULL, &template_sampler_many);
         VK_CHECK("CreateDescriptorUpdateTemplate", result);
      }
   }

   {
      VkDescriptorSetLayoutBinding binding = {0};
      binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
      binding.descriptorCount = 1;
      binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      VkDescriptorSetLayout desc_layout = create_descriptor_layout(&binding, 1);
      VkDescriptorPoolSize size = {0};
      size.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
      size.descriptorCount = 1;
      for (unsigned i = 0; i < ARRAY_SIZE(desc_set_image); i++)
         desc_set_image[i] = create_descriptor_set(desc_layout, &size);
      layout_image = create_pipeline_layout(&desc_layout, 1);
      if (dev->info.have_KHR_descriptor_update_template) {
         tci.descriptorSetLayout = desc_layout;
         tci.pipelineLayout = layout_image;
         template_entry.descriptorCount = 1;
         template_entry.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
         template_entry.stride = sizeof(VkDescriptorImageInfo);
         result = VK(CreateDescriptorUpdateTemplate)(dev->dev, &tci, NULL, &template_image);
         VK_CHECK("CreateDescriptorUpdateTemplate", result);
      }
   }

   {
      VkDescriptorSetLayoutBinding binding = {0};
      binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
      binding.descriptorCount = MIN2(dev->info.props.limits.maxPerStageDescriptorStorageImages, MAX_IMAGES);
      binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      VkDescriptorSetLayout desc_layout = create_descriptor_layout(&binding, 1);
      VkDescriptorPoolSize size = {0};
      size.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
      size.descriptorCount = MIN2(dev->info.props.limits.maxPerStageDescriptorStorageImages, MAX_IMAGES);
      for (unsigned i = 0; i < ARRAY_SIZE(desc_set_image_many); i++)
         desc_set_image_many[i] = create_descriptor_set(desc_layout, &size);
      layout_image_many = create_pipeline_layout(&desc_layout, 1);
      if (dev->info.have_KHR_descriptor_update_template) {
         tci.descriptorSetLayout = desc_layout;
         tci.pipelineLayout = layout_image_many;
         template_entry.descriptorCount = MIN2(dev->info.props.limits.maxPerStageDescriptorStorageImages, MAX_IMAGES);
         template_entry.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
         template_entry.stride = sizeof(VkDescriptorImageInfo);
         result = VK(CreateDescriptorUpdateTemplate)(dev->dev, &tci, NULL, &template_image_many);
         VK_CHECK("CreateDescriptorUpdateTemplate", result);
      }
   }

   {
      VkDescriptorSetLayoutBinding binding = {0};
      binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
      binding.descriptorCount = 1;
      binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      VkDescriptorSetLayout desc_layout = create_descriptor_layout(&binding, 1);
      VkDescriptorPoolSize size = {0};
      size.type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
      size.descriptorCount = 1;
      for (unsigned i = 0; i < ARRAY_SIZE(desc_set_tbo); i++)
         desc_set_tbo[i] = create_descriptor_set(desc_layout, &size);
      layout_tbo = create_pipeline_layout(&desc_layout, 1);
      if (dev->info.have_KHR_descriptor_update_template) {
         tci.descriptorSetLayout = desc_layout;
         tci.pipelineLayout = layout_tbo;
         template_entry.descriptorCount = 1;
         template_entry.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
         template_entry.stride = sizeof(VkBufferView);
         result = VK(CreateDescriptorUpdateTemplate)(dev->dev, &tci, NULL, &template_tbo);
         VK_CHECK("CreateDescriptorUpdateTemplate", result);
      }
   }

   {
      VkDescriptorSetLayoutBinding binding = {0};
      binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
      binding.descriptorCount = MAX_SAMPLERS;
      binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      VkDescriptorSetLayout desc_layout = create_descriptor_layout(&binding, 1);
      VkDescriptorPoolSize size = {0};
      size.type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
      size.descriptorCount = MAX_SAMPLERS;
      for (unsigned i = 0; i < ARRAY_SIZE(desc_set_tbo_many); i++)
         desc_set_tbo_many[i] = create_descriptor_set(desc_layout, &size);
      layout_tbo_many = create_pipeline_layout(&desc_layout, 1);
      if (dev->info.have_KHR_descriptor_update_template) {
         tci.descriptorSetLayout = desc_layout;
         tci.pipelineLayout = layout_tbo_many;
         template_entry.descriptorCount = MAX_SAMPLERS;
         template_entry.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
         template_entry.stride = sizeof(VkBufferView);
         result = VK(CreateDescriptorUpdateTemplate)(dev->dev, &tci, NULL, &template_tbo_many);
         VK_CHECK("CreateDescriptorUpdateTemplate", result);
      }
   }

   {
      VkDescriptorSetLayoutBinding binding = {0};
      binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
      binding.descriptorCount = 1;
      binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      VkDescriptorSetLayout desc_layout = create_descriptor_layout(&binding, 1);
      VkDescriptorPoolSize size = {0};
      size.type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
      size.descriptorCount = 1;
      for (unsigned i = 0; i < ARRAY_SIZE(desc_set_ibo); i++)
         desc_set_ibo[i] = create_descriptor_set(desc_layout, &size);
      layout_ibo = create_pipeline_layout(&desc_layout, 1);
      if (dev->info.have_KHR_descriptor_update_template) {
         tci.descriptorSetLayout = desc_layout;
         tci.pipelineLayout = layout_ibo;
         template_entry.descriptorCount = 1;
         template_entry.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
         template_entry.stride = sizeof(VkBufferView);
         result = VK(CreateDescriptorUpdateTemplate)(dev->dev, &tci, NULL, &template_ibo);
         VK_CHECK("CreateDescriptorUpdateTemplate", result);
      }
   }

   {
      VkDescriptorSetLayoutBinding binding = {0};
      binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
      binding.descriptorCount = MIN2(dev->info.props.limits.maxPerStageDescriptorStorageImages, MAX_IMAGES);
      binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      VkDescriptorSetLayout desc_layout = create_descriptor_layout(&binding, 1);
      VkDescriptorPoolSize size = {0};
      size.type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
      size.descriptorCount = MIN2(dev->info.props.limits.maxPerStageDescriptorStorageImages, MAX_IMAGES);
      for (unsigned i = 0; i < ARRAY_SIZE(desc_set_ibo_many); i++)
         desc_set_ibo_many[i] = create_descriptor_set(desc_layout, &size);
      layout_ibo_many = create_pipeline_layout(&desc_layout, 1);
      if (dev->info.have_KHR_descriptor_update_template) {
         tci.descriptorSetLayout = desc_layout;
         tci.pipelineLayout = layout_ibo_many;
         template_entry.descriptorCount = MIN2(dev->info.props.limits.maxPerStageDescriptorStorageImages, MAX_IMAGES);
         template_entry.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
         template_entry.stride = sizeof(VkBufferView);
         result = VK(CreateDescriptorUpdateTemplate)(dev->dev, &tci, NULL, &template_ibo_many);
         VK_CHECK("CreateDescriptorUpdateTemplate", result);
      }
   }

   create_basic_pipelines(render_pass_clear, layout_basic, pipelines_basic);
   create_basic_pipelines(VK_NULL_HANDLE, layout_basic, pipelines_dyn);
   create_vattrib_pipelines(render_pass_clear, layout_basic, pipelines_vattrib);
   pipeline_vattrib_dynamic = create_vattrib_pipeline_dynamic(render_pass_clear, layout_basic);
   pipeline_multirt = create_multirt_pipeline(render_pass_multirt_clear, layout_basic);
   pipeline_multrt_dyn = create_multirt_pipeline(VK_NULL_HANDLE, layout_basic);
   pipeline_ubo = create_ubo_pipeline(render_pass_clear, layout_ubo);
   pipeline_ssbo = create_ssbo_pipeline(render_pass_clear, layout_ssbo);
   pipeline_ssbo_many = create_ssbo_many_pipeline(render_pass_clear, layout_ssbo_many);
   pipeline_sampler = create_sampler_pipeline(render_pass_clear, layout_sampler);
   pipeline_sampler_many = create_sampler_many_pipeline(render_pass_clear, layout_sampler_many);
   pipeline_image = create_image_pipeline(render_pass_clear, layout_image);
   pipeline_image_many = create_image_many_pipeline(render_pass_clear, layout_image_many);
   pipeline_tbo = create_tbo_pipeline(render_pass_clear, layout_tbo);
   pipeline_tbo_many = create_tbo_many_pipeline(render_pass_clear, layout_tbo_many);
   pipeline_ibo = create_ibo_pipeline(render_pass_clear, layout_ibo);
   pipeline_ibo_many = create_ibo_many_pipeline(render_pass_clear, layout_ibo_many);

   cmdbuf = cmdbufs[0][0];
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
   }
   next_cmdbuf();
   printf("vkoverhead running:\n");
   if (!submit_only && !descriptor_only)
      printf("\t* draw numbers are reported as thousands of operations per second\n"
             "\t* percentages for draw cases are relative to 'draw'\n");
   double base_rate = 0;
   if (test_no > -1) {
      if (!draw_only && !descriptor_only)
         printf("\t* submit numbers are reported as operations per second\n"
                "\t* percentages for submit cases are relative to 'submit_noop'\n");
      if (!draw_only && !submit_only)
         printf("\t* descriptor numbers are reported as thousands of operations per second\n"
                "\t* percentages for submit cases are relative to 'descriptor_noop'\n");
      perf_run(test_no, base_rate, duration);
   } else {
      if (!submit_only && !descriptor_only) {
         base_rate = perf_run(0, 0, duration);
         for (unsigned i = 1; i < ARRAY_SIZE(cases_draw); i++)
            perf_run(i, base_rate, duration);
      }
      if (!draw_only && !descriptor_only) {
         printf("\t* submit numbers are reported as operations per second\n"
                "\t* percentages for submit cases are relative to 'submit_noop'\n");
         base_rate = perf_run(ARRAY_SIZE(cases_draw), 0, duration);
         for (unsigned i = 1; i < ARRAY_SIZE(cases_submit); i++)
            perf_run(ARRAY_SIZE(cases_draw) + i, base_rate, duration);
         if (!submit_only) {
            /* avoid clobbering in-use resources */
            result = VK(QueueWaitIdle)(dev->queue);
            VK_CHECK("QueueWaitIdle", result);
         }
      }
      if (!draw_only && !submit_only) {
         printf("\t* descriptor numbers are reported as thousands of operations per second\n"
                "\t* percentages for submit cases are relative to 'descriptor_noop'\n");
         base_rate = perf_run(ARRAY_SIZE(cases_draw) + ARRAY_SIZE(cases_submit), 0, duration);
         for (unsigned i = 1; i < ARRAY_SIZE(cases_descriptor); i++)
            perf_run(ARRAY_SIZE(cases_draw) + ARRAY_SIZE(cases_submit) +  + i, base_rate, duration);
      }
   }

   return 0;
}
