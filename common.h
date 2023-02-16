#pragma once
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

// https://gcc.gnu.org/onlinedocs/cpp/Pragmas.html
#define DO_PRAGMA_(x) _Pragma (#x)
#define DO_PRAGMA(x) DO_PRAGMA_(x)

#ifdef __clang__
#define NUSED_BEGIN \
	DO_PRAGMA(clang diagnostic push) \
	DO_PRAGMA(clang diagnostic ignored "-Wunused-variable")
#elif  __GNUC__
#define NUSED_BEGIN \
	DO_PRAGMA(GCC diagnostic push) \
	DO_PRAGMA(GCC diagnostic ignored "-Wunused-variable")
#else // win32
#define NUSED_BEGIN \
DO_PRAGMA( warning( push ) ) \
DO_PRAGMA( warning( disable : 4101) )
#endif  // __clang__

#ifdef __clang__
#define NUSED_END DO_PRAGMA(clang diagnostic pop)
#elif __GNUC__
#define NUSED_END DO_PRAGMA(GCC diagnostic pop)
#else  // ! __clang__
#define NUSED_END DO_PRAGMA( warning( pop ) ) 
#endif  // ! __clang__

// thing to be supressed must be on the next line
#ifdef _MSC_VER
#define DBJ_SUPRESS DO_PRAGMA(warning(suppress: 4101))
#else
#define DBJ_SUPRESS
#endif
