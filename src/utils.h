#ifndef UC_UTILS_H_
#define UC_UTILS_H_

#include <stddef.h>

//Gnerate a compiler error if the compile time
//constant expression fails
#define UC_STATIC_ASSERT(expr)\
    typedef int STATIC_ASSERT_FAILED[(expr) ? 1 : -1]

//MAX of a and b
#define UC_MAX(a,b) \
   ({ __typeof__ (a) _amx = (a); \
       __typeof__ (b) _bmx = (b); \
     _amx > _bmx ? _amx : _bmx; })

//min of a and b
#define UC_MIN(a,b) \
   ({ __typeof__ (a) _amn = (a); \
       __typeof__ (b) _bmn = (b); \
     _amn < _bmn ? _amn : _bmn; })

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))

//given 'ptr' as a pointer to a struct 'member',
//find the struct that ptr is the member of, where
//'type' is the type of the containing struct
//e.g.
//struct foo {
//   int i;
//   struct bar zap;
//};
// ...
//struct bar *p = ..; //the local p is a pointer to
//a 'zap' member inside a struct foo.
//give us the struct foo*:
//struct foo *f = CONTAINER_OF(p, struct foo, zap);
#define UC_CONTAINER_OF(ptr, type, member) ({                  \
typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)((void *)(unsigned char *)__mptr - offsetof(type, member) ); })

//the void* cast is to suppress alignment warnings

//Same as UC_CONTAINER_OF, but for a const pointer to avoid gcc warnings..
#define UC_CONST_CONTAINER_OF(ptr, type, member) ({                  \
const typeof( ((const type *)0)->member ) *__mptr = (ptr);    \
    (const type *)((const void *)(const unsigned char *)__mptr - offsetof(const type, member) ); })

/** Align a value.
 * @param val value to align
 * @param align alignment, must be power of 2
 * @return the aligned val
 */
#define UC_ALIGN(val,align) (((val)+(align)-1UL)&~((align)-1UL))

/**
 * Test if x is a power of 2
 *
 * @param x value, must be an unsigned type
 * @return 1 if x is a power of 20, 0 if it is not
 */
#define UC_IS_POW2(x) ((x) && !((x) & (x) - 1))

/**
 * Expands @x and turns it into a string.
 */
#define UC_STRINGIFY(x) UC_STRINGIFY_HLP(x)
#define UC_STRINGIFY_HLP(x) #x

/** Macro that expands to a string for the current file:line
 */
#define UC_SRC_LOCATION __FILE__ ":" UC_STRINGIFY(__LINE__)

#ifdef UC_TRACE_MODULE
#define MODULE_TRACEF(fmt, ...) TRACEF(fmt, ##__VA_ARGS__)
#define MODULE_TRACERAWF(fmt, ...) TRACERAWF(fmt, ## __VA_ARGS__)
#else
#define MODULE_TRACEF(fmt, ...) do { } while (0)
#define MODULE_TRACERAWF(fmt, ...) do { } while (0)
#endif

#ifdef DEBUG
    //include <stdio.h> if using the TRACEF() macro
    #define TRACEF(fmt, ...)\
    do { \
        fprintf(stdout, "%s (%s)\t" fmt, UC_SRC_LOCATION,  __FUNCTION__,  ##__VA_ARGS__);\
        fflush(stdout);\
    } while(0)
#else
    #define TRACEF(fmt, ...) do {} while(0)
#endif

/**
 * Round up x / y
 * (e.g 100/9 == 12, 3/2 == 2)
 * x + y must not overflow.
 *
 * @param x numerator, must be positive
 * @param y denominator
 * @return x/y rounded up
 */
#define UC_DIV_ROUND_UP(x, y) (((x) + ((y) - 1))/(y))

/**
 * Round x up to nearest multiple of y
 * e.g. UC_ROUND_UP(30,48) == 48
 * e.g. UC_ROUND_UP(49,48) == 96
 *
 * @param x numerator, must be positive
 * @param y denominator
 * @return x rounded up to nearest multiple of y
 */
#define UC_ROUND_UP(x, y) (UC_DIV_ROUND_UP((x), (y)) * (y))

/**
 * Round x down to nearest multiple of y
 * e.g. UC_ROUND_UP(30,48) == 0
 * e.g. UC_ROUND_UP(49,48) == 48
 *
 * @param x numerator, must be positive
 * @param y denominator
 * @return x rounded down nearest multiple of y
 */
#define UC_ROUND_DOWN(x, y) ((x) / (y) * (y))

/** Indicate the variable is unused.
 */
#define UC_UNUSED(x) x __attribute__((unused))


#ifdef __GNUC__
#  ifdef __clang__
#    define UC_INLINE inline __attribute__((always_inline))
#  else
#    define UC_INLINE inline __attribute__((always_inline,flatten))
#  endif
#else
#  define UC_INLINE inline
#endif

//ignore_value() macro to suppress warnings about unused value/return value etc.
#if 3 < __GNUC__ + (4 <= __GNUC_MINOR__)
# define ignore_value(x) \
    (__extension__ ({ __typeof__ (x) __x = (x); (void) __x; }))
#else
# define ignore_value(x) ((void) (x))
#endif

#endif

