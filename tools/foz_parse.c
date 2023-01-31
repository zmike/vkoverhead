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

#include <stdlib.h>
#include <stdio.h>
#include "cJSON.h"
#include "os_file.h"
#include "macros.h"
#include "u_dynarray.h"
#include "hash_table.h"
#include "util/bitscan.h"
#include "vk_enum_to_str.h"

struct parser {
   const char *name;
   void (*func)(cJSON *c, FILE *f);
};

static struct util_dynarray jsons[8];
static struct util_dynarray jsonfiles[8];

#define INT(name) \
      } else if (!strcmp(i->string, #name)) { \
         fprintf(f, IND"%s."#name" = %"PRIi64";\n", str, i->valueint)
#define FLOAT(name) \
      } else if (!strcmp(i->string, #name)) { \
         fprintf(f, IND"%s."#name" = %f;\n", str, (float)i->valuedouble)
#define INT_STORE(name, store) \
      } else if (!strcmp(i->string, #name)) { \
         (store) = i->valueint; \
         fprintf(f, IND"%s."#name" = %"PRIi64";\n", str, i->valueint)

#define PTR(name) \
      } else if (!strcmp(i->string, #name)) { \
         fprintf(f, IND"%s."#name" = "#name";\n", str)

#define STRUCT(name, sname, parser) \
      } else if (!strcmp(i->string, #name)) { \
         if (i->type != cJSON_NULL) { \
            parser(i, f); \
            fprintf(f, IND"%s."#sname" = &"#name";\n\n", str); \
         }

#define IND "   "

static void
parse_array(cJSON *c, FILE *f, const char *str, const char *member, const char *type, void (*cb)(cJSON *c, FILE *f, const char *name, unsigned iter));
static bool
parse_gp_member(cJSON *c, FILE *f, const char *str);
static void
parse_pnext(cJSON *c, FILE *f, const char *parent_str, const char *parent_type);

static void
parse_array_format(cJSON *c, FILE *f, const char *name, unsigned iter)
{
   fprintf(f, IND"%s[%u] = %s;\n", name, iter, vk_Format_to_str(c->valueint));
}

static void
parse_array_uint(cJSON *c, FILE *f, const char *name, unsigned iter)
{
   fprintf(f, IND"%s[%u] = %u;\n", name, iter, (unsigned)c->valueint);
}

static void
parse_array_inputbinding(cJSON *c, FILE *f, const char *name, unsigned iter)
{
   fprintf(f, IND"%s[%u] = (VkVertexInputBindingDescription){%"PRIi64", %"PRIi64", %"PRIi64"};\n",name, iter, 
           cJSON_GetObjectItemCaseSensitive(c, "binding")->valueint,
           cJSON_GetObjectItemCaseSensitive(c, "stride")->valueint,
           cJSON_GetObjectItemCaseSensitive(c, "inputRate")->valueint);
}

static void
parse_array_attrdesc(cJSON *c, FILE *f, const char *name, unsigned iter)
{
   fprintf(f, IND"%s[%u] = (VkVertexInputAttributeDescription){%"PRIi64", %"PRIi64", %"PRIi64", %"PRIi64"};\n", name, iter, 
           cJSON_GetObjectItemCaseSensitive(c, "location")->valueint,
           cJSON_GetObjectItemCaseSensitive(c, "binding")->valueint,
           cJSON_GetObjectItemCaseSensitive(c, "format")->valueint,
           cJSON_GetObjectItemCaseSensitive(c, "offset")->valueint);
}

static void
parse_array_viewport(cJSON *c, FILE *f, const char *name, unsigned iter)
{
   fprintf(f, IND"%s[%u] = {%f, %f, %f, %f, %f, %f};\n", name, iter, 
           (float)cJSON_GetObjectItemCaseSensitive(c, "x")->valuedouble,
           (float)cJSON_GetObjectItemCaseSensitive(c, "y")->valuedouble,
           (float)cJSON_GetObjectItemCaseSensitive(c, "width")->valuedouble,
           (float)cJSON_GetObjectItemCaseSensitive(c, "height")->valuedouble,
           (float)cJSON_GetObjectItemCaseSensitive(c, "minDepth")->valuedouble,
           (float)cJSON_GetObjectItemCaseSensitive(c, "maxDepth")->valuedouble);
}

static void
parse_array_rect2d(cJSON *c, FILE *f, const char *name, unsigned iter)
{
   fprintf(f, IND"%s[%u] = {{%d, %d}, {%d, %d}};\n", name, iter, 
           (int)cJSON_GetObjectItemCaseSensitive(c, "x")->valueint,
           (int)cJSON_GetObjectItemCaseSensitive(c, "y")->valueint,
           (int)cJSON_GetObjectItemCaseSensitive(c, "width")->valueint,
           (int)cJSON_GetObjectItemCaseSensitive(c, "height")->valueint);
}

static void
parse_array_blendatt(cJSON *c, FILE *f, const char *name, unsigned iter)
{
   fprintf(f, IND"%s[%u] = (VkPipelineColorBlendAttachmentState){%d, %d, %d, %d, %d, %d, %d, %d};\n", name, iter, 
           (int)cJSON_GetObjectItemCaseSensitive(c, "blendEnable")->valueint,
           (int)cJSON_GetObjectItemCaseSensitive(c, "srcColorBlendFactor")->valueint,
           (int)cJSON_GetObjectItemCaseSensitive(c, "dstColorBlendFactor")->valueint,
           (int)cJSON_GetObjectItemCaseSensitive(c, "colorBlendOp")->valueint,
           (int)cJSON_GetObjectItemCaseSensitive(c, "srcAlphaBlendFactor")->valueint,
           (int)cJSON_GetObjectItemCaseSensitive(c, "dstAlphaBlendFactor")->valueint,
           (int)cJSON_GetObjectItemCaseSensitive(c, "alphaBlendOp")->valueint,
           (int)cJSON_GetObjectItemCaseSensitive(c, "colorWriteMask")->valueint);
}

static void
parse_specinfo(cJSON *c, FILE *f, const char *parent_str)
{
   const char *id = strchr(parent_str, '[');
   int idx = strtol(id + 1, NULL, 10);
   char str[4096];
   snprintf(str, sizeof(str), "specinfo_%d", idx);
   fprintf(f, IND"VkSpecializationInfo %s = {0};\n", str);

   /* need to see this in the wild... */
   fprintf(stderr, "unfinished spec constant support!\n");
   abort();
}

static void
parse_array_shader(cJSON *c, FILE *f, const char *name, unsigned iter)
{
   char str[4096];
   snprintf(str, sizeof(str), "%s[%u]", name, iter);
   if (!cJSON_GetObjectItemCaseSensitive(c, "pNext"))
      fprintf(f, IND"%s.pNext = NULL;\n", str);
   fprintf(f, IND"%s.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;\n", str);
   cJSON *i;
   bool have_spec = false;
   cJSON_ArrayForEach(i, c) {
      if (0) {
      INT(flags);
      INT(stage);
      } else if (!strcmp(i->string, "module")) {
         fprintf(f, IND"%s.module = modules[%u];\n", str, iter);
      } else if (!strcmp(i->string, "name")) {
         fprintf(f, IND"%s.pName = \"%s\";\n", str, i->valuestring);
      } else if (!strcmp(i->string, "specializationInfo")) {
         have_spec = true;
         parse_specinfo(i, f, str);
      // STRUCT(specializationInfo, pSpecializationInfo, parse_specinfo);
      } else {
         fprintf(stderr, "Unhandled VkPipelineShaderStageCreateInfo member: %s\n", i->string);
         abort();
      }
   }
   if (!have_spec)
      fprintf(f, IND"%s.pSpecializationInfo = NULL;\n", str);
}

static void
parse_array_sampler(cJSON *c, FILE *f, const char *name, unsigned iter)
{
   /* TODO: some way to free these? */
   fprintf(f, IND"%s[%u] = sampler_%s();\n", name, iter, c->valuestring);
}

static void
parse_array_dsl_binding(cJSON *c, FILE *f, const char *name, unsigned iter)
{
   char str[4096];
   snprintf(str, sizeof(str), "%s[%u]", name, iter);
   cJSON *i;
   bool have_immutable = false;
   cJSON_ArrayForEach(i, c) {
      if (0) {
      INT(binding);
      INT(descriptorType);
      INT(descriptorCount);
      INT(stageFlags);
      } else if (!strcmp(i->string, "immutableSamplers")) {
         parse_array(i, f, str, "pImmutableSamplers", "VkSampler", parse_array_sampler);
         have_immutable = true;
      } else {
         fprintf(stderr, "Unhandled VkDescriptorSetLayoutBinding member: %s\n", i->string);
         abort();
      }
   }
   if (!have_immutable)
      fprintf(f, IND"%s.pImmutableSamplers = NULL;\n", str);
}

static void
parse_array_push_const_range(cJSON *c, FILE *f, const char *name, unsigned iter)
{
   fprintf(f, IND"%s[%u] = (VkPushConstantRange){%d, %d, %d};\n", name, iter, 
           (int)cJSON_GetObjectItemCaseSensitive(c, "stageFlags")->valueint,
           (int)cJSON_GetObjectItemCaseSensitive(c, "offset")->valueint,
           (int)cJSON_GetObjectItemCaseSensitive(c, "size")->valueint);
}

static void
parse_array(cJSON *c, FILE *f, const char *str, const char *member, const char *type, void (*cb)(cJSON *c, FILE *f, const char *name, unsigned iter))
{
   char name[4096];
   unsigned len = snprintf(name, sizeof(name), "%s_%s", str, type);
   for (unsigned i = 0; i < len; i++) {
      switch (name[i]) {
      case '[':
      case ']':
         name[i] = '_';
         break;
      default: break;
      }
   }
   fprintf(f, IND"%s %s[%u];\n", type, name, cJSON_GetArraySize(c));
   unsigned iter = 0;
   cJSON *i;
   cJSON_ArrayForEach(i, c)
      cb(i, f, name, iter++);
   fprintf(f, IND"%s.%s = %s;\n", str, member, name);
}

static bool
parse_gp_member(cJSON *i, FILE *f, const char *str)
{
   if (0) {
   INT(flags);
   /* VkPipelineRenderingCreateInfo */
   INT(depthAttachmentFormat);
   INT(stencilAttachmentFormat);
   INT(viewMask);
   INT(colorAttachmentCount);
   } else if (!strcmp(i->string, "colorAttachmentFormats")) {
      fprintf(f, IND"%s.colorAttachmentCount = %d;\n", str, cJSON_GetArraySize(i));
      parse_array(i, f, str, "pColorAttachmentFormats", "VkFormat", parse_array_format);
   /* VkPipelineVertexInputStateCreateInfo */
   INT(vertexBindingDescriptionCount);
   } else if (!strcmp(i->string, "bindings")) {
      fprintf(f, IND"%s.vertexBindingDescriptionCount = %d;\n", str, cJSON_GetArraySize(i));
      parse_array(i, f, str, "pVertexBindingDescriptions", "VkVertexInputBindingDescription", parse_array_inputbinding);
   INT(vertexAttributeDescriptionCount);
   } else if (!strcmp(i->string, "attributes")) {
      fprintf(f, IND"%s.vertexAttributeDescriptionCount = %d;\n", str, cJSON_GetArraySize(i));
      parse_array(i, f, str, "pVertexAttributeDescriptions", "VkVertexInputAttributeDescription", parse_array_attrdesc);
   /* VkPipelineInputAssemblyStateCreateInfo */
   INT(topology);
   INT(primitiveRestartEnable);
   /* VkPipelineTessellationStateCreateInfo */
   INT(patchControlPoints);
   /* VkPipelineViewportStateCreateInfo */
   INT(viewportCount);
   } else if (!strcmp(i->string, "viewports")) {
      fprintf(f, IND"%s.viewportCount = %d;\n", str, cJSON_GetArraySize(i));
      parse_array(i, f, str, "pViewports", "VkViewport", parse_array_viewport);
   } else if (!strcmp(i->string, "scissors")) {
      fprintf(f, IND"%s.scissorCount = %d;\n", str, cJSON_GetArraySize(i));
      parse_array(i, f, str, "pScissors", "VkRect2D", parse_array_rect2d);
   INT(scissorCount);
   /* VkPipelineRasterizationStateCreateInfo */
   INT(depthClampEnable);
   INT(rasterizerDiscardEnable);
   INT(polygonMode);
   INT(cullMode);
   INT(frontFace);
   INT(depthBiasEnable);
   FLOAT(depthBiasConstantFactor);
   FLOAT(depthBiasClamp);
   FLOAT(depthBiasSlopeFactor);
   FLOAT(lineWidth);
   /* VkPipelineMultisampleStateCreateInfo */
   INT(rasterizationSamples);
   INT(sampleShadingEnable);
   FLOAT(minSampleShading);
   INT(alphaToCoverageEnable);
   INT(alphaToOneEnable);
   } else if (!strcmp(i->string, "sampleMask")) {
      parse_array(i, f, str, "pSampleMask", "VkSampleMask", parse_array_uint);
   /* VkPipelineDepthStencilStateCreateInfo */
   INT(depthTestEnable);
   INT(depthWriteEnable);
   INT(depthCompareOp);
   INT(depthBoundsTestEnable);
   INT(stencilTestEnable);
   INT(stencilTestEnable);
   } else if (!strcmp(i->string, "front") || !strcmp(i->string, "back")) {
      fprintf(f, IND"%s.%s = (VkStencilOpState){%d, %d, %d, %d, %d, %d, %d};\n", str, i->string,
              (int)cJSON_GetObjectItemCaseSensitive(i, "failOp")->valueint,
              (int)cJSON_GetObjectItemCaseSensitive(i, "passOp")->valueint,
              (int)cJSON_GetObjectItemCaseSensitive(i, "depthFailOp")->valueint,
              (int)cJSON_GetObjectItemCaseSensitive(i, "compareOp")->valueint,
              (int)cJSON_GetObjectItemCaseSensitive(i, "compareMask")->valueint,
              (int)cJSON_GetObjectItemCaseSensitive(i, "writeMask")->valueint,
              (int)cJSON_GetObjectItemCaseSensitive(i, "reference")->valueint);
   FLOAT(minDepthBounds);
   FLOAT(maxDepthBounds);
   /* VkPipelineColorBlendStateCreateInfo */
   INT(logicOpEnable);
   INT(logicOp);
   INT(attachmentCount);
   } else if (!strcmp(i->string, "attachments")) {
      fprintf(f, IND"%s.attachmentCount = %d;\n", str, cJSON_GetArraySize(i));
      parse_array(i, f, str, "pAttachments", "VkPipelineColorBlendAttachmentState", parse_array_blendatt);
   } else if (!strcmp(i->string, "blendConstants")) {
      for (unsigned iter = 0; iter < 4; iter++)
        fprintf(f, IND"%s.blendConstants[%u] = %f;\n", str, iter, (float)cJSON_GetArrayItem(i, iter)->valuedouble);
   /* VkPipelineDynamicStateCreateInfo */
   INT(dynamicStateCount);
   } else if (!strcmp(i->string, "dynamicState")) {
      fprintf(f, IND"%s.dynamicStateCount = %d;\n", str, cJSON_GetArraySize(i));
      parse_array(i, f, str, "pDynamicStates", "VkDynamicState", parse_array_uint);
   /* VkPipelineLibraryCreateInfoKHR */
   } else if (!strcmp(i->string, "libraries")) {
      fprintf(f, IND"%s.libraryCount = %d;\n", str, cJSON_GetArraySize(i));
      fprintf(f, IND"%s.pLibraries = shader_ptr;\n", str);
   } else {
      return false;
   }
   return true;
}

static void
parse_pnext(cJSON *c, FILE *f, const char *parent_str, const char *parent_type)
{
   const char *str = NULL;
   cJSON *i, *m;
   cJSON_ArrayForEach(m, c) {
      i = cJSON_GetObjectItem(m, "sType");
      const char *type = NULL;
      str = vk_StructureType_to_str(i->valueint);
      str += strlen("VK_STRUCTURE_TYPE_");
      switch (i->valueint) {
      case VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO:
         type = "VkPipelineRenderingCreateInfo";
         break;
      case VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT:
         type = "VkGraphicsPipelineLibraryCreateInfoEXT";
         break;
      case VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR:
         type = "VkPipelineLibraryCreateInfoKHR";
         break;
      default:
         fprintf(stderr, "Unhandled %s pNext: %s\n", parent_type, vk_StructureType_to_str(i->valueint));
         abort();
      }
      fprintf(f, IND"%s %s = {0};\n", type, str);
      fprintf(f, IND"%s.pNext = (void*)%s.pNext;\n", str, parent_str);
      fprintf(f, IND"%s.pNext = &%s;\n", parent_str, str);
      fprintf(f, IND"%s.sType = %"PRIi64";\n", str, i->valueint);
      cJSON_ArrayForEach(i, m) {
         if (!strcmp(i->string, "sType")) {
         } else if (!parse_gp_member(i, f, str)) {
            fprintf(stderr, "Unhandled %s pNext member: %s\n", parent_type, i->string);
            abort();
         }
      }
      fprintf(f, "\n");
   }
}

#define PARSE_STRUCT(var, type, stype, member_parser) \
static void \
parse_##var(cJSON *c, FILE *f) \
{ \
   const char *str = #var; \
   fprintf(f, IND #type" "#var" = {0};\n"); \
   fprintf(f, IND #var".sType = "#stype";\n"); \
   cJSON *i; \
   cJSON_ArrayForEach(i, c) { \
      if (!strcmp(i->string, "pNext")) { \
         parse_pnext(i, f, str, #type); \
      } else if (!member_parser(i, f, str)) { \
         fprintf(stderr, "Unhandled "#type" member: %s\n", i->string); \
         abort(); \
      } \
   } \
}

#define PARSE_GP_STRUCT(var, type, stype) PARSE_STRUCT(var, type, stype, parse_gp_member)

PARSE_GP_STRUCT(vertexInputState, VkPipelineVertexInputStateCreateInfo, VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO)
PARSE_GP_STRUCT(inputAssemblyState, VkPipelineInputAssemblyStateCreateInfo, VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO)
PARSE_GP_STRUCT(tessellationState, VkPipelineTessellationStateCreateInfo, VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO)
PARSE_GP_STRUCT(viewportState, VkPipelineViewportStateCreateInfo, VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO)
PARSE_GP_STRUCT(rasterizationState, VkPipelineRasterizationStateCreateInfo, VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO)
PARSE_GP_STRUCT(multisampleState, VkPipelineMultisampleStateCreateInfo, VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO)
PARSE_GP_STRUCT(depthStencilState, VkPipelineDepthStencilStateCreateInfo, VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO)
PARSE_GP_STRUCT(colorBlendState, VkPipelineColorBlendStateCreateInfo, VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO)
PARSE_GP_STRUCT(dynamicState, VkPipelineDynamicStateCreateInfo, VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO)

static void
parse_gp(cJSON *c, FILE *f)
{
   fprintf(f, "static VkPipeline\n");
   fprintf(f, "pso_%s(VkPipelineLayout layout, VkRenderPass renderPass, void *shader_ptr)\n", c->string);
   fprintf(f, "{\n");
   fprintf(f, IND"VkGraphicsPipelineCreateInfo pci = {0};\n");
   fprintf(f, IND"pci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;\n");

   cJSON *i;
   const char *str = "pci";
   cJSON_ArrayForEach(i, c) {
      if (0) {
      INT(flags);
      INT(subpass);
      INT(stageCount);
      PTR(layout);
      PTR(renderPass);
      STRUCT(vertexInputState, pVertexInputState, parse_vertexInputState);
      STRUCT(inputAssemblyState, pInputAssemblyState, parse_inputAssemblyState);
      STRUCT(tessellationState, pTessellationState, parse_tessellationState);
      STRUCT(viewportState, pViewportState, parse_viewportState);
      STRUCT(rasterizationState, pRasterizationState, parse_rasterizationState);
      STRUCT(multisampleState, pMultisampleState, parse_multisampleState);
      STRUCT(depthStencilState, pDepthStencilState, parse_depthStencilState);
      STRUCT(colorBlendState, pColorBlendState, parse_colorBlendState);
      STRUCT(dynamicState, pDynamicState, parse_dynamicState);
      } else if (!strcmp(i->string, "stages")) {
         fprintf(f, IND"%s.stageCount = %d;\n", str, cJSON_GetArraySize(i));
         if (i->child) {
            fprintf(f, "VkShaderModule *modules = (VkShaderModule *)shader_ptr;\n");
            parse_array(i, f, str, "pStages", "VkPipelineShaderStageCreateInfo", parse_array_shader);
         }
      } else if (!strcmp(i->string, "pNext")) {
         parse_pnext(i, f, str, "VkGraphicsPipelineCreateInfo");
      }
   }

   fprintf(f, IND"VkPipeline pipeline;\n");
   fprintf(f, IND"VK(CreateGraphicsPipelines)(dev->dev, VK_NULL_HANDLE, 1, &pci, NULL, &pipeline);\n");
   fprintf(f, IND"return pipeline;\n");
   fprintf(f, "}\n");
}

static void
parse_graphicspipelines(cJSON *c, FILE *f)
{
   cJSON *i;
   cJSON_ArrayForEach(i, c) {
      parse_gp(i, f);
   }
}

static void
parse_setlayout(cJSON *c, FILE *f)
{
   fprintf(f, "static VkDescriptorSetLayout\n");
   fprintf(f, "dsl_%s(void)\n", c->string);
   fprintf(f, "{\n");
   fprintf(f, IND"VkDescriptorSetLayoutCreateInfo dslci = {0};\n");
   fprintf(f, IND"dslci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;\n");

   cJSON *i;
   const char *str = "dslci";
   cJSON_ArrayForEach(i, c) {
      if (0) {
      INT(flags);
      } else if (!strcmp(i->string, "bindings")) {
         fprintf(f, IND"%s.bindingCount = %d;\n", str, cJSON_GetArraySize(i));
         parse_array(i, f, str, "pBindings", "VkDescriptorSetLayoutBinding", parse_array_dsl_binding);
      } else if (!strcmp(i->string, "pNext")) {
         parse_pnext(i, f, str, "VkDescriptorSetLayoutCreateInfo");
      }
   }

   fprintf(f, IND"VkDescriptorSetLayout dsl;\n");
   fprintf(f, IND"VK(CreateDescriptorSetLayout)(dev->dev, &dslci, 0, &dsl);\n");
   fprintf(f, IND"return dsl;\n");
   fprintf(f, "}\n");
}

static void
parse_setlayouts(cJSON *c, FILE *f)
{
   cJSON *i;
   cJSON_ArrayForEach(i, c) {
      parse_setlayout(i, f);
   }
}

static bool
parse_sampler_member(cJSON *i, FILE *f, const char *str)
{
   if (0) {
   INT(flags);
   INT(magFilter);
   INT(minFilter);
   INT(mipmapMode);
   INT(addressModeU);
   INT(addressModeV);
   INT(addressModeW);
   FLOAT(mipLodBias);
   INT(anisotropyEnable);
   FLOAT(maxAnisotropy);
   INT(compareEnable);
   INT(compareOp);
   FLOAT(minLod);
   FLOAT(maxLod);
   INT(borderColor);
   INT(unnormalizedCoordinates);
   } else {
      return false;
   }
   return true;
}

PARSE_STRUCT(samplerCreateInfo, VkSamplerCreateInfo, VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO, parse_sampler_member)

static void
parse_sampler(cJSON *c, FILE *f)
{
   fprintf(f, "static VkSampler\n");
   fprintf(f, "sampler_%s(void)\n", c->string);
   fprintf(f, "{\n");

   parse_samplerCreateInfo(c, f);

   fprintf(f, IND"VkSampler sampler;\n");
   fprintf(f, IND"VK(CreateSampler)(dev->dev, &samplerCreateInfo, 0, &sampler);\n");
   fprintf(f, IND"return sampler;\n");
   fprintf(f, "}\n");
}

static void
parse_samplers(cJSON *c, FILE *f)
{
   cJSON *i;
   cJSON_ArrayForEach(i, c) {
      parse_sampler(i, f);
   }
}

static bool
parse_layout_member(cJSON *i, FILE *f, const char *str)
{
   if (0) {
   INT(flags);
   } else if (!strcmp(i->string, "setLayouts")) {
      fprintf(f, IND"%s.setLayoutCount = %d;\n", str, cJSON_GetArraySize(i));
      fprintf(f, IND"%s.pSetLayouts = dsl;\n", str);
   } else if (!strcmp(i->string, "pushConstantRanges")) {
      fprintf(f, IND"%s.pushConstantRangeCount = %d;\n", str, cJSON_GetArraySize(i));
      parse_array(i, f, str, "pPushConstantRanges", "VkPushConstantRange", parse_array_push_const_range);
   } else {
      return false;
   }
   return true;
}

PARSE_STRUCT(layoutCreateInfo, VkPipelineLayoutCreateInfo, VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, parse_layout_member)

static void
parse_pipelinelayout(cJSON *c, FILE *f)
{
   fprintf(f, "static VkPipelineLayout\n");
   fprintf(f, "layout_%s(VkDescriptorSetLayout *dsl)\n", c->string);
   fprintf(f, "{\n");

   parse_layoutCreateInfo(c, f);

   fprintf(f, IND"VkPipelineLayout layout;\n");
   fprintf(f, IND"VK(CreatePipelineLayout)(dev->dev, &layoutCreateInfo, 0, &layout);\n");
   fprintf(f, IND"return layout;\n");
   fprintf(f, "}\n");
}

static void
parse_pipelinelayouts(cJSON *c, FILE *f)
{
   cJSON *i;
   cJSON_ArrayForEach(i, c) {
      parse_pipelinelayout(i, f);
   }
}

static struct parser parsers[] = {
   {"samplers", parse_samplers},
   {"setLayouts", parse_setlayouts},
   {"pipelineLayouts", parse_pipelinelayouts},
   {"graphicsPipelines", parse_graphicspipelines},
};

int
main(int argc, char *argv[])
{
   if (argc < 2) {
      fprintf(stderr, "Usage: %s <filename.json [filename2.json ...]>\n", argv[0]);
      exit(1);
   }
   for (unsigned i = 0; i < ARRAY_SIZE(jsons); i++) {
      util_dynarray_init(&jsons[i], NULL);
      util_dynarray_init(&jsonfiles[i], NULL);
   }

   for (unsigned i = 1; i < argc; i++) {
      size_t size;
      char *json = os_read_file(argv[i], &size);
      if (!json) {
         fprintf(stderr, "Usage: %s <filename.json [filename2.json ...]>\n", argv[0]);
         exit(1);
      }

      printf("parsing %s\n", argv[i]);
      cJSON *c = cJSON_ParseWithLength(json, size);
      if (!c) {
         fprintf(stderr, "Failed to parse file: %s\n", argv[i]);
         exit(1);
      }

      for (unsigned j = 0; j < ARRAY_SIZE(parsers); j++) {
         cJSON *test = cJSON_GetObjectItemCaseSensitive(c, parsers[j].name);
         if (!test)
            continue;

         printf("%s found in %s\n", parsers[j].name, argv[i]);
         util_dynarray_append(&jsons[j], cJSON*, c);
         char buf[4096];
         snprintf(buf, sizeof(buf), "%s", argv[i]);
         char *p = strrchr(buf, '.');
         assert(p);
         p[1] = 'x';
         p[2] = 0;
         FILE *f = fopen(buf, "wb");
         util_dynarray_append(&jsonfiles[j], void*, f);
      }
   }

   for (unsigned i = 0; i < ARRAY_SIZE(jsons); i++) {
      while (util_dynarray_contains(&jsons[i], cJSON*)) {
         cJSON *c = util_dynarray_pop(&jsons[i], cJSON*);
         FILE *f = util_dynarray_pop(&jsonfiles[i], FILE*);
         parsers[i].func(cJSON_GetObjectItemCaseSensitive(c, parsers[i].name), f);
         cJSON_Delete(c);
         fclose(f);
      }
   }

   for (unsigned i = 0; i < ARRAY_SIZE(jsons); i++) {
      util_dynarray_fini(&jsons[i]);
      util_dynarray_fini(&jsonfiles[i]);
   }

   return 0;
}