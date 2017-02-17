/**
    \brief pthread classes Thread implementation
    \author M.Kovac
*/
#include <Thread.h>
#include <stdio.h>
#ifndef _WIN32
#include <unistd.h>
#endif


/**
    Creates a (joinable) thread.
    \param start_routine Global function or a static class member, must take void* parameter
    \param arg Argument passed to start_routine
*/
Thread::Thread(void *(*start_routine)(void*), void *arg, size_t stacksize):
    valid(false)
{
    // POSIX draft recommends that threads be created joinable (in case they are joined)
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setstacksize(&attr, stacksize);
    valid = !pthread_create(&thread, &attr, start_routine, arg);
    pthread_attr_destroy(&attr);

}


Thread::~Thread()
{
    Detach();
    Cancel();
}


/**
    True if usable.
*/
bool Thread::IsValid()
{
    return valid;
}


/**
 * Sends a cancellation request.
 * http://man7.org/linux/man-pages/man3/pthread_cancel.3.html
 */
int Thread::Cancel()
{
    return pthread_cancel(thread);
}

/**
 * Sets the cancelability state of the calling thread.
 * http://man7.org/linux/man-pages/man3/pthread_setcancelstate.3.html
 */
int Thread::EnableCancel(bool enable)
{
    return pthread_setcancelstate(enable ? PTHREAD_CANCEL_ENABLE : PTHREAD_CANCEL_DISABLE, NULL);
}

/**
 * Waits for the thread to terminate.
 * http://man7.org/linux/man-pages/man3/pthread_join.3.html
 * \param value Exit value if not NULL.
 */
int Thread::Join(void **value)
{
    return pthread_join(thread, value);
}

/**
 * Performs a nonblocking join.
 */
int Thread::TryJoin(void **value)
{
    return pthread_tryjoin_np(thread, value);
}


/**
 * When a detached thread terminates, its resources 
 * are automatically released back to the system without the need 
 * for another thread to join with the terminated thread.
 */
int Thread::Detach()
{
    return pthread_detach(thread);
}


/**
    Checks if this thread is the currently running thread.
    Useful when multiple threads share the same procedure.
*/
bool Thread::IsCurrent()
{
    return pthread_equal(pthread_self(), thread);
}



/**
    Terminates the calling thread.
*/
void Thread::Exit(void *value)
{
    pthread_exit(value);
}


/**
    Suspends execution of the calling thread.
*/
void Thread::Sleep(int msec)
{
#ifdef WIN32
    ::Sleep(msec);
#else
    usleep(msec * 1000);
#endif
}

/**
    Runs init_routine exactly once in the entire program.
    Subsequent calls are ignored.
*/
void Thread::Once( void (*init_routine)(void))
{
    static pthread_once_t initialized = PTHREAD_ONCE_INIT;
    pthread_once(&initialized, init_routine);
}


