#ifndef DAEDALUS_ATTRIBUTES_H
#define DAEDALUS_ATTRIBUTES_H

// ABSL_HAVE_ATTRIBUTE
//
// A function-like feature checking macro that is a wrapper around
// `__has_attribute`, which is defined by GCC 5+ and Clang and evaluates to a
// nonzero constant integer if the attribute is supported or 0 if not.
//
// It evaluates to zero if `__has_attribute` is not defined by the compiler.
//
// GCC: https://gcc.gnu.org/gcc-5/changes.html
// Clang: https://clang.llvm.org/docs/LanguageExtensions.html
#ifdef __has_attribute
#define ABSL_HAVE_ATTRIBUTE(x) __has_attribute(x)
#else
#define ABSL_HAVE_ATTRIBUTE(x) 0
#endif

// ABSL_HAVE_CPP_ATTRIBUTE
//
// A function-like feature checking macro that accepts C++11 style attributes.
// It's a wrapper around `__has_cpp_attribute`, defined by ISO C++ SD-6
// (https://en.cppreference.com/w/cpp/experimental/feature_test). If we don't
// find `__has_cpp_attribute`, will evaluate to 0.
#if defined(__cplusplus) && defined(__has_cpp_attribute)
// NOTE: requiring __cplusplus above should not be necessary, but
// works around https://bugs.llvm.org/show_bug.cgi?id=23435.
#define ABSL_HAVE_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#define ABSL_HAVE_CPP_ATTRIBUTE(x) 0
#endif

// ABSL_ATTRIBUTE_ALWAYS_INLINE
// ABSL_ATTRIBUTE_NOINLINE
//
// Forces functions to either inline or not inline. Introduced in gcc 3.1.
#if ABSL_HAVE_ATTRIBUTE(always_inline) || (defined(__GNUC__) && !defined(__clang__))
#define ABSL_ATTRIBUTE_ALWAYS_INLINE __attribute__((always_inline))
#define ABSL_HAVE_ATTRIBUTE_ALWAYS_INLINE 1
#else
#define ABSL_ATTRIBUTE_ALWAYS_INLINE
#endif

#if ABSL_HAVE_ATTRIBUTE(noinline) || (defined(__GNUC__) && !defined(__clang__))
#define ABSL_ATTRIBUTE_NOINLINE __attribute__((noinline))
#define ABSL_HAVE_ATTRIBUTE_NOINLINE 1
#else
#define ABSL_ATTRIBUTE_NOINLINE
#endif

#ifdef _MSC_VER
#define NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#else
#define NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#endif
