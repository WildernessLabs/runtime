/**
 * \file
 * Low-level threading, NuttX version
 *
 * NuttX-specific thread support for the Mono runtime.
 * Provides stack bounds via pthread_get_stackaddr_np/pthread_get_stacksize_np.
 */

#include <config.h>

#if defined(HOST_NUTTX)

#include <mono/utils/mono-threads.h>
#include <mono/utils/mono-compiler.h>
#include <pthread.h>

void
mono_threads_platform_get_stack_bounds (guint8 **staddr, size_t *stsize)
{
	*staddr = (guint8*)pthread_get_stackaddr_np (pthread_self ());
	*stsize = pthread_get_stacksize_np (pthread_self ());

	/* Since NuttX 10.2.0 (upstream commit 2335b69120a, "arch: Allocate the
	 * space from the beginning in up_stack_frame"), pthread_get_stackaddr_np
	 * returns stack_base_ptr, the lowest usable address, and
	 * pthread_get_stacksize_np the usable size, so the stack is
	 * [staddr, staddr + stsize) and needs no adjustment.  The same commit moved
	 * the argument frame to the base of the stack (up_stack_frame() raises
	 * stack_base_ptr), so it is outside these bounds.  Up to NuttX 10.1 the
	 * address was adj_stack_ptr, the initial stack pointer at the top. */
}

guint64
mono_native_thread_os_id_get (void)
{
	return (guint64)(gsize)pthread_self ();
}

#else

#include <mono/utils/mono-compiler.h>

MONO_EMPTY_SOURCE_FILE (mono_threads_nuttx);

#endif
