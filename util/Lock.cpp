#include <Lock.h>


/**
 * Creates a new unlocked mutex.
 */
Mutex::Mutex()
{
    pthread_mutex_init(&mutex, NULL);
}


Mutex::~Mutex()
{
    pthread_mutex_destroy(&mutex);
}


/**
 * Blocks if locked by another thread.
 */
bool Mutex::Lock()
{
    return !pthread_mutex_lock(&mutex);
}


/**
    Non-blocking Lock().
    \return false if already locked.
*/
bool Mutex::TryLock()
{
    return !pthread_mutex_trylock(&mutex);
}


/**
 * Releases the lock.
 */
bool Mutex::Unlock()
{
    return !pthread_mutex_unlock(&mutex);
}





SpinLock::SpinLock(bool shared)
{
    pthread_spin_init(&spinlock, shared ? PTHREAD_PROCESS_SHARED : PTHREAD_PROCESS_PRIVATE);
}


SpinLock::~SpinLock()
{
    pthread_spin_destroy(&spinlock);
}




