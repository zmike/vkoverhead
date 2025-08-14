/*
 * Copyright © 2014 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef UTIL_MACROS_H
#define UTIL_MACROS_H

#include <assert.h>
#if defined(__HAIKU__)  && !defined(__cplusplus)
#define static_assert _Static_assert
#endif
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef _GAMING_XBOX
#define strdup _strdup
#define stricmp _stricmp
#define unlink _unlink
#define access(a, b) _access(a, b)
#endif

/* Compute the size of an array */
#ifndef ARRAY_SIZE
#  define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

/* For compatibility with Clang's __has_builtin() */
#ifndef __has_builtin
#  define __has_builtin(x) 0
#endif

#ifndef __has_attribute
#  define __has_attribute(x) 0
#endif

/**
 * __builtin_expect macros
 */
#if !defined(HAVE___BUILTIN_EXPECT)
#  define __builtin_expect(x, y) (x)
#endif

#ifndef likely
#  ifdef HAVE___BUILTIN_EXPECT
#    define likely(x)   __builtin_expect(!!(x), 1)
#    define unlikely(x) __builtin_expect(!!(x), 0)
#  else
#    define likely(x)   (x)
#    define unlikely(x) (x)
#  endif
#endif

/**
 * __builtin_types_compatible_p compat
 */
#if defined(__cplusplus) || !defined(HAVE___BUILTIN_TYPES_COMPATIBLE_P)
#  define __builtin_types_compatible_p(type1, type2) (1)
#endif

/* This should match linux gcc cdecl semantics everywhere, so that we
 * just codegen one calling convention on all platforms.
 */
#ifdef _MSC_VER
#define UTIL_CDECL __cdecl
#else
#define UTIL_CDECL
#endif

/**
 * Static (compile-time) assertion.
 */
#define STATIC_ASSERT(cond) do { \
   static_assert(cond, #cond); \
} while (0)

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 */
#ifndef __GNUC__
   /* a grown-up compiler is required for the extra type checking: */
#  define container_of(ptr, type, member)                               \
      (type*)((uint8_t *)ptr - offsetof(type, member))
#else
#  define __same_type(a, b) \
      __builtin_types_compatible_p(__typeof__(a), __typeof__(b))
#  define container_of(ptr, type, member) ({                            \
         uint8_t *__mptr = (uint8_t *)(ptr);                            \
         static_assert(__same_type(*(ptr), ((type *)0)->member) ||      \
                       __same_type(*(ptr), void),                       \
                       "pointer type mismatch in container_of()");      \
         ((type *)(__mptr - offsetof(type, member)));                   \
      })
#endif

/**
 * Unreachable macro. Useful for suppressing "control reaches end of non-void
 * function" warnings.
 */
#if defined(HAVE___BUILTIN_UNREACHABLE) || __has_builtin(__builtin_unreachable)
#define UNREACHABLE(str)    \
do {                        \
   (void)"" str; /* str must be a string literal */ \
   assert(!str);            \
   __builtin_unreachable(); \
} while (0)
#elif defined (_MSC_VER)
#define UNREACHABLE(str)    \
do {                        \
   (void)"" str; /* str must be a string literal */ \
   assert(!str);            \
   __assume(0);             \
} while (0)
#else
#define UNREACHABLE(str)    \
do {                        \
   (void)"" str; /* str must be a string literal */ \
   assert(!str);            \
} while (0)
#endif

/**
 * Assume macro. Useful for expressing our assumptions to the compiler,
 * typically for purposes of silencing warnings.
 */
#if __has_builtin(__builtin_assume)
#define assume(expr)       \
do {                       \
   assert(expr);           \
   __builtin_assume(expr); \
} while (0)
#elif defined HAVE___BUILTIN_UNREACHABLE
#define assume(expr) ((expr) ? ((void) 0) \
                             : (assert(!"assumption failed"), \
                                __builtin_unreachable()))
#elif defined (_MSC_VER)
#define assume(expr) __assume(expr)
#else
#define assume(expr) assert(expr)
#endif

/* Attribute const is used for functions that have no effects other than their
 * return value, and only rely on the argument values to compute the return
 * value.  As a result, calls to it can be CSEed.  Note that using memory
 * pointed to by the arguments is not allowed for const functions.
 */
#if !defined(__clang__) && defined(HAVE_FUNC_ATTRIBUTE_CONST)
#define ATTRIBUTE_CONST __attribute__((__const__))
#else
#define ATTRIBUTE_CONST
#endif

#ifdef HAVE_FUNC_ATTRIBUTE_FLATTEN
#define FLATTEN __attribute__((__flatten__))
#else
#define FLATTEN
#endif

