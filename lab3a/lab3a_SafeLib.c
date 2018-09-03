 #include "lab3a_SafeLib.h"
 #include <stdio.h>
 #include <errno.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 
 void* s_malloc(size_t size)
{
    void* ptr = malloc(size);
    if( ptr == NULL && size != 0 )
    {
        int err = errno;
        fprintf(stderr, "ERROR: call to malloc(%zu) failed. %s.\n", size, strerror(err));
        exit(EXIT_SYS_CALL_FAIL);
    }
    return ptr;
}

void* s_calloc(size_t nelem, size_t elsize)
{
    void* ptr = calloc(nelem, elsize);
    if( nelem != 0 && elsize != 0 )
        if( ptr == NULL )
        {
            int err = errno;
            fprintf(stderr, "ERROR: call to calloc(%zu, %zu) failed. %s.\n", nelem, elsize, strerror(err));
            exit(EXIT_SYS_CALL_FAIL);
        }
    return ptr;
}

void* s_realloc(void* ptr, size_t size)
{
    void* newPtr = realloc(ptr, size);
    if( size != 0 )
        if( ptr == NULL )
        {
            int err = errno;
            fprintf(stderr, "ERROR: call to realloc failed. %s.\n", strerror(err));
            exit(EXIT_SYS_CALL_FAIL);
        }
    return newPtr;
}

int s_atexit(void (*func)(void))
{
    int stat = atexit(func);
    if( stat )
    {
        int err = errno;
        fprintf(stderr, "ERROR: call to atexit failed. %s.\n", strerror(err));
        exit(EXIT_SYS_CALL_FAIL);
    }
    return stat;
}

ssize_t s_write(int fildes, const void* buff, size_t nbyte)
{
    ssize_t noBytes;
    if( (noBytes = write(fildes, buff, nbyte)) < 0 )
    {
        int err = errno;
        fprintf(stderr, "ERROR: call to write() failed.\n %s.\n", strerror(err));
        exit(EXIT_SYS_CALL_FAIL);
    }
    return noBytes;
}

ssize_t s_read(int fildes, void* buff, size_t nbyte)
{
    ssize_t noBytes;
    if( (noBytes = read(fildes, buff, nbyte)) < 0 )
    {
        int err = errno;
        fprintf(stderr, "ERROR: call to read() failed.\n %s.\n", strerror(err));
        exit(EXIT_SYS_CALL_FAIL);
    }
    return noBytes;
}

ssize_t s_pread(int fildes, void* buff, size_t nbyte, off_t offset)
{
    ssize_t noBytes;
    if( (noBytes = pread(fildes, buff, nbyte, offset)) < 0 )
    {
        int err = errno;
        fprintf(stderr, "ERROR: call to pread() failed.\n %s.\n", strerror(err));
        exit(EXIT_SYS_CALL_FAIL);
    }
    return noBytes;
}
