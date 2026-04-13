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

	/* NuttX's pthread_get_stackaddr_np returns adj_stack_ptr (initial SP) and
	 * pthread_get_stacksize_np returns adj_stack_size.  However, up_stack_frame()
	 * carves a frame off the top of the stack for argument passing BEFORE the
	 * thread starts, reducing adj_stack_ptr/adj_stack_size.  The actual allocated
	 * memory extends above adj_stack_ptr by up to ~4KB (argument frame + alignment).
	 * MONO_CONTEXT_GET_CURRENT captures the real SP which may land in this
	 * carved-off region, causing is_thread_in_critical_region to think the thread
	 * is on an alternate stack.  Add a 4KB guard to cover the frame area. */
	*staddr -= *stsize;
	*stsize += 4096;
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
