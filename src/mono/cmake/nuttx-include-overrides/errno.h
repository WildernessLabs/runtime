/*
 * errno.h override for Mono on NuttX protected build.
 *
 * NuttX's errno.h defines errno as get_errno() (rvalue-only) in protected
 * user mode, which breaks Mono's "errno = X" assignments. This wrapper
 * includes the real errno.h, then redefines errno as (*__errno()) so it
 * works as an lvalue via a static int in mono_nuttx_stubs.c.
 *
 * Errno sync mechanism:
 *   - PAL writes "errno = X": writes to the static (fast, no SVC)
 *   - PAL reads "errno":      reads from the static (fast, no SVC)
 *   - Kernel syscall fails:   --wrap wrappers in mono_nuttx_stubs.c
 *                              call get_errno() (one SVC) and store
 *                              the result in the static, so subsequent
 *                              PAL errno reads get the correct value.
 *
 * See mono_nuttx_stubs.c for the __wrap_xxx() wrappers and __errno().
 */
#ifndef _MONO_NUTTX_ERRNO_H_WRAPPER
#define _MONO_NUTTX_ERRNO_H_WRAPPER

/* Include the real NuttX errno.h using the next-in-path mechanism */
#include_next <errno.h>

/* Override errno to be an lvalue */
#undef errno
extern int *__errno(void);
#define errno (*__errno())

/* Write-through set_errno: updates both the lvalue static and NuttX's
 * real per-thread errno in the TCB via syscall.
 * __nuttx_set_errno() is provided by mono_nuttx_stubs.c. */
extern void __nuttx_set_errno(int errcode);
#undef set_errno
#define set_errno(e) do { int _e = (int)(e); errno = _e; __nuttx_set_errno(_e); } while (0)

/* get_errno reads through the synced __errno() pointer */
#undef get_errno
#define get_errno() errno

#endif /* _MONO_NUTTX_ERRNO_H_WRAPPER */
