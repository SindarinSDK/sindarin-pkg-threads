/* sindarin-pkg-threads/mutex.sn.c — Mutex implementation (pthread_mutex_t) */

#include "threads_internal.h"

typedef __sn__Mutex RtMutex;

RtMutex *sn_mutex_new(void)
{
    RtMutex *m = (RtMutex *)calloc(1, sizeof(RtMutex));
    if (!m) { fprintf(stderr, "sn_mutex_new: out of memory\n"); exit(1); }

    MutexInternal *internal = (MutexInternal *)calloc(1, sizeof(MutexInternal));
    if (!internal) { fprintf(stderr, "sn_mutex_new: out of memory\n"); exit(1); }

    pthread_mutex_init(&internal->mutex, NULL);
    m->internal = (long long)(uintptr_t)internal;
    return m;
}

void sn_mutex_lock(RtMutex *self)
{
    MutexInternal *m = (MutexInternal *)(uintptr_t)self->internal;
    pthread_mutex_lock(&m->mutex);
}

void sn_mutex_unlock(RtMutex *self)
{
    MutexInternal *m = (MutexInternal *)(uintptr_t)self->internal;
    pthread_mutex_unlock(&m->mutex);
}

bool sn_mutex_try_lock(RtMutex *self)
{
    MutexInternal *m = (MutexInternal *)(uintptr_t)self->internal;
    return pthread_mutex_trylock(&m->mutex) == 0;
}

void sn_mutex_dispose(RtMutex *self)
{
    if (!self->internal) return;
    MutexInternal *m = (MutexInternal *)(uintptr_t)self->internal;
    pthread_mutex_destroy(&m->mutex);
    free(m);
    self->internal = 0;
}
