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

VkRenderPass
create_renderpass(unsigned num_rts, bool clear)
{
   VkAttachmentReference2 color_ref[MAX_RTS] = {0};
   VkAttachmentDescription2 attachment[MAX_RTS] = {0};
   VkPipelineStageFlags dep_pipeline = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
   VkAccessFlags dep_access = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

   if (dev->info.props.limits.maxColorAttachments < 8) {
      fprintf(stderr, "maxColorAttachments >= 8 required!\n");
      abort();
   }

   for (unsigned i = 0; i < MAX_RTS; i++) {
      attachment[i].sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
      attachment[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
      attachment[i].samples = VK_SAMPLE_COUNT_1_BIT;
      attachment[i].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attachment[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      attachment[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attachment[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attachment[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      attachment[i].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      color_ref[i].sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
      color_ref[i].attachment = i;
      color_ref[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      color_ref[i].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
   }

   VkSubpassDependency2 deps[] = {
      {VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2, NULL, VK_SUBPASS_EXTERNAL, 0, dep_pipeline, dep_pipeline, 0, dep_access, 0, 0},
      {VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2, NULL, 0, VK_SUBPASS_EXTERNAL, dep_pipeline, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, dep_access, 0, 0, 0}
   };

   VkSubpassDescription2 subpass = {0};
   subpass.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
   subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
   subpass.colorAttachmentCount = num_rts;
   subpass.pColorAttachments = color_ref;

   VkRenderPassCreateInfo2 rpci = {0};
   rpci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2;
   rpci.attachmentCount = num_rts;
   rpci.pAttachments = attachment;
   rpci.subpassCount = 1;
   rpci.pSubpasses = &subpass;
   rpci.dependencyCount = 2;
   rpci.pDependencies = deps;

   VkRenderPass render_pass;
   VkResult result = VK(CreateRenderPass2)(dev->dev, &rpci, NULL, &render_pass);
   VK_CHECK("vkCreateRenderPass2", result);
   return render_pass;
}


VkFramebuffer
create_framebuffer(unsigned num_rts, VkRenderPass render_pass)
{
   VkImageView image_views[MAX_RTS];
   for (unsigned i = 0; i < num_rts; i++)
      image_views[i] = create_rt(NULL);
   VkFramebufferCreateInfo fci = {0};
   fci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
   fci.renderPass = render_pass;
   fci.attachmentCount = num_rts;
   fci.pAttachments = image_views;
   fci.width = 100;
   fci.height = 100;
   fci.layers = 1;

   VkFramebuffer fb;
   VkResult result = VK(CreateFramebuffer)(dev->dev, &fci, NULL, &fb);
   VK_CHECK("CreateFramebuffer", result);
   return fb;
}
