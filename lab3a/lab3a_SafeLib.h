#ifndef LAB3A_SAFELIB_H_
#define LAB3A_SAFELIB_H_
#include <stddef.h>
#include <sys/types.h>
#define EXIT_SYS_CALL_FAIL 2

/* 
 *  Simply a "safe" wrapper for malloc, that checks its return value and
 *  handles it appropriately. s_ indicates a function is merely a safe wrapper
 *  of a standard function.
 */
void* s_malloc(size_t size);

/* 
 *  A safe wrapper for calloc.
 *  Checks the return value of calloc, and handles it appropriately.
 *  Otherwise, exactly the same as calloc.
 */
void* s_calloc(size_t nelem, size_t elsize);

/* 
 *  A safe wrapper for realloc.
 *  Checks the return value of realloc, and handles it appropriately.
 *  Otherwise, exactly the same as realloc.
 */
void* s_realloc(void* ptr, size_t size);

/* 
 *  A safe wrapper for atexit.
 *  Checks the return value of atexit, and handles it appropriately.
 *  Otherwise, exactly the same as atexit.
 */
int s_atexit(void (*func)(void));

/* 
 *  A safe wrapper for write.
 *  Checks the return value of write, and handles it appropriately.
 *  Otherwise, exactly the same as write.
 */
ssize_t s_write(int fildes, const void* buff, size_t nbyte);

/* 
 *  A safe wrapper for read.
 *  Checks the return value of read, and handles it appropriately.
 *  Otherwise, exactly the same as read.
 */
ssize_t s_read(int fildes, void* buff, size_t nbyte);

/* 
 *  A safe wrapper for pread.
 *  Checks the return value of pread, and handles it appropriately.
 *  Otherwise, exactly the same as pread.
 */
ssize_t s_pread(int fildes, void* buff, size_t nbyte, off_t offset);

#endif
