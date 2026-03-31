/*
 * nuttx-compat.h — Compatibility declarations for legacy NuttX headers.
 *
 * Force-included via -include in the toolchain file. Provides POSIX
 * declarations missing from the older NuttX header set used by the
 * Meadow firmware.
 */

#ifndef _NUTTX_COMPAT_H
#define _NUTTX_COMPAT_H

/* POSIX version — not defined in legacy NuttX unistd.h but the RTOS is
 * POSIX-compliant enough for Mono's purposes (pthreads, signals, etc.) */
#ifndef _POSIX_VERSION
#define _POSIX_VERSION 200809L
#endif

/* Integer constant macros — disabled (#if 0) in legacy NuttX stdint.h */
#ifndef UINT32_C
#define UINT32_C(x) (x ## U)
#endif
#ifndef INT32_C
#define INT32_C(x) (x)
#endif
#ifndef UINT64_C
#define UINT64_C(x) (x ## ULL)
#endif
#ifndef INT64_C
#define INT64_C(x) (x ## LL)
#endif

/* Tell dlmalloc.h that mallinfo is already defined (NuttX defines it in
 * stdlib.h, not malloc.h, so dlmalloc's guard __INCLUDE_MALLOC_H misses it) */
#define __INCLUDE_MALLOC_H 1

/* NuttX sysconf() doesn't support _SC_PAGE_SIZE (returns -1). Mono's
 * dlmalloc uses sysconf(_SC_PAGE_SIZE) via HAVE_SYSCONF and ABORTs when
 * it gets 0xFFFFFFFF (not a power of 2). Pre-define malloc_getpagesize
 * to bypass the broken sysconf path. NuttX has no MMU pages; 4096 is
 * the standard default. */
#define malloc_getpagesize ((size_t)4096U)

/* Force C99 _Bool for bool instead of NuttX's _Bool8 (uint8_t).
 * Mono code assumes bool is int-sized (C99 _Bool). */
#define CONFIG_C99_BOOL8 1

/* strcasecmp/strncasecmp — in <strings.h>, not <string.h> on NuttX */
#include <strings.h>

/* realpath — in <stdlib.h> on modern POSIX, may be missing in legacy NuttX */
char *realpath(const char *path, char *resolved_path);

/* posix_memalign — POSIX memory allocation, implemented in mono_nuttx_stubs.c */
int posix_memalign(void **memptr, size_t alignment, size_t size);

/* POSIX functions missing from legacy NuttX headers.
 * Declarations only — implementations in mono_nuttx_stubs.c or not needed. */
#include <sys/types.h>
struct stat;
ssize_t readlink(const char *path, char *buf, size_t bufsize);
int lstat(const char *path, struct stat *buf);
char *mkdtemp(char *tmpl);

/* Force HAVE_MKDTEMP so Mono's gfile-unix.c uses mkdtemp() instead of
 * mktemp() (which has wrong return type on legacy NuttX). */
#define HAVE_MKDTEMP 1

/* SA_RESTART — not defined in legacy NuttX signal.h */
#ifndef SA_RESTART
#define SA_RESTART 0x10000000
#endif

/* Math functions and macros missing from legacy NuttX math.h */
double fmin(double x, double y);
double fmax(double x, double y);
float fminf(float x, float y);
float fmaxf(float x, float y);
#ifndef isunordered
#define isunordered(x, y) (__builtin_isunordered((x), (y)))
#endif
#ifndef signbit
#define signbit(x) (__builtin_signbit((x)))
#endif

/* POSIX rand48 family — not declared in legacy NuttX stdlib.h.
 * Actual implementations provided by mono_nuttx_stubs.c in firmware. */
#ifdef __cplusplus
extern "C" {
#endif

void srand48(long int seedval);
long int lrand48(void);

#ifdef __cplusplus
}
#endif

#endif /* _NUTTX_COMPAT_H */
