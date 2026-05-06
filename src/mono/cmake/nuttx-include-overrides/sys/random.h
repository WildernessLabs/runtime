/*
 * sys/random.h override for Mono on NuttX.
 *
 * NuttX declares: void getrandom(void *bytes, size_t nbytes)
 * POSIX/Linux:    ssize_t getrandom(void *buf, size_t buflen, unsigned int flags)
 *
 * minipal/random.c calls the 3-arg POSIX version. This wrapper includes
 * the real NuttX header, hides its declaration, and provides a POSIX-
 * compatible inline wrapper.
 */
#ifndef _MONO_NUTTX_SYS_RANDOM_H_WRAPPER
#define _MONO_NUTTX_SYS_RANDOM_H_WRAPPER

#include <sys/types.h>

/* Include the real NuttX sys/random.h to get the 2-arg declaration */
#include_next <sys/random.h>

/* Hide NuttX's getrandom and provide POSIX-compatible version */
static inline ssize_t __nuttx_getrandom_posix(void *buf, size_t buflen, unsigned int flags)
{
    (void)flags;
    getrandom(buf, buflen);
    return (ssize_t)buflen;
}

#define getrandom __nuttx_getrandom_posix

#endif /* _MONO_NUTTX_SYS_RANDOM_H_WRAPPER */
