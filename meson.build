# Copyright © 2017-2020 Intel Corporation
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

project(
  'vkoverhead',
  ['c', 'cpp'],
  version : '1.0',
  license : 'MIT',
  meson_version : '>= 0.53',
  default_options : ['buildtype=debugoptimized', 'b_ndebug=if-release', 'c_std=c11', 'cpp_std=c++14']
)

cc = meson.get_compiler('c')
cpp = meson.get_compiler('cpp')

null_dep = dependency('', required : false)


prog_python = import('python').find_installation('python3')

# Arguments for the preprocessor, put these in a separate array from the C and
# C++ (cpp in meson terminology) arguments since they need to be added to the
# default arguments for both C and C++.
pre_args = [
  '-D__STDC_CONSTANT_MACROS',
  '-D__STDC_FORMAT_MACROS',
  '-D__STDC_LIMIT_MACROS',
  '-DPACKAGE_VERSION="@0@"'.format(meson.project_version()),
  '-DPACKAGE_BUGREPORT="https://gitlab.freedesktop.org/mesa/mesa/-/issues"',
]

_platforms = get_option('platforms')
if _platforms.contains('auto')
  if ['darwin', 'cygwin'].contains(host_machine.system())
    _platforms = ['x11']
  elif ['haiku'].contains(host_machine.system())
    _platforms = ['haiku']
  elif host_machine.system() == 'windows'
    _platforms = ['windows']
  endif
endif

with_platform_android = _platforms.contains('android')
with_platform_haiku = _platforms.contains('haiku')
with_platform_windows = _platforms.contains('windows')

c_args = []
cpp_args = []

if with_platform_windows
  c_args += '-DVK_USE_PLATFORM_WIN32_KHR'
  #add this once aco and other places can build with it
  #cpp_args += '-DVK_USE_PLATFORM_WIN32_KHR'
endif
if with_platform_android
  c_args += '-DVK_USE_PLATFORM_ANDROID_KHR'
  cpp_args += '-DVK_USE_PLATFORM_ANDROID_KHR'
endif

# Check for GCC style builtins
foreach b : ['bswap32', 'bswap64', 'clz', 'clzll', 'ctz', 'expect', 'ffs',
             'ffsll', 'popcount', 'popcountll', 'unreachable', 'types_compatible_p']
  if cc.has_function(b)
    pre_args += '-DHAVE___BUILTIN_@0@'.format(b.to_upper())
  endif
endforeach

# check for GCC __attribute__
_attributes = [
  'const', 'flatten', 'malloc', 'pure', 'unused', 'warn_unused_result',
  'weak', 'format', 'packed', 'returns_nonnull', 'alias', 'noreturn',
]
foreach a : cc.get_supported_function_attributes(_attributes)
  pre_args += '-DHAVE_FUNC_ATTRIBUTE_@0@'.format(a.to_upper())
endforeach
if cc.has_function_attribute('visibility:hidden')
  pre_args += '-DHAVE_FUNC_ATTRIBUTE_VISIBILITY'
endif
if cc.compiles('__uint128_t foo(void) { return 0; }',
               name : '__uint128_t')
  pre_args += '-DHAVE_UINT128'
endif

if cc.has_function('reallocarray')
   pre_args += '-DHAVE_REALLOCARRAY'
endif

if ['linux', 'cygwin', 'gnu', 'freebsd', 'gnu/kfreebsd', 'haiku', 'android'].contains(host_machine.system())
  pre_args += '-D_GNU_SOURCE'
elif host_machine.system() == 'sunos'
  pre_args += '-D__EXTENSIONS__'
