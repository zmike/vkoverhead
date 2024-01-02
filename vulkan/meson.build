# Copyright © 2017 Intel Corporation
# Copyright © 2022 Valve Corporation

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

vk_api_xml = files('vk.xml')

vk_device_info = custom_target(
  'vk_device_info',
  input : 'vk_device_info.py',
  output : ['vk_device_info.h', 'vk_device_info.c'],
  command : [
    prog_python, '@INPUT@', '@OUTPUT@', join_paths(meson.source_root(), 'vulkan/vk.xml')
  ]
)

vk_instance = custom_target(
  'vk_instance.c',
  input : ['vk_instance.py'],
  output : ['vk_instance.h', 'vk_instance.c'],
  command : [
    prog_python, '@INPUT@', '@OUTPUT@', join_paths(meson.source_root(), 'vulkan/vk.xml')
  ]
)

vk_extensions_depend_files = [
]
vk_entrypoints_depend_files = [
  files('vk_extensions.py'),
  vk_extensions_depend_files,
]
vk_extensions_gen_depend_files = [
  files('vk_extensions.py'),
  vk_extensions_depend_files,
]
vk_dispatch_table_gen_depend_files = [
  files('vk_entrypoints.py'),
  vk_entrypoints_depend_files,
]

vk_dispatch_table = custom_target(
  'vk_dispatch_table',
  input : ['vk_dispatch_table_gen.py', vk_api_xml],
  output : ['vk_dispatch_table.c', 'vk_dispatch_table.h'],
  command : [
    prog_python, '@INPUT0@', '--xml', '@INPUT1@',
    '--out-c', '@OUTPUT0@', '--out-h', '@OUTPUT1@',
    '--beta', 'false',
  ],
  depend_files : vk_dispatch_table_gen_depend_files,
)

vk_enum_to_str = custom_target(
  'vk_enum_to_str',
  input : ['gen_enum_to_str.py', vk_api_xml],
  output : ['vk_enum_to_str.c', 'vk_enum_to_str.h', 'vk_enum_defines.h'],
  command : [
    prog_python, '@INPUT0@', '--xml', '@INPUT1@',
    '--outdir', meson.current_build_dir(),
    '--beta', 'false',
  ],
)

vk_extensions = custom_target(
  'vk_extensions',
  input : ['vk_extensions_gen.py', vk_api_xml],
  output : ['vk_extensions.c', 'vk_extensions.h'],
  command : [
    prog_python, '@INPUT0@', '--xml', '@INPUT1@',
    '--out-c', '@OUTPUT0@', '--out-h', '@OUTPUT1@'
  ],
  depend_files : vk_extensions_gen_depend_files,
)
