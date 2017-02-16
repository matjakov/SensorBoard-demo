/**
    \brief pthread classes Thread implementation
    \author M.Kovac
*/
#include <Thread.h>
#include <stdio.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
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
    Cancels this thread immediately or at the next possibility.
*/
int Thread::Cancel()
{
    return pthread_cancel(thread);
}


/**
    Makes the calling thread wait for termination of this thread.
    The exit status of the thread is stored in *value if not NULL.
*/
int Thread::Join(void **value)
{
    return pthread_join(thread, value);
}

int Thread::TryJoin(void **value)
{
    return pthread_tryjoin_np(thread, value);
}

/**
    Indicate that this thread is never to be joined even though it was created as joinable.
    The resources will therefore be freed immediately when it terminates instead of waiting for another thread to perform join on it.
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


