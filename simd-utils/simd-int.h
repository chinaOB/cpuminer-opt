#if !defined(SIMD_INT_H__)
#define SIMD_INT_H__ 1

// Endian byte swap
#if defined(__x86_64__)

#define bswap_64    __builtin_bswap64
#define bswap_32    __builtin_bswap32

#elif defined(__aarch64__)

//#pragma message "aarch64 fast bswap"

static inline uint64_t bswap_64( uint64_t a )
{
   uint64_t b;
   asm( "rev %0, %1\n\t" : "=r"(b) : "r"(a) );
   return b;
}

static inline uint32_t bswap_32( uint32_t a )
{
   uint32_t b;
   asm( "rev32 %0, %1\n\t" : "=r"(b) : "r"(a) );
   return b;
}

#else

#define bswap_64(x) \
    ( ( ( (x) & 0x00000000FFFFFFFF ) << 32 ) \
    | ( ( (x) & 0xFFFFFFFF00000000 ) >> 32 ) \
    | ( ( (x) & 0x0000FFFF0000FFFF ) << 16 ) \
    | ( ( (x) & 0xFFFF0000FFFF0000 ) >> 16 ) \
    | ( ( (x) & 0x00FF00FF00FF00FF ) <<  8 ) \
    | ( ( (x) & 0xFF00FF00FF00FF00 ) >>  8 ) )

#define bswap_32(x) \
   ( ( ( (x) << 24 ) & 0xff000000 ) | ( ((x) <<  8 ) & 0x00ff0000 ) \
   | ( ( (x) >>  8 ) & 0x0000ff00 ) | ( ((x) >> 24 ) & 0x000000ff ) )

#endif

// Bit rotation
#if defined(__x86_64__)

#define rol64       __rolq
#define ror64       __rorq
#define rol32       __rold
#define ror32       __rord

#elif defined(__aarch64__)

//#pragma message "aarch64 fast bit rotation"

// "ror" instruction (intrinsic?) for 32 & 64 bits, args must determine size.

static inline uint64_t ror64( uint64_t a, const int c )
{
   uint64_t b;
   asm( "ror %0, %1, %2\n\t" : "=r"(b) : "r"(a), "r"(c) );
   return b;
}
#define rol64( a, c )     ror64( a, 64-(c) )

static inline uint32_t ror32( uint32_t a, const int c )
{
   uint32_t b;
   asm( "ror %0, %1, %2\n\t" : "=r"(b) : "r"(a), "r"(c) );
   return b;
}
#define rol32( a, c )     ror32( a, 32-(c) )

#else

#define ror64( x, c )    ( ( (x) >> (c) ) | ( (x) << (64-(c)) ) )
#define rol64( x, c )    ( ( (x) << (c) ) | ( (x) >> (64-(c)) ) )
#define ror32( x, c )    ( ( (x) >> (c) ) | ( (x) << (32-(c)) ) )
#define rol32( x, c )    ( ( (x) << (c) ) | ( (x) >> (32-(c)) ) )

#endif

// Safe division, integer or floating point. For floating point it's as  
// safe as 0 is precisely zero.
// Returns safe_result if division by zero, typically zero.
#define safe_div( dividend, divisor, safe_result ) \
   ( (divisor) == 0 ? safe_result : ( (dividend) / (divisor) )  )


///////////////////////////////////////
// 
//      128 bit integers
//
// 128 bit integers are inneficient and not a shortcut for __m128i.
// Native type __int128 supported starting with GCC-4.8.
//
// __int128 uses two 64 bit GPRs to hold the data. The main benefits are
// for 128 bit arithmetic. Vectors are preferred when 128 bit arith
// is not required. int128 also works better with other integer sizes.
// Vectors benefit from wider registers.
//
// For safety use typecasting on all numeric arguments.
//
// Use typecasting for conversion to/from 128 bit vector:
// __m128i v128 = (__m128i)my_int128l
// __m256i v256 = _mm256_set_m128i( (__m128i)my_int128, (__m128i)my_int128 );
// my_int128 = (uint128_t)_mm256_extracti128_si256( v256, 1 );

// obsolete test
// Compiler check for __int128 support
// Configure also has a test for int128.
#if ( __GNUC__ > 4 ) || ( ( __GNUC__ == 4 ) && ( __GNUC_MINOR__ >= 8 ) )
  #define GCC_INT128 1
#endif

// obsolte test
#if !defined(GCC_INT128)
  #warning "__int128 not supported, requires GCC-4.8 or newer."
#endif

#if defined(GCC_INT128)

// Familiar looking type names
typedef          __int128  int128_t;
typedef unsigned __int128 uint128_t;

typedef union
{
   uint128_t u128;
   uint64_t  u64[2];
   uint32_t  u32[4];
} __attribute__ ((aligned (16))) u128_ovly;

// Extracting the low bits is a trivial cast.
// These specialized functions are optimized while providing a
// consistent interface.
#define u128_hi64( x )    ( (uint64_t)( (uint128_t)(x) >> 64 ) )
#define u128_lo64( x )    ( (uint64_t)(x) )

#endif  // GCC_INT128

#endif // SIMD_INT_H__


