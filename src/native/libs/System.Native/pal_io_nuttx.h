/*
 * pal_io_nuttx.h — NuttX compatibility shim for pal_io.c
 *
 * Bridges gaps between NuttX's POSIX implementation and what upstream
 * pal_io.c expects. Included only when __NuttX__ is defined.
 */

#pragma once
#ifdef __NuttX__

#include <stdint.h>
#include <dirent.h>

/* ------------------------------------------------------------------ */
/* fnmatch — NuttX has no fnmatch.h                                   */
/* ------------------------------------------------------------------ */
#define FNM_NOMATCH 1
#define FNM_NOESCAPE 0
#define FNM_PATHNAME 0
#define FNM_PERIOD   0
static inline int fnmatch(const char *pattern, const char *string, int flags)
{
    (void)pattern; (void)string; (void)flags;
    return FNM_NOMATCH;
}

/* ------------------------------------------------------------------ */
/* sys/file.h — NuttX has no flock() or LOCK_* constants              */
/* ------------------------------------------------------------------ */
#ifndef LOCK_SH
#define LOCK_SH 1
#define LOCK_EX 2
#define LOCK_NB 4
#define LOCK_UN 8
#endif

/* ------------------------------------------------------------------ */
/* DT_* directory entry types — NuttX uses DTYPE_* bitmask instead    */
/* ------------------------------------------------------------------ */
#ifndef DT_UNKNOWN
#define DT_UNKNOWN  0
#define DT_FIFO     1
#define DT_CHR      2
#define DT_DIR      4
#define DT_BLK      6
#define DT_REG      8
#define DT_LNK     10
#define DT_SOCK    12
#define DT_WHT     14
#endif

/* Convert NuttX DTYPE_* bitmask to POSIX DT_* sequential value */
static inline int32_t nuttx_dtype_to_posix(uint8_t d_type)
{
    if (d_type & DTYPE_DIRECTORY) return DT_DIR;
    if (d_type & DTYPE_FILE)     return DT_REG;
    if (d_type & DTYPE_CHR)      return DT_CHR;
    if (d_type & DTYPE_BLK)      return DT_BLK;
    if (d_type & DTYPE_LINK)     return DT_LNK;
    return DT_UNKNOWN;
}

/* ------------------------------------------------------------------ */
/* O_NOFOLLOW — NuttX may not define it                               */
/* ------------------------------------------------------------------ */
#ifndef O_NOFOLLOW
#define O_NOFOLLOW 0
#endif

/* ------------------------------------------------------------------ */
/* major()/minor() — NuttX has no sys/sysmacros.h                     */
/* ------------------------------------------------------------------ */
#ifndef major
#define major(dev) (0)
#define minor(dev) (0)
#endif

/* ------------------------------------------------------------------ */
/* NuttX S_IF* file type → POSIX S_IF* conversion                     */
/*                                                                     */
/* NuttX uses non-standard bit positions for file type in st_mode:     */
/*   IFIFO=0x0800 IFCHR=0x1000 IFDIR=0x1800 IFBLK=0x2000             */
/*   IFREG=0x2800 IFSOCK=0x4800 IFLNK=0x8000                         */
/* POSIX (and what .NET PAL expects):                                  */
/*   IFIFO=0x1000 IFCHR=0x2000 IFDIR=0x4000 IFBLK=0x6000             */
/*   IFREG=0x8000 IFLNK=0xA000 IFSOCK=0xC000                         */
/* Permission bits (lower 12 bits) are the same on both.              */
/* ------------------------------------------------------------------ */
#include <sys/stat.h>

/* POSIX-standard S_IF* values (what the .NET PAL expects) */
#define POSIX_S_IFMT   0xF000
#define POSIX_S_IFIFO  0x1000
#define POSIX_S_IFCHR  0x2000
#define POSIX_S_IFDIR  0x4000
#define POSIX_S_IFBLK  0x6000
#define POSIX_S_IFREG  0x8000
#define POSIX_S_IFLNK  0xA000
#define POSIX_S_IFSOCK 0xC000

static inline int32_t nuttx_mode_to_posix(int32_t nuttx_mode)
{
    /* Extract permission bits (lower 12 bits — same on both systems) */
    int32_t perms = nuttx_mode & 0x0FFF;

    /* Convert file type field */
    int32_t posix_type = 0;
    switch (nuttx_mode & S_IFMT) {
        case S_IFIFO:  posix_type = POSIX_S_IFIFO;  break;
        case S_IFCHR:  posix_type = POSIX_S_IFCHR;  break;
        case S_IFDIR:  posix_type = POSIX_S_IFDIR;  break;
        case S_IFBLK:  posix_type = POSIX_S_IFBLK;  break;
        case S_IFREG:  posix_type = POSIX_S_IFREG;  break;
        case S_IFSOCK: posix_type = POSIX_S_IFSOCK;  break;
        case S_IFLNK:  posix_type = POSIX_S_IFLNK;  break;
        default:       posix_type = 0;               break;
    }

    return posix_type | perms;
}

/* Reverse: POSIX mode → NuttX mode (for chmod, etc.) */
static inline int32_t posix_mode_to_nuttx(int32_t posix_mode)
{
    int32_t perms = posix_mode & 0x0FFF;

    int32_t nuttx_type = 0;
    switch (posix_mode & POSIX_S_IFMT) {
        case POSIX_S_IFIFO:  nuttx_type = S_IFIFO;  break;
        case POSIX_S_IFCHR:  nuttx_type = S_IFCHR;  break;
        case POSIX_S_IFDIR:  nuttx_type = S_IFDIR;  break;
        case POSIX_S_IFBLK:  nuttx_type = S_IFBLK;  break;
        case POSIX_S_IFREG:  nuttx_type = S_IFREG;  break;
        case POSIX_S_IFSOCK: nuttx_type = S_IFSOCK; break;
        case POSIX_S_IFLNK:  nuttx_type = S_IFLNK;  break;
        default:             nuttx_type = 0;         break;
    }

    return nuttx_type | perms;
}

#endif /* __NuttX__ */
