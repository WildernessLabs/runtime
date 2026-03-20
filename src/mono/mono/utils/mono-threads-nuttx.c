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

	/* pthread_get_stackaddr_np returns the top of the stack (highest address),
	 * but Mono expects the base (lowest address). Adjust accordingly. */
	*staddr -= *stsize;
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
