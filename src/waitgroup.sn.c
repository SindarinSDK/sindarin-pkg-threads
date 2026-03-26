/* sindarin-pkg-threads/waitgroup.sn.c — WaitGroup (mutex + condvar + counter) */

#include "threads_internal.h"

typedef __sn__WaitGroup RtWaitGroup;

RtWaitGroup *sn_waitgroup_new(void)
{
    RtWaitGroup *wg = __sn__WaitGroup__new();

    WaitGroupInternal *internal = (WaitGroupInternal *)calloc(1, sizeof(WaitGroupInternal));
    if (!internal) { fprintf(stderr, "sn_waitgroup_new: out of memory\n"); exit(1); }

    pthread_mutex_init(&internal->mutex, NULL);
    pthread_cond_init (&internal->cond,  NULL);
    internal->count = 0;
    wg->internal = (long long)(uintptr_t)internal;
    return wg;
}

void sn_waitgroup_add(RtWaitGroup *self, long long n)
{
    WaitGroupInternal *wg = (WaitGroupInternal *)(uintptr_t)self->internal;
    pthread_mutex_lock(&wg->mutex);
    wg->count += (int)n;
    if (wg->count <= 0)
        pthread_cond_broadcast(&wg->cond);
    pthread_mutex_unlock(&wg->mutex);
}

void sn_waitgroup_done(RtWaitGroup *self)
{
    WaitGroupInternal *wg = (WaitGroupInternal *)(uintptr_t)self->internal;
    pthread_mutex_lock(&wg->mutex);
    wg->count--;
    if (wg->count <= 0)
        pthread_cond_broadcast(&wg->cond);
    pthread_mutex_unlock(&wg->mutex);
}

void sn_waitgroup_wait(RtWaitGroup *self)
{
    WaitGroupInternal *wg = (WaitGroupInternal *)(uintptr_t)self->internal;
    pthread_mutex_lock(&wg->mutex);
    while (wg->count > 0)
        pthread_cond_wait(&wg->cond, &wg->mutex);
    pthread_mutex_unlock(&wg->mutex);
}

void sn_waitgroup_dispose(RtWaitGroup *self)
{
    if (!self->internal) return;
    WaitGroupInternal *wg = (WaitGroupInternal *)(uintptr_t)self->internal;
    pthread_mutex_destroy(&wg->mutex);
    pthread_cond_destroy (&wg->cond);
    free(wg);
    self->internal = 0;
}