elif host_machine.system() == 'windows'
  pre_args += [
    '-D_WINDOWS', '-D_WIN32_WINNT=0x0A00', '-DWINVER=0x0A00',
    '-DPIPE_SUBSYSTEM_WINDOWS_USER',
    '-D_USE_MATH_DEFINES',  # XXX: scons didn't use this for mingw
  ]
  if cc.get_argument_syntax() == 'msvc'
    pre_args += [
      '-DVC_EXTRALEAN',
      '-D_CRT_SECURE_NO_WARNINGS',
      '-D_CRT_SECURE_NO_DEPRECATE',
      '-D_SCL_SECURE_NO_WARNINGS',
      '-D_SCL_SECURE_NO_DEPRECATE',
      '-D_ALLOW_KEYWORD_MACROS',
      '-D_HAS_EXCEPTIONS=0', # Tell C++ STL to not use exceptions
      '-DNOMINMAX',
    ]
  else
    # When the target is not mingw/ucrt
    # NOTE: clang's stddef.h are conflict with mingw/ucrt's stddef.h
    # So do not include headers that defined in clang for detecting
    # _UCRT
    if cc.compiles('''
      #include <string.h>
      #if defined(__MINGW32__) && defined(_UCRT)
      #error
      #endif
      int main(void) { return 0; }''')
      pre_args += ['-D__MSVCRT_VERSION__=0x0700']
    endif
  endif
elif host_machine.system() == 'openbsd'
  pre_args += '-D_ISOC11_SOURCE'
endif

if host_machine.system() == 'windows'
  if cc.get_argument_syntax() == 'msvc'
    add_project_link_arguments(
      '/fixed:no',
      '/dynamicbase',
      '/nxcompat',
      language : ['c', 'cpp'],
    )
    if get_option('buildtype') != 'debug'
      add_project_link_arguments(
        '/incremental:no',
        language : ['c', 'cpp'],
      )
    endif
  else
    add_project_link_arguments(
      cc.get_supported_link_arguments(
        '-Wl,--nxcompat',
        '-Wl,--dynamicbase',
        '-static-libgcc',
        '-static-libstdc++',
      ),
      language : ['c'],
    )
    add_project_link_arguments(
      cpp.get_supported_link_arguments(
        '-Wl,--nxcompat',
        '-Wl,--dynamicbase',
        '-static-libgcc',
        '-static-libstdc++',
      ),
      language : ['cpp'],
    )
  endif
endif

# Check for generic C arguments
c_msvc_compat_args = []
no_override_init_args = []
cpp_msvc_compat_args = []
if cc.get_argument_syntax() == 'msvc'
  _trial = [
    '/wd4018',  # signed/unsigned mismatch
    '/wd4056',  # overflow in floating-point constant arithmetic
    '/wd4244',  # conversion from 'type1' to 'type2', possible loss of data
    '/wd4267',  # 'var' : conversion from 'size_t' to 'type', possible loss of data
    '/wd4305',  # trancation from 'type1' to 'type2'
    '/wd4351',  # new behavior: elements of array 'array' will be default initialized
    '/wd4756',  # overflow in constant arithmetic
    '/wd4800',  # forcing value to bool 'true' or 'false' (performance warning)
    '/wd4996',  # disabled deprecated POSIX name warnings
    '/wd4291',  # no matching operator delete found
    '/wd4146',  # unary minus operator applied to unsigned type, result still unsigned
    '/wd4200',  # nonstandard extension used: zero-sized array in struct/union
    '/wd4624',  # destructor was implicitly defined as deleted [from LLVM]
    '/wd4309',  # 'initializing': truncation of constant value
    '/wd4838',  # conversion from 'int' to 'const char' requires a narrowing conversion
    '/wd5105',  # macro expansion producing 'defined' has undefined behavior (winbase.h, need Windows SDK upgrade)
    '/we4020',  # Error when passing the wrong number of parameters
    '/we4024',  # Error when passing different type of parameter
    '/Zc:__cplusplus', #Set __cplusplus macro to match the /std:c++<version> on the command line
  ]
  c_args += cc.get_supported_arguments(_trial)
  cpp_args += cpp.get_supported_arguments(_trial)
