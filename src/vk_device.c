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

#include "vk_device.h"
#include "ralloc.h"

void
vk_device_destroy(struct vk_device *dev)
{
   if (dev->sem)
      VK(DestroySemaphore)(dev->dev, dev->sem, NULL);

   //if (dev->threaded)
      //util_queue_destroy(&dev->flush_queue);

   VK(DestroyDevice)(dev->dev, NULL);
   VK(DestroyInstance)(dev->instance, NULL);

   util_dl_close(dev->loader_lib);

   ralloc_free(dev);
}

static bool
choose_pdev(struct vk_device *dev)
{
   uint32_t i, pdev_count;
   VkPhysicalDevice *pdevs;
   bool is_cpu = false;
   VkResult result = VK(EnumeratePhysicalDevices)(dev->instance, &pdev_count, NULL);
   if (result != VK_SUCCESS) {
      fprintf(stderr, "vkEnumeratePhysicalDevices failed (%s)\n", vk_Result_to_str(result));
      return is_cpu;
   }

   assert(pdev_count > 0);

   pdevs = malloc(sizeof(*pdevs) * pdev_count);
   result = VK(EnumeratePhysicalDevices)(dev->instance, &pdev_count, pdevs);
   assert(result == VK_SUCCESS);
   assert(pdev_count > 0);

   VkPhysicalDeviceProperties props;
   /* priority when multiple drivers are available (highest to lowest):
      VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU
      VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
      VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
      VK_PHYSICAL_DEVICE_TYPE_CPU
      VK_PHYSICAL_DEVICE_TYPE_OTHER

    * users should specify VK_ICD_FILENAMES since this is a standardized variable
    * used by all vulkan applications
    */
   unsigned prio_map[] = {
      [VK_PHYSICAL_DEVICE_TYPE_OTHER] = 0,
      [VK_PHYSICAL_DEVICE_TYPE_CPU] = 1,
      [VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU] = 2,
      [VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU] = 3,
      [VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU] = 4,
   };
   unsigned idx = 0;
   int cur_prio = 0;
   for (i = 0; i < pdev_count; ++i) {
      VK(GetPhysicalDeviceProperties)(pdevs[i], &props);

      assert(props.deviceType <= VK_PHYSICAL_DEVICE_TYPE_CPU);
      if (prio_map[props.deviceType] > cur_prio) {
         idx = i;
         cur_prio = prio_map[props.deviceType];
      }
   }
   is_cpu = cur_prio == prio_map[VK_PHYSICAL_DEVICE_TYPE_CPU];

   dev->pdev = pdevs[idx];
   VK(GetPhysicalDeviceProperties)(dev->pdev, &dev->info.props);
   dev->info.device_version = dev->info.props.apiVersion;

   /* runtime version is the lesser of the instance version and device version */
   dev->vk_version = MIN2(dev->info.device_version, dev->instance_info.loader_version);

   /* calculate SPIR-V version based on VK version */
   if (dev->vk_version >= VK_MAKE_VERSION(1, 2, 0))
      dev->spirv_version = SPIRV_VERSION(1, 5);
   else if (dev->vk_version >= VK_MAKE_VERSION(1, 1, 0))
      dev->spirv_version = SPIRV_VERSION(1, 3);
   else
      dev->spirv_version = SPIRV_VERSION(1, 0);

   free(pdevs);
   return is_cpu;
}

