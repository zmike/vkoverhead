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


#include "data/slow/01.09146e52defab964.x"
#include "data/slow/01.0bee72a75e9b777d.x"
#include "data/slow/01.0bf53ea75ea13dcf.x"
#include "data/slow/01.48465701329d9e2c.x"
#include "data/slow/01.5d3d8cb814dcb447.x"
#include "data/slow/01.6898ef3a01c2d55c.x"
#include "data/slow/01.6e9082b81eacb19d.x"
#include "data/slow/02.800e7eefee23e6f4.x"
#include "data/slow/02.88d1ffa45096fb09.x"
#include "data/slow/02.aed3d6739c41ce31.x"
#include "data/slow/03.c6d2fe52447ad8aa.x"
#include "data/slow/06.2a09cbc3ec439860.x"
#include "data/slow/06.33faa40f0177b2b2.x"
#include "data/slow/06.383d888f441822ed.x"
#include "data/slow/06.3e2e614d4775bbb0.x"
#include "data/slow/06.6d50553122782d72.x"
#include "data/slow/slow_frag.x"
#include "data/slow/slow_vert.x"


VkPipelineLayout
slow_init(VkPipeline *pipelines)
{
   VkDescriptorSetLayout dsl[] = {
      dsl_88d1ffa45096fb09(),
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      dsl_800e7eefee23e6f4(),
      dsl_aed3d6739c41ce31(),
   };
   VkPipelineLayout layout = layout_c6d2fe52447ad8aa(dsl);
   VkShaderModule vs = create_shader_module(slow_vert_spv, slow_vert_spv_len);
   VkShaderModule fs = create_shader_module(slow_frag_spv, slow_frag_spv_len);
   pipelines[0] = pso_6d50553122782d72(VK_NULL_HANDLE, VK_NULL_HANDLE, NULL);
   pipelines[1] = pso_3e2e614d4775bbb0(VK_NULL_HANDLE, VK_NULL_HANDLE, NULL);
   pipelines[2] = pso_383d888f441822ed(layout, VK_NULL_HANDLE, &vs);
   pipelines[3] = pso_2a09cbc3ec439860(layout, VK_NULL_HANDLE, &fs);
   return layout;
}