#ifdef HAVE_FUNC_ATTRIBUTE_FORMAT
#if defined (__MINGW_PRINTF_FORMAT)
# define PRINTFLIKE(f, a) __attribute__ ((format(__MINGW_PRINTF_FORMAT, f, a)))
#else
# define PRINTFLIKE(f, a) __attribute__ ((format(__printf__, f, a)))
#endif
#else
#define PRINTFLIKE(f, a)
#endif

#ifdef HAVE_FUNC_ATTRIBUTE_MALLOC
#define MALLOCLIKE __attribute__((__malloc__))
#else
#define MALLOCLIKE
#endif

/* Forced function inlining */
/* Note: Clang also sets __GNUC__ (see other cases below) */
#ifndef ALWAYS_INLINE
#  if defined(__GNUC__)
#    define ALWAYS_INLINE inline __attribute__((always_inline))
#  elif defined(_MSC_VER)
#    define ALWAYS_INLINE __forceinline
#  else
#    define ALWAYS_INLINE inline
#  endif
#endif

/* Used to optionally mark structures with misaligned elements or size as
 * packed, to trade off performance for space.
 */
#ifdef HAVE_FUNC_ATTRIBUTE_PACKED
#  if defined(__MINGW32__) || defined(__MINGW64__)
#    define PACKED __attribute__((gcc_struct,__packed__))
#  else
#    define PACKED __attribute__((__packed__))
#  endif
#  define ENUM_PACKED __attribute__((packed))
#else
#define PACKED
#define ENUM_PACKED
#endif

/* Attribute pure is used for functions that have no effects other than their
 * return value.  As a result, calls to it can be dead code eliminated.
 */
#ifdef HAVE_FUNC_ATTRIBUTE_PURE
#define ATTRIBUTE_PURE __attribute__((__pure__))
#else
#define ATTRIBUTE_PURE
#endif

#ifdef HAVE_FUNC_ATTRIBUTE_RETURNS_NONNULL
#define ATTRIBUTE_RETURNS_NONNULL __attribute__((__returns_nonnull__))
#else
#define ATTRIBUTE_RETURNS_NONNULL
#endif

#ifndef NORETURN
#  ifdef _MSC_VER
#    define NORETURN __declspec(noreturn)
#  elif defined HAVE_FUNC_ATTRIBUTE_NORETURN
#    define NORETURN __attribute__((__noreturn__))
#  else
#    define NORETURN
#  endif
#endif

#ifdef HAVE_FUNC_ATTRIBUTE_OPTIMIZE
#define ATTRIBUTE_OPTIMIZE(flags) __attribute__((__optimize__((flags))))
#else
#define ATTRIBUTE_OPTIMIZE(flags)
#endif

#ifdef __cplusplus
/**
 * Macro function that evaluates to true if T is a trivially
 * destructible type -- that is, if its (non-virtual) destructor
 * performs no action and all member variables and base classes are
 * trivially destructible themselves.
 */
#   if defined(__clang__)
#      if __has_builtin(__is_trivially_destructible)
#         define HAS_TRIVIAL_DESTRUCTOR(T) __is_trivially_destructible(T)
#      elif (defined(__has_feature) && __has_feature(has_trivial_destructor))
#         define HAS_TRIVIAL_DESTRUCTOR(T) __has_trivial_destructor(T)
#      endif
#   elif defined(__GNUC__)
#      if ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 3)))
#         define HAS_TRIVIAL_DESTRUCTOR(T) __has_trivial_destructor(T)
#      endif
#   elif defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#      define HAS_TRIVIAL_DESTRUCTOR(T) __has_trivial_destructor(T)
#   endif
#   ifndef HAS_TRIVIAL_DESTRUCTOR
       /* It's always safe (if inefficient) to assume that a
        * destructor is non-trivial.
        */
#      define HAS_TRIVIAL_DESTRUCTOR(T) (false)
#   endif
#endif

/**
 * This marks symbols that should be visible to dynamic library consumers.
 */
#ifndef PUBLIC
#  if defined(_WIN32)
#    define PUBLIC __declspec(dllexport)
#  elif defined(__GNUC__)
#    define PUBLIC __attribute__((visibility("default")))
#  else
#    define PUBLIC
#  endif
#endif

/**
 * UNUSED marks variables (or sometimes functions) that have to be defined,
 * but are sometimes (or always) unused beyond that. A common case is for
 * a function parameter to be used in some build configurations but not others.
 * Another case is fallback vfuncs that don't do anything with their params.
 *
 * Note that this should not be used for identifiers used in `assert()`;
 * see ASSERTED below.
 */
#ifdef HAVE_FUNC_ATTRIBUTE_UNUSED
#define UNUSED __attribute__((unused))
#elif defined (_MSC_VER)
#define UNUSED __pragma(warning(suppress:4100 4101 4189))
#else
#define UNUSED
#endif

