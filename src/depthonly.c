/*
 * Copyright 2023 Valve Corporation
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

#include "data/depthonly/01.09146e52defab964.x"
#include "data/depthonly/01.0bee72a75e9b777d.x"
#include "data/depthonly/01.48465701329d9e2c.x"
#include "data/depthonly/01.5d3d8cb814dcb447.x"
#include "data/depthonly/01.6898ef3a01c2d55c.x"
#include "data/depthonly/02.800e7eefee23e6f4.x"
#include "data/depthonly/02.88d1ffa45096fb09.x"
#include "data/depthonly/02.cb3985cfac50f434.x"
#include "data/depthonly/03.780aa78157656308.x"
#include "data/depthonly/06.022584b7c03a4127.x"
#include "data/depthonly/06.0de5ebd3a8b0d81d.x"
#include "data/depthonly/06.164cf1086d7f78be.x"
#include "data/depthonly/06.4fa452ff1edadbe1.x"
#include "data/depthonly/depthonly.x"

VkPipelineLayout
depthonly_init(VkPipeline *pipelines)
{
   VkDescriptorSetLayout dsl[4] = {
      dsl_88d1ffa45096fb09(),
      VK_NULL_HANDLE,
      dsl_cb3985cfac50f434(),
      dsl_800e7eefee23e6f4()
   };
   VkPipelineLayout layout = layout_780aa78157656308(dsl);
   VkShaderModule modules[1] = {create_shader_module(depthonly_spv, depthonly_spv_len)};
   pipelines[0] = pso_0de5ebd3a8b0d81d(VK_NULL_HANDLE, VK_NULL_HANDLE, NULL);
   pipelines[1] = pso_164cf1086d7f78be(VK_NULL_HANDLE, VK_NULL_HANDLE, NULL);
   pipelines[2] = pso_4fa452ff1edadbe1(layout, VK_NULL_HANDLE, modules);
   pipelines[3] = pso_022584b7c03a4127(layout, VK_NULL_HANDLE, NULL);
   return layout;
}