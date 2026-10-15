/*
 * nuttx-compat.h - Compatibility definitions for building Mono against NuttX.
 *
 * Force-included via -include in the toolchain file.  It holds only what the
 * NuttX 12 headers used by Meadow.OS still lack, or where the runtime needs a
 * different value than NuttX provides.  Everything else (realpath, readlink,
 * lstat, mkdtemp, posix_memalign, the rand48 family, SA_RESTART,
 * _POSIX_VERSION, ...) comes from the NuttX headers, and the C99 math
 * functions and macros come from the toolchain's newlib <math.h>.
 */

#ifndef _NUTTX_COMPAT_H
#define _NUTTX_COMPAT_H

/* Identify this as a NuttX build for #ifdef __NuttX__ guards throughout the
 * Mono runtime (e.g. the sgen-stw.c stack bounds fix).  CMake normally passes
 * -D__NuttX__ as well. */
#ifndef __NuttX__
#define __NuttX__ 1
#endif

/* Integer constant macros.  The NuttX <stdint.h> uses these but does not
 * define them, so take them from the compiler, which keeps them consistent
 * with __INT32_TYPE__ and friends (int32_t is 'long int' with
 * arm-none-eabi GCC). */
#ifndef INT32_C
#define INT32_C(x) __INT32_C(x)
#endif
#ifndef UINT32_C
#define UINT32_C(x) __UINT32_C(x)
#endif
#ifndef INT64_C
#define INT64_C(x) __INT64_C(x)
#endif
#ifndef UINT64_C
#define UINT64_C(x) __UINT64_C(x)
#endif

/* Stop dlmalloc.h from including the NuttX <malloc.h>, whose struct
 * mallinfo conflicts with dlmalloc's own definition.  The guard name is the
 * one NuttX uses. */
#define __INCLUDE_MALLOC_H 1

/* Page size for dlmalloc.  Without an MMU, NuttX's sysconf(_SC_PAGESIZE)
 * returns CONFIG_MM_PGSIZE if set, otherwise CONFIG_PTHREAD_STACK_MIN (256
 * on Meadow), which is not a sensible allocation granularity.  NuttX has no
 * MMU pages; use the conventional 4096. */
#define malloc_getpagesize ((size_t)4096U)

/* strcasecmp/strncasecmp are declared in <strings.h>, not <string.h>. */
#include <strings.h>

/* _POSIX_HOST_NAME_MAX is not defined by the NuttX <limits.h>. */
#ifndef _POSIX_HOST_NAME_MAX
#define _POSIX_HOST_NAME_MAX 255
#endif

/* NuttX has no IFF_ALLMULTI interface flag.  Define it as 0 so tests such as
 * (flags & (IFF_MULTICAST | IFF_ALLMULTI)) only see IFF_MULTICAST.  (A
 * Linux-style value such as (1 << 8) would alias NuttX's IFF_NAT.) */
#ifndef IFF_ALLMULTI
#define IFF_ALLMULTI 0
#endif

/* Meadow.OS interface flag: espcp (the ESP32 WiFi coprocessor driver) sets it
 * on the interface it reports, so that the interface can be identified as
 * WiFi.  It must match IFF_WIFI in Meadow.OS F7MeadowOS/espcp/espcp_usrsock.h
 * and uses a bit that NuttX <net/if.h> leaves free. */
#define MEADOW_IFF_WIFI (1 << 14)

#endif /* _NUTTX_COMPAT_H */
