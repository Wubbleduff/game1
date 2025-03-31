
#pragma once

#include <immintrin.h>

#if defined(__clang__)
    #define MAYBE_UNUSED __attribute__((unused))
#elif defined(__GNUC__) || defined(__GNUG__)
    #define MAYBE_UNUSED __attribute__((unused))
#elif defined(_MSC_VER)
    #define MAYBE_UNUSED 
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
typedef float f32;
typedef double f64;

_Static_assert(sizeof(u8) == 1, "Unexpected type size.");
_Static_assert(sizeof(u16) == 2, "Unexpected type size.");
_Static_assert(sizeof(u32) == 4, "Unexpected type size.");
_Static_assert(sizeof(u64) == 8, "Unexpected type size.");
_Static_assert(sizeof(s8) == 1, "Unexpected type size.");
_Static_assert(sizeof(s16) == 2, "Unexpected type size.");
_Static_assert(sizeof(s32) == 4, "Unexpected type size.");
_Static_assert(sizeof(s64) == 8, "Unexpected type size.");
_Static_assert(sizeof(f32) == 4, "Unexpected type size.");
_Static_assert(sizeof(f64) == 8, "Unexpected type size.");

#define u8_MAX 0xFF
#define u16_MAX 0xFFFF
#define u32_MAX 0xFFFFFFFF
#define u64_MAX 0xFFFFFFFFFFFFFFFF
#define s8_MAX 0x7F
#define s16_MAX 0x7FFF
#define s32_MAX 0x7FFFFFFF
#define s64_MAX 0x7FFFFFFFFFFFFFFF

void assert_fn(const char* file, int line, const u64 c, const char* msg);
#define ASSERT(c, msg, ...) assert_fn(__FILE__, __LINE__, (const u64)(c), (msg))

#define ARRAY_COUNT(N) (sizeof(N) / sizeof((N)[0]))

#define COPY_ARRAY(DST, SRC) \
    do { \
        _Static_assert(sizeof(DST) == sizeof(SRC), "Array element sizes do not match."); \
        memcpy((DST), (SRC), sizeof(SRC)); \
    } while(0)

#define ZERO_ARRAY(a) \
    do { \
        memset((a), 0, sizeof(a)); \
    } while(0)

#define KB(N) (N * 1024ULL)
#define MB(N) (N * 1024ULL * 1024ULL)
#define GB(N) (N * 1024ULL * 1024ULL * 1024ULL)

