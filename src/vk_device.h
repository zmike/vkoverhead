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

#ifndef VK_DEVICE_H
#define VK_DEVICE_H
#include "detect_os.h"

#if DETECT_OS_WINDOWS
#include <io.h>
#define VK_LIBNAME "vulkan-1.dll"
#else
#include <unistd.h>
#if DETECT_OS_APPLE
#define VK_LIBNAME "libvulkan.1.dylib"
// Source of MVK_VERSION
#include "MoltenVK/vk_mvk_moltenvk.h"
#else
#define VK_LIBNAME "libvulkan.so.1"
#endif
#endif

#include "u_dl.h"
#include "vk_instance.h"
#include "vk_device_info.h"
#include "vk_dispatch_table.h"
#include "macros.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vk_enum_to_str.h"

#define GET_PROC_ADDR_INSTANCE_LOCAL(screen, instance, x) PFN_vk##x vk_##x = (PFN_vk##x)(screen)->vk_GetInstanceProcAddr(instance, "vk"#x)

#define SPIRV_VERSION(major, minor) (((major) << 16) | ((minor) << 8))

struct vk_device {
   bool is_cpu;
   bool have_triangle_fans;

   struct util_dl_library *loader_lib;
   PFN_vkGetInstanceProcAddr vk_GetInstanceProcAddr;
   PFN_vkGetDeviceProcAddr vk_GetDeviceProcAddr;

   VkInstance instance;
   struct vk_instance_info instance_info;

   VkPhysicalDevice pdev;
   uint32_t vk_version, spirv_version;
   struct vk_device_info info;

   uint32_t gfx_queue;
   uint32_t sparse_queue;
   uint32_t max_queues;
   uint32_t timestamp_valid_bits;

   VkDevice dev;
   VkQueue queue;
   VkQueue queue_sparse;
   int vram_mem_idx;
   int host_mem_idx;

   VkSemaphore sem;
   uint64_t timeline;

   struct vk_dispatch_table vk;
};

struct vk_device *
vk_device_create(void);
void vk_stub_function_not_loaded(void);

#define VK(fn) dev->vk.fn

#define VK_CHECK(call, result) do { \
   if (result != VK_SUCCESS) { \
      fprintf(stderr, call " failed (%s)\n", vk_Result_to_str(result)); \
      abort(); \
   } \
} while (0)


#endif
