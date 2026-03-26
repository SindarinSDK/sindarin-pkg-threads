/* sindarin-pkg-threads/condition.sn.c — Condition variable (pthread_cond_t) */

#include "threads_internal.h"

typedef __sn__Condition RtCondition;
typedef __sn__Mutex     RtMutex;

/* Extract the pthread_mutex_t from a Sindarin Mutex. */
static inline pthread_mutex_t *get_mutex_pthread(RtMutex *mutex)
{
    MutexInternal *m = (MutexInternal *)(uintptr_t)mutex->internal;
    return &m->mutex;
}

RtCondition *sn_condition_new(void)
{
    RtCondition *cv = __sn__Condition__new();

    ConditionInternal *internal = (ConditionInternal *)calloc(1, sizeof(ConditionInternal));
    if (!internal) { fprintf(stderr, "sn_condition_new: out of memory\n"); exit(1); }

    pthread_cond_init(&internal->cond, NULL);
    cv->internal = (long long)(uintptr_t)internal;
    return cv;
}

void sn_condition_wait(RtCondition *self, RtMutex *mutex)
{
    ConditionInternal *cv = (ConditionInternal *)(uintptr_t)self->internal;
    pthread_cond_wait(&cv->cond, get_mutex_pthread(mutex));
}

bool sn_condition_timed_wait(RtCondition *self, RtMutex *mutex, long long ms)
{
    ConditionInternal *cv = (ConditionInternal *)(uintptr_t)self->internal;

    struct timespec deadline;
    timespec_get(&deadline, TIME_UTC);
    deadline.tv_sec  += ms / 1000;
    deadline.tv_nsec += (ms % 1000) * 1000000;
    while (deadline.tv_nsec >= 1000000000) {
        deadline.tv_sec++;
        deadline.tv_nsec -= 1000000000;
    }

    int r = pthread_cond_timedwait(&cv->cond, get_mutex_pthread(mutex), &deadline);
    return r != ETIMEDOUT;
}

void sn_condition_signal(RtCondition *self)
{
    ConditionInternal *cv = (ConditionInternal *)(uintptr_t)self->internal;
    pthread_cond_signal(&cv->cond);
}

void sn_condition_broadcast(RtCondition *self)
{
    ConditionInternal *cv = (ConditionInternal *)(uintptr_t)self->internal;
    pthread_cond_broadcast(&cv->cond);
}

bool sn_condition_timed_wait_until(RtCondition *self, RtMutex *mutex, long long ms, void *predicate)
{
    ConditionInternal *cv   = (ConditionInternal *)(uintptr_t)self->internal;
    pthread_mutex_t   *m    = get_mutex_pthread(mutex);

    /* Compute absolute deadline. */
    struct timespec deadline;
    timespec_get(&deadline, TIME_UTC);
    deadline.tv_sec  += ms / 1000;
    deadline.tv_nsec += (ms % 1000) * 1000000;
    while (deadline.tv_nsec >= 1000000000) {
        deadline.tv_sec++;
        deadline.tv_nsec -= 1000000000;
    }

    while (!sn_closure_call_bool(predicate)) {
        int r = pthread_cond_timedwait(&cv->cond, m, &deadline);
        if (r == ETIMEDOUT)
            return sn_closure_call_bool(predicate); /* one last check */
    }
    return true;
}

void sn_condition_dispose(RtCondition *self)
{
    if (!self->internal) return;
    ConditionInternal *cv = (ConditionInternal *)(uintptr_t)self->internal;
    pthread_cond_destroy(&cv->cond);
    free(cv);
    self->internal = 0;
}
