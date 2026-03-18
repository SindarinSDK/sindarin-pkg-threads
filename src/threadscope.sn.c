/* sindarin-pkg-threads/threadscope.sn.c — Scoped thread spawning */

#include "threads_internal.h"

typedef __sn__ThreadScope RtThreadScope;

/* ---- Internal thread runner ---- */

static void *threadscope_runner(void *arg)
{
    void *closure = arg;
    sn_closure_call_void(closure);
    free(closure);  /* free the shallow copy; captures are owned by parent scope */
    return NULL;
}

/* ---- Public API ---- */

static RtThreadScope *threadscope_new(void)
{
    RtThreadScope *scope = (RtThreadScope *)calloc(1, sizeof(RtThreadScope));
    if (!scope) { fprintf(stderr, "threadscope_new: out of memory\n"); exit(1); }

    ThreadScopeInternal *internal = (ThreadScopeInternal *)calloc(1, sizeof(ThreadScopeInternal));
    if (!internal) { fprintf(stderr, "threadscope_new: out of memory\n"); exit(1); }

    pthread_mutex_init(&internal->mutex, NULL);
    internal->threads = NULL;
    scope->internal = (long long)(uintptr_t)internal;
    return scope;
}

static void threadscope_join_all(RtThreadScope *self)
{
    ThreadScopeInternal *scope = (ThreadScopeInternal *)(uintptr_t)self->internal;
    /* Body has returned — no more spawns can happen, so no lock needed. */
    SpawnedThread *st = scope->threads;
    while (st) {
        pthread_join(st->thread, NULL);
        SpawnedThread *next = st->next;
        free(st);
        st = next;
    }
    scope->threads = NULL;
}

/*
 * sn_threadscope_run — called from the Sindarin static fn run().
 *
 * body is a fn(ThreadScope): void closure. Calling convention:
 *   void (*fn)(void *self, RtThreadScope *scope)
 * where self is the closure pointer.
 */
void sn_threadscope_run(void *body)
{
    RtThreadScope *scope = threadscope_new();

    typedef void (*BodyFn)(void *, RtThreadScope *);
    BodyFn fn = *(BodyFn *)body;
    fn(body, scope);

    threadscope_join_all(scope);
    sn_threadscope_dispose(scope);
}

void sn_threadscope_spawn(RtThreadScope *self, void *task)
{
    ThreadScopeInternal *scope = (ThreadScopeInternal *)(uintptr_t)self->internal;

    void *closure_copy = sn_closure_copy(task);

    SpawnedThread *st = (SpawnedThread *)calloc(1, sizeof(SpawnedThread));
    if (!st) { fprintf(stderr, "sn_threadscope_spawn: out of memory\n"); exit(1); }

    pthread_mutex_lock(&scope->mutex);
    st->next       = scope->threads;
    scope->threads = st;
    pthread_mutex_unlock(&scope->mutex);

    if (pthread_create(&st->thread, NULL, threadscope_runner, closure_copy) != 0) {
        fprintf(stderr, "sn_threadscope_spawn: pthread_create failed\n");
        exit(1);
    }
}

void sn_threadscope_dispose(RtThreadScope *self)
{
    if (!self->internal) return;
    ThreadScopeInternal *scope = (ThreadScopeInternal *)(uintptr_t)self->internal;
    pthread_mutex_destroy(&scope->mutex);
    free(scope);
    self->internal = 0;
}
