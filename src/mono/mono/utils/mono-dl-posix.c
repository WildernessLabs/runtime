/**
 * \file
 * Interface to the dynamic linker
 *
 * Author:
 *    Mono Team (http://www.mono-project.com)
 *
 * Copyright 2001-2004 Ximian, Inc.
 * Copyright 2004-2009 Novell, Inc.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */
#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#if defined(_POSIX_VERSION) && !defined (HOST_WASM)

#include "mono/utils/mono-dl.h"
#include "mono/utils/mono-path.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <glib.h>
#include <dlfcn.h>

#if !defined (TARGET_MACH)
const char *
mono_dl_get_so_prefix (void)
{
	return "lib";
}
const char **
mono_dl_get_so_suffixes (void)
{
	static const char *suffixes[] = {
		".so",
#if defined (_AIX)
/*
 * libtool generating sysv style names (.so) still results in an .a archive,
 * (AIX prefers to put shared objects inside the same ar archive used for
 * static members, likely as a fat library system) so try common member names
 * when no suffix is given. The same path with member names can be tried, then
 * .a and .so extensions with member names. (The suffixless member names are
 * for when .so names are specified, but the archive is a member - the runtime
 * should just keep trying with one of these suffixes then.)
 *
 * Unfortunately, this strategy won't work for "aix" style libtool sonames -
 * it tries something awful like like "libfoo.a(libfoo.so.9)" which requires
 * you to hardcode a version in the member name.
 */
		"(shr.o)",
		"(shr_64.o)",
		".a(shr.o)",
		".a(shr_64.o)",
		".so(shr.o)",
		".so(shr_64.o)",
#endif
		"",
	};
	return suffixes;
}

#endif

#ifdef HOST_NUTTX
/* NuttX's dlopen() does not accept NULL (it dereferences the file name), so
 * the "main program" handle that mono_dl_open (NULL, ...) asks for is this
 * sentinel instead.  Symbol lookups through it find nothing, which matches
 * a statically linked image with no exported dynamic symbol table: P/Invokes
 * are resolved by the host's PINVOKE_OVERRIDE callback instead. */
static char nuttx_main_program_handle;
#endif

void *
mono_dl_open_file (const char *file, int flags, MonoError *error)
{
#ifdef HOST_NUTTX
	if (file == NULL)
		return &nuttx_main_program_handle;
#endif
#ifdef HOST_ANDROID
	/* Bionic doesn't support NULL filenames */
	if (!file)
		return NULL;
	/* The intention of calling `g_file_test (file, G_FILE_TEST_EXISTS)` is
	 * to speed up probing for non-existent libraries.  The problem is that
	 * if file is just a simple "libdl.so" then `dlopen (file)` doesn't just
	 * look for it in the current working directory, it will probe some
	 * other paths too.  (For example on desktop linux you'd also look in
	 * all the directories in LD_LIBRARY_PATH).  So the g_file_test() call
	 * is not a robust way to avoid calling dlopen if the filename is
	 * relative.
	 */
	if (g_path_is_absolute (file) && !g_file_test (file, G_FILE_TEST_EXISTS))
		return NULL;
#endif
#if defined(_AIX)
	/*
	 * dlopen is /weird/ on AIX
	 * shared libraries (really, all oobjects are, since PPC is PIC)
	 * can cohabitate with not just SOs of the other arch, but also
	 * with regular objects in an archive used for static linking
	 *
	 * we have to pass RTLD_MEMBER, otherwise lib.a(lib.o) doesn't work
	 */
	return dlopen (file, flags | RTLD_MEMBER);
#else
	return dlopen (file, flags);
#endif
}

void
mono_dl_close_handle (MonoDl *module, MonoError *error)
{
#ifdef HOST_NUTTX
	if (module->handle == &nuttx_main_program_handle)
		return;
#endif
	dlclose (module->handle);
}

void*
mono_dl_lookup_symbol (MonoDl *module, const char *name)
{
#ifdef HOST_NUTTX
	if (module->handle == &nuttx_main_program_handle)
		return NULL;
#endif
	return dlsym (module->handle, name);
}

int
mono_dl_convert_flags (int mono_flags, int native_flags)
{
	int lflags = native_flags;

	// Specifying both will default to LOCAL
	if (mono_flags & MONO_DL_GLOBAL && !(mono_flags & MONO_DL_LOCAL))
		lflags |= RTLD_GLOBAL;
	else
		lflags |= RTLD_LOCAL;

	if (mono_flags & MONO_DL_LAZY)
		lflags |= RTLD_LAZY;
	else
		lflags |= RTLD_NOW;

	return lflags;
}

char*
mono_dl_current_error_string (void)
{
	return g_strdup (dlerror ());
}

#else

#include <mono/utils/mono-compiler.h>

MONO_EMPTY_SOURCE_FILE (mono_dl_posix);

#endif