static void
update_queue_props(struct vk_device *dev)
{
   uint32_t num_queues;
   VK(GetPhysicalDeviceQueueFamilyProperties)(dev->pdev, &num_queues, NULL);
   assert(num_queues > 0);

   VkQueueFamilyProperties *props = malloc(sizeof(*props) * num_queues);
   VK(GetPhysicalDeviceQueueFamilyProperties)(dev->pdev, &num_queues, props);

   bool found_gfx = false;
   uint32_t sparse_only = UINT32_MAX;
   dev->sparse_queue = UINT32_MAX;
   for (uint32_t i = 0; i < num_queues; i++) {
      if (!found_gfx && (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
         dev->gfx_queue = i;
         dev->max_queues = props[i].queueCount;
         dev->timestamp_valid_bits = props[i].timestampValidBits;
         found_gfx = true;
         if (props[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
            dev->sparse_queue = i;
      } else if (props[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
         sparse_only = i;
   }
   if (dev->sparse_queue == UINT32_MAX)
      dev->sparse_queue = sparse_only;
   free(props);
}

static bool
vk_internal_setup_moltenvk(struct vk_device *dev)
{
#if defined(MVK_VERSION)
   if (!dev->instance_info.have_MVK_moltenvk)
      return true;

   GET_PROC_ADDR_INSTANCE_LOCAL(dev, dev->instance, GetMoltenVKConfigurationMVK);
   GET_PROC_ADDR_INSTANCE_LOCAL(dev, dev->instance, SetMoltenVKConfigurationMVK);
   GET_PROC_ADDR_INSTANCE_LOCAL(dev, dev->instance, GetVersionStringsMVK);

   if (vk_GetVersionStringsMVK) {
      char molten_version[64] = {0};
      char vulkan_version[64] = {0};

      vk_GetVersionStringsMVK(molten_version, sizeof(molten_version) - 1, vulkan_version, sizeof(vulkan_version) - 1);

      //printf("MoltenVK %s Vulkan %s \n", molten_version, vulkan_version);
   }

   if (vk_GetMoltenVKConfigurationMVK && vk_SetMoltenVKConfigurationMVK) {
      MVKConfiguration molten_config = {0};
      size_t molten_config_size = sizeof(molten_config);

      VkResult res = vk_GetMoltenVKConfigurationMVK(dev->instance, &molten_config, &molten_config_size);
      if (res == VK_SUCCESS || res == VK_INCOMPLETE) {
         // Needed to allow MoltenVK to accept VkImageView swizzles.
         // Encountered when using VK_FORMAT_R8G8_UNORM
         molten_config.fullImageViewSwizzle = VK_TRUE;
         vk_SetMoltenVKConfigurationMVK(dev->instance, &molten_config, &molten_config_size);
      }
   }
#endif // MVK_VERSION

   return true;
}

static void
init_queue(struct vk_device *dev)
{
   VK(GetDeviceQueue)(dev->dev, dev->gfx_queue, 0, &dev->queue);
   if (dev->sparse_queue != UINT32_MAX) {
      if (dev->sparse_queue != dev->gfx_queue)
         VK(GetDeviceQueue)(dev->dev, dev->sparse_queue, 0, &dev->queue_sparse);
      else
         dev->queue_sparse = dev->queue;
   }
}

static void
init_semaphore(struct vk_device *dev)
{
   VkSemaphoreCreateInfo sci = {0};
   VkSemaphoreTypeCreateInfo tci = {0};
   sci.pNext = &tci;
   sci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
   tci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
   tci.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;

   VkResult result = VK(CreateSemaphore)(dev->dev, &sci, NULL, &dev->sem);
   VK_CHECK("CreateSemaphore", result);
}

static uint32_t
get_loader_version(struct vk_device *dev)
{

   uint32_t loader_version = VK_API_VERSION_1_0;

   // Get the Loader version
   GET_PROC_ADDR_INSTANCE_LOCAL(dev, NULL, EnumerateInstanceVersion);
   if (vk_EnumerateInstanceVersion) {
      uint32_t loader_version_temp = VK_API_VERSION_1_0;
      VkResult result = (*vk_EnumerateInstanceVersion)(&loader_version_temp);
      if (VK_SUCCESS == result) {
         loader_version = loader_version_temp;
      } else {
         fprintf(stderr, "vkEnumerateInstanceVersion failed (%s)\n", vk_Result_to_str(result));
      }
   }

   return loader_version;
}

static VkDevice
vk_create_logical_device(struct vk_device *dev)
{
   VkDevice device = VK_NULL_HANDLE;

   VkDeviceQueueCreateInfo qci = {0};
   float dummy = 0.0f;
   qci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
   qci.queueFamilyIndex = dev->gfx_queue;
   qci.queueCount = 1;
   qci.pQueuePriorities = &dummy;

   VkDeviceCreateInfo dci = {0};
   dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
   dci.queueCreateInfoCount = 1;
   dci.pQueueCreateInfos = &qci;
   /* extensions don't have bool members in pEnabledFeatures.
    * this requires us to pass the whole VkPhysicalDeviceFeatures2 struct
    */
   if (dev->info.feats.sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2) {
      dci.pNext = &dev->info.feats;
   } else {
      dci.pEnabledFeatures = &dev->info.feats.features;
   }

   dci.ppEnabledExtensionNames = dev->info.extensions;
   dci.enabledExtensionCount = dev->info.num_extensions;

   VkResult result = VK(CreateDevice)(dev->pdev, &dci, NULL, &device);
   if (result != VK_SUCCESS)
      fprintf(stderr, "vkCreateDevice failed (%s)\n", vk_Result_to_str(result));
   
   return device;
}

struct vk_device *
vk_device_create(void)
{
   struct vk_device *dev = rzalloc(NULL, struct vk_device);
   if (!dev)
      abort();

   dev->loader_lib = util_dl_open(VK_LIBNAME);
   if (!dev->loader_lib) {
      fprintf(stderr, "failed to open '" VK_LIBNAME "\n");
      abort();
   }

   dev->vk_GetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)util_dl_get_proc_address(dev->loader_lib, "vkGetInstanceProcAddr");
   dev->vk_GetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)util_dl_get_proc_address(dev->loader_lib, "vkGetDeviceProcAddr");
   if (!dev->vk_GetInstanceProcAddr ||
       !dev->vk_GetDeviceProcAddr)
      abort();

   dev->instance_info.loader_version = get_loader_version(dev);

   if (!vk_create_instance(dev))
      abort();

   vk_instance_dispatch_table_load(&dev->vk.instance,
                                   dev->vk_GetInstanceProcAddr,
                                   dev->instance);
   vk_physical_device_dispatch_table_load(&dev->vk.physical_device,
                                          dev->vk_GetInstanceProcAddr,
                                          dev->instance);

   vk_verify_instance_extensions(dev);

   dev->is_cpu = choose_pdev(dev);
   if (dev->pdev == VK_NULL_HANDLE)
      abort();

   update_queue_props(dev);

   if (!vk_get_physical_device_info(dev)) {
      fprintf(stderr, "failed to detect features\n");
      abort();
   }

   //if (dev->threaded && !util_queue_init(&dev->flush_queue, "flush", 8, 1, UTIL_QUEUE_INIT_RESIZE_IF_FULL, dev)) {
      //fprintf(stderr, "Failed to create flush queue.\n");
      //abort();
   //}

   vk_internal_setup_moltenvk(dev);
   if (!dev->info.have_KHR_timeline_semaphore && !dev->info.feats12.timelineSemaphore) {
      fprintf(stderr, "KHR_timeline_semaphore is required\n");
      abort();
   }

   dev->dev = vk_create_logical_device(dev);
   if (!dev->dev)
      abort();

   vk_device_dispatch_table_load(&dev->vk.device,
                                 dev->vk_GetDeviceProcAddr,
                                 dev->dev);

   init_queue(dev);

   vk_verify_device_extensions(dev);

   dev->have_triangle_fans = true;
#if defined(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME)
   if (dev->info.have_KHR_portability_subset) {
      dev->have_triangle_fans = (VK_TRUE == dev->info.portability_subset_feats.triangleFans);
   }
#endif // VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME

   init_semaphore(dev);
   dev->host_mem_idx = -1;
   dev->vram_mem_idx = -1;
   VkMemoryPropertyFlags host_vis = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
   for (unsigned i = 0; i < dev->info.mem_props.memoryTypeCount; i++) {
      if (dev->host_mem_idx == -1 &&
          ((dev->info.mem_props.memoryTypes[i].propertyFlags & host_vis) == host_vis))
         dev->host_mem_idx = i;
      if (dev->vram_mem_idx == -1 &&
               (dev->info.mem_props.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
         dev->vram_mem_idx = i;
   }
   if (dev->host_mem_idx == -1) {
      fprintf(stderr, "Couldn't find host mem heap!\n");
      abort();
   }
   if (dev->vram_mem_idx == -1) {
      fprintf(stderr, "Couldn't find device-local mem heap!\n");
      abort();
   }

   return dev;
}

void vk_stub_function_not_loaded(void)
{
   /* this will be used by the vk_verify_*_extensions() functions on a
    * release build
    */
   fprintf(stderr, "A Vulkan function was called without being loaded\n");
   abort();
}