else
  _trial_c = [
    '-Werror=implicit-function-declaration',
    '-Werror=return-type',
    '-Werror=empty-body',
    '-Werror=incompatible-pointer-types',
    '-Werror=int-conversion',
    '-Wimplicit-fallthrough',
    '-Wno-missing-field-initializers',
    '-Wno-format-truncation',
    '-fno-math-errno',
    '-fno-trapping-math',
    '-Qunused-arguments',
    '-fno-common',
    # Clang
    '-Wno-microsoft-enum-value',
    '-Wno-unused-function',
  ]
  _trial_cpp = [
    '-Werror=return-type',
    '-Werror=empty-body',
    '-Wno-non-virtual-dtor',
    '-Wno-missing-field-initializers',
    '-Wno-format-truncation',
    '-fno-math-errno',
    '-fno-trapping-math',
    '-Qunused-arguments',
    # Some classes use custom new operator which zeroes memory, however
    # gcc does aggressive dead-store elimination which threats all writes
    # to the memory before the constructor as "dead stores".
    # For now we disable this optimization.
    '-flifetime-dse=1',
    # Clang
    '-Wno-microsoft-enum-value',
  ]

  # MinGW chokes on format specifiers and I can't get it all working
  if not (cc.get_argument_syntax() == 'gcc' and host_machine.system() == 'windows')
    _trial_c += ['-Werror=format', '-Wformat-security']
    _trial_cpp += ['-Werror=format', '-Wformat-security']
  endif

  # FreeBSD annotated <pthread.h> but Mesa isn't ready
  if not (cc.get_id() == 'clang' and host_machine.system() == 'freebsd')
    _trial_c += ['-Werror=thread-safety']
  endif

  # If the compiler supports it, put function and data symbols in their
  # own sections and GC the sections after linking.  This lets drivers
  # drop shared code unused by that specific driver (particularly
  # relevant for Vulkan drivers).
  if cc.has_link_argument('-Wl,--gc-sections')
    _trial_c += ['-ffunction-sections', '-fdata-sections']
    _trial_cpp += ['-ffunction-sections', '-fdata-sections']
  endif

  # Variables that are only used for assertions are considered unused in release
  # builds. Don't treat this as an error, since we build with -Werror even for
  # release in CI.
  if get_option('buildtype') == 'release'
    _trial_c += ['-Wno-unused-variable', '-Wno-unused-but-set-variable']
    _trial_cpp += ['-Wno-unused-variable', '-Wno-unused-but-set-variable']
  endif

  c_args += cc.get_supported_arguments(_trial_c)
  cpp_args += cpp.get_supported_arguments(_trial_cpp)

  no_override_init_args += cc.get_supported_arguments(
    ['-Wno-override-init', '-Wno-initializer-overrides']
  )

  # Check for C and C++ arguments for MSVC compatibility. These are only used
  # in parts of the mesa code base that need to compile with MSVC, mainly
  # common code
  _trial_msvc = ['-Werror=pointer-arith', '-Werror=vla', '-Werror=gnu-empty-initializer']
  c_msvc_compat_args += cc.get_supported_arguments(_trial_msvc)
  cpp_msvc_compat_args += cpp.get_supported_arguments(_trial_msvc)
endif

# set linker arguments
if host_machine.system() == 'windows'
  if cc.get_argument_syntax() == 'msvc'
    add_project_link_arguments(
      '/fixed:no',
      '/dynamicbase',
      '/nxcompat',
      language : ['c', 'cpp'],
    )
    if get_option('buildtype') != 'debug'
      add_project_link_arguments(
        '/incremental:no',
        language : ['c', 'cpp'],
      )
    endif
  else
    add_project_link_arguments(
      cc.get_supported_link_arguments(
        '-Wl,--nxcompat',
        '-Wl,--dynamicbase',
        '-static-libgcc',
        '-static-libstdc++',
      ),
      language : ['c'],
    )
    add_project_link_arguments(
      cpp.get_supported_link_arguments(
        '-Wl,--nxcompat',
        '-Wl,--dynamicbase',
        '-static-libgcc',
        '-static-libstdc++',
      ),
      language : ['cpp'],
    )
  endif
endif

