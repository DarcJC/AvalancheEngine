#pragma once

// === FORCEINLINE ===
#if !defined(FORCEINLINE)
#   if defined(_MSC_VER) // Microsoft Visual Studio
#       define FORCEINLINE __forceinline
#   elif defined(__GNUC__) || defined(__clang__) // GCC and Clang
#       define FORCEINLINE inline __attribute__((always_inline))
#   elif defined(__INTEL_COMPILER) // Intel Compiler
#       define FORCEINLINE __forceinline
#   else
#       define FORCEINLINE inline
#   endif
#endif
// === FORCEINLINE ===

// === CONSTEXPR ===
#if !defined(AVALANCHE_CONSTEXPR)
#   define AVALANCHE_CONSTEXPR constexpr
#endif
// === CONSTEXPR ===

// === NOEXCEPT ===
#if !defined(AVALANCHE_NOEXCEPT)
#   define AVALANCHE_NOEXCEPT noexcept
#endif
#if !defined(AVALANCHE_NOEXCEPT_COND)
#   define AVALANCHE_NOEXCEPT_COND(expr) noexcept((expr))
#endif
// === NOEXCEPT ===

// === __FUNCTION__ ===
#if !defined(AVALANCHE_CURRENT_FUNCTION)
#   define AVALANCHE_CURRENT_FUNCTION static_cast<const char*>(__FUNCTION__)
#endif
// === __FUNCTION__ ===

// === Branch ===
#if !defined(AVALANCHE_UNLIKELY_BRANCH)
#   define AVALANCHE_UNLIKELY_BRANCH [[unlikely]]
#endif
#if !defined(AVALANCHE_LIKELY_BRANCH)
#   define AVALANCHE_LIKELY_BRANCH [[likely]]
#endif
// === Branch ===

// === Decl ===
#if !defined(AVALANCHE_CDECL)
#   if defined(_MSC_VER)
#       define AVALANCHE_CDECL __cdecl
#   elif defined(__GUNC__)
#       define AVALANCHE_CDECL __attribute__((__cdecl__))
#   else
#       define AVALANCHE_CDECL
#   endif
#endif
// === Decl ===
