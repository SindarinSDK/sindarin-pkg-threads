/* sindarin-pkg-threads/semaphore.sn.c — Counting semaphore (mutex + condvar) */

#include "threads_internal.h"

typedef __sn__Semaphore RtSemaphore;

RtSemaphore *sn_semaphore_new(long long initial)
{
    RtSemaphore *s = __sn__Semaphore__new();

    SemaphoreInternal *internal = (SemaphoreInternal *)calloc(1, sizeof(SemaphoreInternal));
    if (!internal) { fprintf(stderr, "sn_semaphore_new: out of memory\n"); exit(1); }

    pthread_mutex_init(&internal->mutex, NULL);
    pthread_cond_init(&internal->cond, NULL);
    internal->count = (int)initial;
    s->internal = (long long)(uintptr_t)internal;
    return s;
}

void sn_semaphore_wait(RtSemaphore *self)
{
    SemaphoreInternal *s = (SemaphoreInternal *)(uintptr_t)self->internal;
    pthread_mutex_lock(&s->mutex);
    while (s->count <= 0)
        pthread_cond_wait(&s->cond, &s->mutex);
    s->count--;
    pthread_mutex_unlock(&s->mutex);
}

void sn_semaphore_post(RtSemaphore *self)
{
    SemaphoreInternal *s = (SemaphoreInternal *)(uintptr_t)self->internal;
    pthread_mutex_lock(&s->mutex);
    s->count++;
    pthread_cond_signal(&s->cond);
    pthread_mutex_unlock(&s->mutex);
}

bool sn_semaphore_try_wait(RtSemaphore *self)
{
    SemaphoreInternal *s = (SemaphoreInternal *)(uintptr_t)self->internal;
    pthread_mutex_lock(&s->mutex);
    bool acquired = s->count > 0;
    if (acquired) s->count--;
    pthread_mutex_unlock(&s->mutex);
    return acquired;
}

void sn_semaphore_dispose(RtSemaphore *self)
{
    if (!self->internal) return;
    SemaphoreInternal *s = (SemaphoreInternal *)(uintptr_t)self->internal;
    pthread_mutex_destroy(&s->mutex);
    pthread_cond_destroy(&s->cond);
    free(s);
    self->internal = 0;
}