if host_machine.cpu_family().startswith('x86') and cc.get_argument_syntax() != 'msvc'
  pre_args += '-DUSE_SSE41'
  with_sse41 = true
  sse41_args = ['-msse4.1']

  if host_machine.cpu_family() == 'x86'
    if get_option('sse2')
      # These settings make generated GCC code match MSVC and follow
      # GCC advice on https://gcc.gnu.org/wiki/FloatingPointMath#x86note
      #
      # NOTE: We need to ensure stack is realigned given that we
      # produce shared objects, and have no control over the stack
      # alignment policy of the application. Therefore we need
      # -mstackrealign or -mincoming-stack-boundary=2.
      #
      # XXX: We could have SSE without -mstackrealign if we always used
      # __attribute__((force_align_arg_pointer)), but that's not
      # always the case.
      c_args += ['-msse2', '-mfpmath=sse', '-mstackrealign']
    else
      # GCC on x86 (not x86_64) with -msse* assumes a 16 byte aligned stack, but
      # that's not guaranteed
      sse41_args += '-mstackrealign'
    endif
  endif
else
  with_sse41 = false
  sse41_args = []
endif

# Check for GCC style atomics
dep_atomic = null_dep

if cc.compiles('''#include <stdint.h>
                  int main() {
                    struct {
                      uint64_t *v;
                    } x;
                    return (int)__atomic_load_n(x.v, __ATOMIC_ACQUIRE) &
                           (int)__atomic_add_fetch(x.v, (uint64_t)1, __ATOMIC_ACQ_REL);

                  }''',
               name : 'GCC atomic builtins')
  pre_args += '-DUSE_GCC_ATOMIC_BUILTINS'

  # Not all atomic calls can be turned into lock-free instructions, in which
  # GCC will make calls into the libatomic library. Check whether we need to
  # link with -latomic.
  #
  # This can happen for 64-bit atomic operations on 32-bit architectures such
  # as ARM.
  if not cc.links('''#include <stdint.h>
                     int main() {
                       struct {
                         uint64_t *v;
                       } x;
                       return (int)__atomic_load_n(x.v, __ATOMIC_ACQUIRE) &
                              (int)__atomic_add_fetch(x.v, (uint64_t)1, __ATOMIC_ACQ_REL);
                     }''',
                  name : 'GCC atomic builtins required -latomic')
    dep_atomic = cc.find_library('atomic')
  endif
endif
if not cc.links('''#include <stdint.h>
                   uint64_t v;
                   int main() {
                     return __sync_add_and_fetch(&v, (uint64_t)1);
                   }''',
                dependencies : dep_atomic,
                name : 'GCC 64bit atomics')
  pre_args += '-DMISSING_64BIT_ATOMICS'
endif

dep_ws2_32 = cc.find_library('ws2_32', required : with_platform_windows)

if with_platform_android and get_option('platform-sdk-version') >= 29
  # By default the NDK compiler, at least, emits emutls references instead of
  # ELF TLS, even when building targeting newer API levels.  Make it actually do
  # ELF TLS instead.
  c_args += '-fno-emulated-tls'
  cpp_args += '-fno-emulated-tls'
endif


# -mtls-dialect=gnu2 speeds up non-initial-exec TLS significantly but requires
# full toolchain (including libc) support.
have_mtls_dialect = false
foreach c_arg : get_option('c_args')
  if c_arg.startswith('-mtls-dialect=')
    have_mtls_dialect = true
    break
  endif
endforeach
if not have_mtls_dialect
  # need .run to check libc support. meson aborts when calling .run when
  # cross-compiling, but because this is just an optimization we can skip it
  if meson.is_cross_build() and not meson.has_exe_wrapper()
    warning('cannot auto-detect -mtls-dialect when cross-compiling, using compiler default')
  else
    # -fpic to force dynamic tls, otherwise TLS relaxation defeats check
    gnu2_test = cc.run('int __thread x; int main() { return x; }',
                       args: ['-mtls-dialect=gnu2', '-fpic'],
                       name: '-mtls-dialect=gnu2')
    if gnu2_test.returncode() == 0 and (
          # check for lld 13 bug: https://gitlab.freedesktop.org/mesa/mesa/-/issues/5665
          host_machine.cpu_family() != 'x86_64' or
          # get_linker_id misses LDFLAGS=-fuse-ld=lld: https://github.com/mesonbuild/meson/issues/6377
          #cc.get_linker_id() != 'ld.lld' or
          cc.links('''int __thread x; int y; int main() { __asm__(
                "leaq x@TLSDESC(%rip), %rax\n"
                "movq y@GOTPCREL(%rip), %rdx\n"
                "call *x@TLSCALL(%rax)\n"); }''', name: 'split TLSDESC')
          )
      c_args += '-mtls-dialect=gnu2'
      cpp_args += '-mtls-dialect=gnu2'
    endif
  endif
