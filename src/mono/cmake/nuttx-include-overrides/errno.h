/*
 * errno.h override for Mono on NuttX protected build.
 *
 * NuttX's errno.h defines errno as get_errno() (rvalue-only) in protected
 * user mode, which breaks Mono's "errno = X" assignments. This wrapper
 * includes the real errno.h, then redefines errno as (*__errno()) so it
 * works as an lvalue. __errno() is provided by mono_nuttx_stubs.c.
 */
#ifndef _MONO_NUTTX_ERRNO_H_WRAPPER
#define _MONO_NUTTX_ERRNO_H_WRAPPER

/* Include the real NuttX errno.h using the next-in-path mechanism */
#include_next <errno.h>

/* Override errno to be an lvalue */
#undef errno
extern int *__errno(void);
#define errno (*__errno())

/* Provide set_errno/get_errno macros if not already defined */
#ifndef set_errno
#define set_errno(e) do { errno = (int)(e); } while (0)
#endif
#ifndef get_errno
#define get_errno() errno
#endif

#endif /* _MONO_NUTTX_ERRNO_H_WRAPPER */