/**
 * Use ASSERTED to indicate that an identifier is unused outside of an `assert()`,
 * so that assert-free builds don't get "unused variable" warnings.
 */
#ifdef NDEBUG
#define ASSERTED UNUSED
#else
#define ASSERTED
#endif

#ifdef HAVE_FUNC_ATTRIBUTE_WARN_UNUSED_RESULT
#define MUST_CHECK __attribute__((warn_unused_result))
#else
#define MUST_CHECK
#endif

#if defined(__GNUC__)
#define ATTRIBUTE_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define ATTRIBUTE_NOINLINE __declspec(noinline)
#else
#define ATTRIBUTE_NOINLINE
#endif

/**
 * Check that STRUCT::FIELD can hold MAXVAL.  We use a lot of bitfields
 * in Mesa/gallium.  We have to be sure they're of sufficient size to
 * hold the largest expected value.
 * Note that with MSVC, enums are signed and enum bitfields need one extra
 * high bit (always zero) to ensure the max value is handled correctly.
 * This macro will detect that with MSVC, but not GCC.
 */
#define ASSERT_BITFIELD_SIZE(STRUCT, FIELD, MAXVAL) \
   do { \
      ASSERTED STRUCT s; \
      s.FIELD = (MAXVAL); \
      assert((int) s.FIELD == (MAXVAL) && "Insufficient bitfield size!"); \
   } while (0)


/** Compute ceiling of integer quotient of A divided by B. */
#define DIV_ROUND_UP( A, B )  ( ((A) + (B) - 1) / (B) )

/**
 * Clamp X to [MIN, MAX].
 * This is a macro to allow float, int, unsigned, etc. types.
 * We arbitrarily turn NaN into MIN.
 */
#define CLAMP( X, MIN, MAX )  ( (X)>(MIN) ? ((X)>(MAX) ? (MAX) : (X)) : (MIN) )

/* Syntax sugar occuring frequently in graphics code */
#define SATURATE( X ) CLAMP(X, 0.0f, 1.0f)

/** Minimum of two values: */
#define MIN2( A, B )   ( (A)<(B) ? (A) : (B) )

/** Maximum of two values: */
#define MAX2( A, B )   ( (A)>(B) ? (A) : (B) )

/** Minimum of three values: */
#define MIN3( A, B, C ) ((A) < (B) ? MIN2(A, C) : MIN2(B, C))

/** Maximum of three values: */
#define MAX3( A, B, C ) ((A) > (B) ? MAX2(A, C) : MAX2(B, C))

/** Minimum of four values: */
#define MIN4( A, B, C, D ) ((A) < (B) ? MIN3(A, C, D) : MIN3(B, C, D))

/** Maximum of four values: */
#define MAX4( A, B, C, D ) ((A) > (B) ? MAX3(A, C, D) : MAX3(B, C, D))

/** Align a value to a power of two */
#define ALIGN_POT(x, pot_align) (((x) + (pot_align) - 1) & ~((pot_align) - 1))

/** Checks is a value is a power of two. Does not handle zero. */
#define IS_POT(v) (((v) & ((v) - 1)) == 0)

/** Checks is a value is a power of two. Zero handled. */
#define IS_POT_NONZERO(v) ((v) != 0 && IS_POT(v))

/** Set a single bit */
#define BITFIELD_BIT(b)      (1u << (b))
/** Set all bits up to excluding bit b */
#define BITFIELD_MASK(b)      \
   ((b) == 32 ? (~0u) : BITFIELD_BIT((b) & 31) - 1)
/** Set count bits starting from bit b  */
#define BITFIELD_RANGE(b, count) \
   (BITFIELD_MASK((b) + (count)) & ~BITFIELD_MASK(b))

/** Set a single bit */
#define BITFIELD64_BIT(b)      (UINT64_C(1) << (b))
/** Set all bits up to excluding bit b */
#define BITFIELD64_MASK(b)      \
   ((b) == 64 ? (~UINT64_C(0)) : BITFIELD64_BIT((b) & 63) - 1)
/** Set count bits starting from bit b  */
#define BITFIELD64_RANGE(b, count) \
   (BITFIELD64_MASK((b) + (count)) & ~BITFIELD64_MASK(b))

static inline int64_t
u_intN_max(unsigned bit_size)
{
   assert(bit_size <= 64 && bit_size > 0);
   return INT64_MAX >> (64 - bit_size);
}

static inline int64_t
u_intN_min(unsigned bit_size)
{
   /* On 2's compliment platforms, which is every platform Mesa is likely to
    * every worry about, stdint.h generally calculated INT##_MIN in this
    * manner.
    */
   return (-u_intN_max(bit_size)) - 1;
}

