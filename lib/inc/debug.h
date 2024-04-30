#ifndef DEBUG_H
#define DEBUG_H
#include <stdio.h>

// taken from [Debug Print Macro in C? - Stack Overflow](https://stackoverflow.com/questions/1941307/debug-print-macro-in-c)
// __func__ is standard C after c99

// Define the debug level
// 0: No debug
// 1: Standard debug
// 2: Medium debug
// 3: Deep debug
#define DEBUG 2

#if defined(DEBUG) && DEBUG >= 3
// Use this for deep debugging like variable values in loops etc.
#define DEBUG_PRINT3(fmt, args...) fprintf(stderr, "DEBUG(3): %s:%d:%s(): " fmt, \
                                           __FILE__, __LINE__, __func__, ##args)
#else
#define DEBUG_PRINT3(fmt, args...) /* Don't do anything in release builds */
#endif

#if defined(DEBUG) && DEBUG >= 2
// Use this for medium debugging like function entry/exit
#define DEBUG_PRINT2(fmt, args...) fprintf(stderr, "DEBUG(2): %s:%d:%s(): " fmt, \
                                           __FILE__, __LINE__, __func__, ##args)
#else
#define DEBUG_PRINT2(fmt, args...) /* Don't do anything in release builds */
#endif

#if defined(DEBUG) && DEBUG >= 1
// Use this for standard printf that should not be in the final version
#define DEBUG_PRINT(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
                                          __FILE__, __LINE__, __func__, ##args)
#else
#define DEBUG_PRINT(fmt, args...) /* Don't do anything in release builds */
#endif

#endif