endif

# Check for standard headers and functions
if (cc.has_header_symbol('sys/sysmacros.h', 'major') and
  cc.has_header_symbol('sys/sysmacros.h', 'minor') and
  cc.has_header_symbol('sys/sysmacros.h', 'makedev'))
  pre_args += '-DMAJOR_IN_SYSMACROS'
endif
if (cc.has_header_symbol('sys/mkdev.h', 'major') and
  cc.has_header_symbol('sys/mkdev.h', 'minor') and
  cc.has_header_symbol('sys/mkdev.h', 'makedev'))
  pre_args += '-DMAJOR_IN_MKDEV'
endif

if cc.check_header('sched.h')
  pre_args += '-DHAS_SCHED_H'
  if cc.has_function('sched_getaffinity')
    pre_args += '-DHAS_SCHED_GETAFFINITY'
  endif
endif

if not ['linux'].contains(host_machine.system())
  # Deprecated on Linux and requires <sys/types.h> on FreeBSD and OpenBSD
  if cc.check_header('sys/sysctl.h', prefix : '#include <sys/types.h>')
    pre_args += '-DHAVE_SYS_SYSCTL_H'
  endif
endif

foreach h : ['xlocale.h', 'linux/futex.h', 'endian.h', 'dlfcn.h', 'sys/shm.h', 'cet.h', 'pthread_np.h']
  if cc.check_header(h)
    pre_args += '-DHAVE_@0@'.format(h.to_upper().underscorify())
  endif
endforeach

functions_to_detect = {
  'strtof': '',
  'mkostemp': '',
  'timespec_get': '#include <time.h>',
  'memfd_create': '',
  'random_r': '',
  'flock': '',
  'strtok_r': '',
  'getrandom': '',
  'qsort_r': '',
  'qsort_s': '',
}

foreach f, prefix: functions_to_detect
  if cc.has_function(f, prefix: prefix)
    pre_args += '-DHAVE_@0@'.format(f.to_upper())
  endif
endforeach

if cc.has_header_symbol('time.h', 'struct timespec')
   pre_args += '-DHAVE_STRUCT_TIMESPEC'
endif

with_c11_threads = false
if cc.has_function('thrd_create', prefix: '#include <threads.h>')
  if with_platform_android
    # Current only Android's c11 <threads.h> are verified
    pre_args += '-DHAVE_THRD_CREATE'
    with_c11_threads = true
  endif
endif

if cc.has_header_symbol('errno.h', 'program_invocation_name',
                        args : '-D_GNU_SOURCE')
   pre_args += '-DHAVE_PROGRAM_INVOCATION_NAME'
endif

if cc.has_header_symbol('math.h', 'issignaling',
                        args : '-D_GNU_SOURCE')
   pre_args += '-DHAVE_ISSIGNALING'
endif

# MinGW provides a __builtin_posix_memalign function, but not a posix_memalign.
# This means that this check will succeed, but then compilation will later
# fail. MSVC doesn't have this function at all, so only check for it on
# non-windows platforms.
if host_machine.system() != 'windows'
  if cc.has_function('posix_memalign')
    pre_args += '-DHAVE_POSIX_MEMALIGN'
  endif
endif

if cc.has_member('struct dirent', 'd_type', prefix: '''#include <sys/types.h>
   #include <dirent.h>''')
   pre_args += '-DHAVE_DIRENT_D_TYPE'
endif

# strtod locale support
if cc.links('''
    #define _GNU_SOURCE
    #include <stdlib.h>
    #include <locale.h>
    #ifdef HAVE_XLOCALE_H
    #include <xlocale.h>
    #endif
    int main() {
      locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
      const char *s = "1.0";
      char *end;
      double d = strtod_l(s, end, loc);
      float f = strtof_l(s, end, loc);
      freelocale(loc);
      return 0;
    }''',
    args : pre_args,
    name : 'strtod has locale support')
  pre_args += '-DHAVE_STRTOD_L'
