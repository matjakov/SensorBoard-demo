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
    Locks when created, unlocks when destroyed. Handy for protecting entire functions.
*/
class AutoLock
{
    public:
        AutoLock(Mutex *locker): lock(locker) {};
        ~AutoLock() { lock->Unlock(); }
    private:
        Mutex *lock;
};



/**
    Spin locks are meant for protecting _simple_ operations.
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
    Returns false if already locked.
*/
bool SpinLock::TryLock()
{
    return !pthread_spin_trylock(&spinlock);
}


bool SpinLock::Unlock()
{
    return !pthread_spin_unlock(&spinlock);
}


#endif // PLock_H
