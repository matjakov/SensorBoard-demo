/**
 * POSIX Threads - locking
 */
#ifndef PLOCK_H_
#define PLOCK_H_

#include <pthread.h>

/**
    Protects access to a shared resource with a mutex variable.
*/
class Mutex
{
    public:
        Mutex();
        virtual ~Mutex();
        bool Lock();
        bool TryLock();
        bool Unlock();

    protected:
        pthread_mutex_t mutex;
};



/**
    Spin locks are meant for implementing _simple_ atomic operations.
    Unlock as quickly as possible, because CPU time is being wasted while locked.
*/
class SpinLock
{
    public:
        SpinLock(bool shared = false);
        virtual ~SpinLock();
        inline bool Lock();
        inline bool TryLock();
        inline bool Unlock();

    protected:
        pthread_spinlock_t spinlock;
};



/**
    Some compilers require inlines be visible at the point of declaration.
*/



/**
    Blocks if locked by another thread.
    This involves busy-waiting so you should Unlock() ASAP.
*/
bool SpinLock::Lock()
{
    return !pthread_spin_lock(&spinlock);
}


/**
    Non-blocking Lock().
    \return false if already locked.
*/
bool SpinLock::TryLock()
{
    return !pthread_spin_trylock(&spinlock);
}


/**
    Releases the lock.
*/
bool SpinLock::Unlock()
{
    return !pthread_spin_unlock(&spinlock);
}




/**
    Locks when created, unlocks when destroyed.
*/
 template<typename T> class AutoLock
{
    public:
        AutoLock(T& lck): lock(lck)
        {
            lock.Lock();
        };
        ~AutoLock()
        {
            lock.Unlock();
        }
    private:
        T& lock;
};


#endif // PLOCK_H