static inline uint64_t
u_uintN_max(unsigned bit_size)
{
   assert(bit_size <= 64 && bit_size > 0);
   return UINT64_MAX >> (64 - bit_size);
}

/* alignas usage
 * For struct or union, use alignas(align_size) on any member
 * of it will make it aligned to align_size.
 * See https://en.cppreference.com/w/c/language/_Alignas for
 * details. We can use static_assert and alignof to check if
 * the alignment result of alignas(align_size) on struct or
 * union is valid.
 * For example:
 *   static_assert(alignof(struct tgsi_exec_machine) == 16, "")
 * Also, we can use special code to see the size of the aligned
 * struct or union at the compile time with GCC, Clang or MSVC.
 * So we can see if the size of union or struct are as expected
 * when using alignas(align_size) on its member.
 * For example:
 *   char (*__kaboom)[sizeof(struct tgsi_exec_machine)] = 1;
 * can show us the size of struct tgsi_exec_machine at compile
 * time.
 */
#ifndef __cplusplus
#ifdef _MSC_VER
#define alignof _Alignof
#define alignas _Alignas
#else
#include <stdalign.h>
#endif
#endif

/* Macros for static type-safety checking.
 *
 * https://clang.llvm.org/docs/ThreadSafetyAnalysis.html
 */

#if __has_attribute(capability)
typedef int __attribute__((capability("mutex"))) lock_cap_t;

#define guarded_by(l) __attribute__((guarded_by(l)))
#define acquire_cap(l) __attribute((acquire_capability(l), no_thread_safety_analysis))
#define release_cap(l) __attribute((release_capability(l), no_thread_safety_analysis))
#define assert_cap(l) __attribute((assert_capability(l), no_thread_safety_analysis))
#define requires_cap(l) __attribute((requires_capability(l)))
#define disable_thread_safety_analysis __attribute((no_thread_safety_analysis))

#else

typedef int lock_cap_t;

#define guarded_by(l)
#define acquire_cap(l)
#define release_cap(l)
#define assert_cap(l)
#define requires_cap(l)
#define disable_thread_safety_analysis

#endif

#define DO_PRAGMA(X) _Pragma (#X)

#if defined(__clang__)
#define PRAGMA_DIAGNOSTIC_PUSH       _Pragma("clang diagnostic push")
#define PRAGMA_DIAGNOSTIC_POP        _Pragma("clang diagnostic pop")
#define PRAGMA_DIAGNOSTIC_ERROR(X)   DO_PRAGMA( clang diagnostic error #X )
#define PRAGMA_DIAGNOSTIC_WARNING(X) DO_PRAGMA( clang diagnostic warning #X )
#define PRAGMA_DIAGNOSTIC_IGNORED(X) DO_PRAGMA( clang diagnostic ignored #X )
#elif defined(__GNUC__)
#define PRAGMA_DIAGNOSTIC_PUSH       _Pragma("GCC diagnostic push")
#define PRAGMA_DIAGNOSTIC_POP        _Pragma("GCC diagnostic pop")
#define PRAGMA_DIAGNOSTIC_ERROR(X)   DO_PRAGMA( GCC diagnostic error #X )
#define PRAGMA_DIAGNOSTIC_WARNING(X) DO_PRAGMA( GCC diagnostic warning #X )
#define PRAGMA_DIAGNOSTIC_IGNORED(X) DO_PRAGMA( GCC diagnostic ignored #X )
#else
#define PRAGMA_DIAGNOSTIC_PUSH
#define PRAGMA_DIAGNOSTIC_POP
#define PRAGMA_DIAGNOSTIC_ERROR(X)
#define PRAGMA_DIAGNOSTIC_WARNING(X)
#define PRAGMA_DIAGNOSTIC_IGNORED(X)
#endif

#define PASTE2(a, b) a ## b
#define PASTE3(a, b, c) a ## b ## c
#define PASTE4(a, b, c, d) a ## b ## c ## d

#define CONCAT2(a, b) PASTE2(a, b)
#define CONCAT3(a, b, c) PASTE3(a, b, c)
#define CONCAT4(a, b, c, d) PASTE4(a, b, c, d)

#if defined(__GNUC__)
#define PRAGMA_POISON(X) DO_PRAGMA( GCC poison X )
#elif defined(__clang__)
#define PRAGMA_POISON(X) DO_PRAGMA( clang poison X )
#else
#define PRAGMA_POISON
#endif

/*
 * SWAP - swap value of @a and @b
 */
#define SWAP(a, b)                                                             \
   do {                                                                        \
      __typeof__(a) __tmp = (a);                                               \
      (a) = (b);                                                               \
      (b) = __tmp;                                                             \
   } while (0)

#endif /* UTIL_MACROS_H */
