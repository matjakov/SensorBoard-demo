/*\brief Thread Class
*/
#ifndef THREAD_H_
#define THREAD_H_

#include <pthread.h>
#include <signal.h>
/**
    Runs a procedure independently until it ends, its parent process dies or is canceled.
*/
class Thread
{
    public:
        Thread(void *(*start_routine)(void*), void *arg = NULL, size_t stacksize = 4096);
        virtual ~Thread();
        bool IsValid();
        int Cancel();
        int Detach();
        int Join(void **value = NULL);
        int TryJoin(void **value = NULL);
        bool IsCurrent();
        static void Exit(void *value = NULL);
        static void Sleep(int msec);
        static void Once(void (*init_routine)(void));

    protected:
        bool valid;
        pthread_t thread;
};


#endif
