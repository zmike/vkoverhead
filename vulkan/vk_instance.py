# Copyright © 2020 Hoe Hao Cheng
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice (including the next
# paragraph) shall be included in all copies or substantial portions of the
# Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
# 
# Authors:
#    Hoe Hao Cheng <haochengho12907@gmail.com>
#

from mako.template import Template
from os import path
from xml.etree import ElementTree
from vk_device_extensions import Extension,Layer,ExtensionRegistry,Version
import sys
import platform

# constructor: Extension(name, conditions=[], nonstandard=False)
# The attributes:
#  - conditions: If the extension is provided by the Vulkan implementation, then
#                these are the extra conditions needed to enable the extension.
#  - nonstandard: Disables validation (cross-checking with vk.xml) if True.
EXTENSIONS = [
    Extension("VK_EXT_debug_utils"),
    Extension("VK_KHR_get_physical_device_properties2"),
    Extension("VK_KHR_external_memory_capabilities"),
    Extension("VK_KHR_external_semaphore_capabilities"),
    Extension("VK_MVK_moltenvk",
        nonstandard=True),
    Extension("VK_KHR_surface"),
    Extension("VK_EXT_headless_surface"),
    Extension("VK_KHR_wayland_surface"),
    Extension("VK_KHR_xcb_surface"),
    Extension("VK_KHR_win32_surface"),
    Extension("VK_KHR_android_surface"),
]

if platform.system() == "Darwin":
    EXTENSIONS += [
        Extension("VK_KHR_portability_enumeration"),
    ]

# constructor: Layer(name, conditions=[])
# - conditions: See documentation of EXTENSIONS.
LAYERS = [
]

REPLACEMENTS = {
    "VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES2_EXTENSION_NAME" : "VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME"
}

header_code = """
#ifndef VK_INSTANCE_H
#define VK_INSTANCE_H

#include <stdbool.h>
#include "util/u_process.h"

#include <vulkan/vulkan_core.h>

#ifdef __APPLE__
#include "MoltenVK/mvk_vulkan.h"
// Source of MVK_VERSION
#include "MoltenVK/mvk_config.h"
#endif /* __APPLE__ */

struct pipe_dev;
struct vk_device;

struct vk_instance_info {
   uint32_t loader_version;

%for ext in extensions:
   bool have_${ext.name_with_vendor()};
%endfor

%for layer in layers:
   bool have_layer_${layer.pure_name()};
%endfor
};

bool
vk_create_instance(struct vk_device *dev);

void
vk_verify_instance_extensions(struct vk_device *dev);

/* stub functions that get inserted into the dispatch table if they are not
 * properly loaded.
 */
%for ext in extensions:
   %if registry.in_registry(ext.name):
      %for cmd in registry.get_registry_entry(ext.name).instance_commands:
         void VKAPI_PTR vk_stub_${cmd.name()}(void);
      %endfor
      %for cmd in registry.get_registry_entry(ext.name).pdevice_commands:
         void VKAPI_PTR vk_stub_${cmd.name()}(void);
      %endfor
   %endif
%endfor

struct pipe_dev;
struct pipe_resource;

#endif
"""

impl_code = """
#include "vk_enum_to_str.h"
#include "vk_instance.h"
#include "vk_device.h"

bool
vk_create_instance(struct vk_device *dev)
{
   struct vk_instance_info *instance_info = &dev->instance_info;

   /* reserve one slot for MoltenVK */
   const char *layers[${len(layers) + 1}] = {0};
   uint32_t num_layers = 0;
   
   const char *extensions[${len(extensions) + 1}] = {0};
   uint32_t num_extensions = 0;

%for ext in extensions:
   bool have_${ext.name_with_vendor()} = false;
%endfor

%for layer in layers:
   bool have_layer_${layer.pure_name()} = false;
%endfor

#if defined(MVK_VERSION)
   bool have_moltenvk_layer = false;
#endif

   GET_PROC_ADDR_INSTANCE_LOCAL(dev, NULL, EnumerateInstanceExtensionProperties);
   GET_PROC_ADDR_INSTANCE_LOCAL(dev, NULL, EnumerateInstanceLayerProperties);
   if (!vk_EnumerateInstanceExtensionProperties ||
       !vk_EnumerateInstanceLayerProperties)
      return NULL;

   // Build up the extensions from the reported ones but only for the unnamed layer
   uint32_t extension_count = 0;
   if (vk_EnumerateInstanceExtensionProperties(NULL, &extension_count, NULL) != VK_SUCCESS) {
      fprintf(stderr, "VK: vkEnumerateInstanceExtensionProperties failed\\n");
   } else {
       VkExtensionProperties *extension_props = malloc(extension_count * sizeof(VkExtensionProperties));
       if (extension_props) {
           if (vk_EnumerateInstanceExtensionProperties(NULL, &extension_count, extension_props) != VK_SUCCESS) {
               fprintf(stderr, "VK: vkEnumerateInstanceExtensionProperties failed\\n");
           } else {
              for (uint32_t i = 0; i < extension_count; i++) {
        %for ext in extensions:
                if (!strcmp(extension_props[i].extensionName, ${ext.extension_name_literal()})) {
                    have_${ext.name_with_vendor()} = true;
                }
        %endfor
              }
           }
       free(extension_props);
       }
   }

    // Build up the layers from the reported ones
    uint32_t layer_count = 0;

    if (vk_EnumerateInstanceLayerProperties(&layer_count, NULL) != VK_SUCCESS) {
      fprintf(stderr, "VK: vkEnumerateInstanceLayerProperties failed\\n");
    } else {
        VkLayerProperties *layer_props = malloc(layer_count * sizeof(VkLayerProperties));
        if (layer_props) {
            if (vk_EnumerateInstanceLayerProperties(&layer_count, layer_props) != VK_SUCCESS) {
               fprintf(stderr, "VK: vkEnumerateInstanceLayerProperties failed\\n");
            } else {
               for (uint32_t i = 0; i < layer_count; i++) {
%for layer in layers:
                  if (!strcmp(layer_props[i].layerName, ${layer.extension_name_literal()})) {
                     have_layer_${layer.pure_name()} = true;
                  }
%endfor
#if defined(MVK_VERSION)
                  if (!strcmp(layer_props[i].layerName, "MoltenVK")) {
                     have_moltenvk_layer = true;
                     layers[num_layers++] = "MoltenVK";
                  }
#endif
               }
            }
        free(layer_props);
        }
    }

%for ext in extensions:
<%
    conditions = ""
    if ext.enable_conds:
        for cond in ext.enable_conds:
            conditions += "&& (" + cond + ") "
    conditions = conditions.strip()
%>\
   if (have_${ext.name_with_vendor()} ${conditions}) {
      instance_info->have_${ext.name_with_vendor()} = have_${ext.name_with_vendor()};
      extensions[num_extensions++] = ${ext.extension_name_literal()};
   }
%endfor

%for layer in layers:
<%
    conditions = ""
    if layer.enable_conds:
        for cond in layer.enable_conds:
            conditions += "&& (" + cond + ") "
    conditions = conditions.strip()
%>\
   if (have_layer_${layer.pure_name()} ${conditions}) {
      layers[num_layers++] = ${layer.extension_name_literal()};
      instance_info->have_layer_${layer.pure_name()} = true;
   }
%endfor

   VkApplicationInfo ai = {0};
   ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

   ai.pApplicationName = "vkoverhead";
   ai.pEngineName = "vkoverhead";
   ai.apiVersion = instance_info->loader_version;

   VkInstanceCreateInfo ici = {0};
   ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
#ifdef __APPLE__
   ici.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
   ici.pApplicationInfo = &ai;
   ici.ppEnabledExtensionNames = extensions;
   ici.enabledExtensionCount = num_extensions;
   ici.ppEnabledLayerNames = layers;
   ici.enabledLayerCount = num_layers;

   GET_PROC_ADDR_INSTANCE_LOCAL(dev, NULL, CreateInstance);
   assert(vk_CreateInstance);

   VkResult err = vk_CreateInstance(&ici, NULL, &dev->instance);
   if (err != VK_SUCCESS) {
      fprintf(stderr, "VK: vkCreateInstance failed (%s)\\n", vk_Result_to_str(err));

      return false;
   } else {
      return true;
   }
}

void
vk_verify_instance_extensions(struct vk_device *dev)
{
%for ext in extensions:
   %if registry.in_registry(ext.name):
      %if ext.platform_guard:
      #ifdef ${ext.platform_guard}
      %endif
         if (dev->instance_info.have_${ext.name_with_vendor()}) {
      %for cmd in registry.get_registry_entry(ext.name).instance_commands:
            if (!dev->vk.${cmd.name()}) {
      #ifndef NDEBUG
               dev->vk.${cmd.name()} = (PFN_${cmd.full_name})vk_stub_${cmd.name()};
      #else
               dev->vk.${cmd.name()} = (PFN_${cmd.full_name})vk_stub_function_not_loaded;
      #endif
            }
      %endfor
      %for cmd in registry.get_registry_entry(ext.name).pdevice_commands:
            if (!dev->vk.${cmd.name()}) {
      #ifndef NDEBUG
               dev->vk.${cmd.name()} = (PFN_${cmd.full_name})vk_stub_${cmd.name()};
      #else
               dev->vk.${cmd.name()} = (PFN_${cmd.full_name})vk_stub_function_not_loaded;
      #endif
            }
      %endfor
         }
      %if ext.platform_guard:
         #endif
      %endif
   %endif
%endfor
}

#ifndef NDEBUG
/* generated stub functions */
## see vk_device_info.py for why this is needed
<% generated_funcs = set() %>

%for ext in extensions:
   %if registry.in_registry(ext.name):
      %for cmd in registry.get_registry_entry(ext.name).instance_commands + registry.get_registry_entry(ext.name).pdevice_commands:
         %if cmd.name in generated_funcs:
            <% continue %>
         %else:
            <% generated_funcs.add(cmd.full_name) %>
         %endif
         %if ext.platform_guard:
         #ifdef ${ext.platform_guard}
         %endif
            void VKAPI_PTR
            vk_stub_${cmd.name()}()
            {
               fprintf(stderr, "VK: ${cmd.full_name} is not loaded properly!\\n");
               abort();
            }
         %if ext.platform_guard:
         #endif
         %endif
      %endfor
   %endif
%endfor

#endif
"""


def replace_code(code: str, replacement: dict):
    for (k, v) in replacement.items():
        code = code.replace(k, v)
    
    return code


if __name__ == "__main__":
    try:
        header_path = sys.argv[1]
        impl_path = sys.argv[2]
        vkxml_path = sys.argv[3]

        header_path = path.abspath(header_path)
        impl_path = path.abspath(impl_path)
        vkxml_path = path.abspath(vkxml_path)
    except:
        print("usage: %s <path to .h> <path to .c> <path to vk.xml>" % sys.argv[0])
        exit(1)

    registry = ExtensionRegistry(vkxml_path)

    extensions = EXTENSIONS
    layers = LAYERS
    replacement = REPLACEMENTS

    # Perform extension validation and set core_since for the extension if available
    error_count = 0
    for ext in extensions:
        if not registry.in_registry(ext.name):
            # disable validation for nonstandard extensions
            if ext.is_nonstandard:
                continue

            error_count += 1
            print("The extension {} is not registered in vk.xml - a typo?".format(ext.name))
            continue
        
        entry = registry.get_registry_entry(ext.name)

        if entry.ext_type != "instance":
            error_count += 1
            print("The extension {} is {} extension - expected an instance extension.".format(ext.name, entry.ext_type))
            continue

        if entry.promoted_in:
            ext.core_since = Version((*entry.promoted_in, 0))

        if entry.platform_guard:
            ext.platform_guard = entry.platform_guard 

    if error_count > 0:
        print("vk_instance.py: Found {} error(s) in total. Quitting.".format(error_count))
        exit(1)

    with open(header_path, "w", encoding='utf-8') as header_file:
        header = Template(header_code).render(extensions=extensions, layers=layers, registry=registry).strip()
        header = replace_code(header, replacement)
        print(header, file=header_file)

    with open(impl_path, "w", encoding='utf-8') as impl_file:
        impl = Template(impl_code).render(extensions=extensions, layers=layers, registry=registry).strip()
        impl = replace_code(impl, replacement)
        print(impl, file=impl_file)