endif

# Check for some linker flags
ld_args_bsymbolic = []
if cc.links('int main() { return 0; }', args : '-Wl,-Bsymbolic', name : 'Bsymbolic')
  ld_args_bsymbolic += '-Wl,-Bsymbolic'
endif
ld_args_gc_sections = []
if cc.links('static char unused() { return 5; } int main() { return 0; }',
            args : '-Wl,--gc-sections', name : 'gc-sections')
  ld_args_gc_sections += '-Wl,--gc-sections'
endif
with_ld_version_script = false
if cc.links('int main() { return 0; }',
            args : '-Wl,--version-script=@0@'.format(
              join_paths(meson.current_source_dir(), 'build-support/conftest.map')),
            name : 'version-script')
  with_ld_version_script = true
endif
with_ld_dynamic_list = false
if cc.links('int main() { return 0; }',
            args : '-Wl,--dynamic-list=@0@'.format(
              join_paths(meson.current_source_dir(), 'build-support/conftest.dyn')),
            name : 'dynamic-list')
  with_ld_dynamic_list = true
endif


# check for dl support
dep_dl = null_dep
if host_machine.system() != 'windows'
  if not cc.has_function('dlopen')
    dep_dl = cc.find_library('dl', required : true)
  endif
  if cc.has_function('dladdr', dependencies : dep_dl)
    # This is really only required for util/disk_cache.h
    pre_args += '-DHAVE_DLADDR'
  endif
endif

if cc.has_function('dl_iterate_phdr')
  pre_args += '-DHAVE_DL_ITERATE_PHDR'
endif

# Determine whether or not the rt library is needed for time functions
if host_machine.system() == 'windows' or cc.has_function('clock_gettime')
  dep_clock = null_dep
else
  dep_clock = cc.find_library('rt')
endif
dep_thread = dependency('threads')
if dep_thread.found() and host_machine.system() != 'windows'
  pre_args += '-DHAVE_PTHREAD'
  if host_machine.system() != 'netbsd' and cc.has_function(
      'pthread_setaffinity_np',
      dependencies : dep_thread,
      prefix : '#include <pthread.h>',
      args : '-D_GNU_SOURCE')
    pre_args += '-DHAVE_PTHREAD_SETAFFINITY'
  endif
endif
# this only exists on linux so either this is linux and it will be found, or
# it's not linux and wont
dep_m = cc.find_library('m', required : false)

dep_futex = null_dep
if host_machine.system() == 'windows'
  dep_futex = cc.find_library('synchronization', required : true)
endif


add_project_arguments(pre_args, language : ['c', 'cpp'])
add_project_arguments(c_args, language : ['c'])
add_project_arguments(cpp_args, language : ['cpp'])

inc_util = [include_directories('util')]

subdir('src')
subdir('tools')
subdir('util')
subdir('vulkan')

vk_c_args = []
inc_vk = include_directories('vulkan')
inc_src = include_directories('src')

with_moltenvk_dir = get_option('moltenvk-dir')
# MoltenVK options
if with_moltenvk_dir != ''
  inc_vk += [include_directories( join_paths(with_moltenvk_dir, 'include') )]
  vk_c_args += ['-x','objective-c'] # Put compiler into objective-C mode to allow for MacOS types, like IOSurface and CAMetalLayer
  vk_c_args += ['-iframework' , 'Foundation']
endif

vkoverhead = executable(
  'vkoverhead',
  [files_vkoverhead, vk_device_info, vk_instance, vk_dispatch_table, vk_enum_to_str, vk_extensions, files_util],
  include_directories : [inc_src, inc_vk, inc_util],
  dependencies : [dep_thread, dep_m, dep_dl, dep_ws2_32, dep_atomic, dep_clock, dep_futex, idep_mesautilc11],
  c_args: vk_c_args,
)

foz_parse = executable(
  'foz_parse',
  [files_foz_parse, files_util, vk_enum_to_str],
  include_directories : [inc_util, inc_vk],
  dependencies : [dep_thread, dep_m, dep_dl, dep_ws2_32, dep_atomic, dep_clock, dep_futex, idep_mesautilc11],
  c_args: vk_c_args,
)
