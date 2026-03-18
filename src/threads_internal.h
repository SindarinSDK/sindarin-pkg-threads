/*
 * sindarin-pkg-threads — Internal C structures and helpers
 *
 * Included by every .sn.c file in this package. These files are compiled
 * by the Sindarin compiler with:
 *   - sn_types.h force-included  (Sindarin-generated struct definitions)
 *   - runtime headers on the path (sn_thread.h pulls in pthread or compat layer)
 */
#ifndef THREADS_INTERNAL_H
#define THREADS_INTERNAL_H

#include "sn_thread.h"   /* pthread / compat_pthread, SnThread helpers */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>

/* ============================================================================
 * Internal state structures — one per synchronization type.
 * Each Sindarin native struct stores a uintptr_t (cast to long long) in its
 * _internal field pointing to one of these heap-allocated structs.
 * ============================================================================ */

typedef struct {
    pthread_mutex_t mutex;
} MutexInternal;

typedef struct {
    pthread_cond_t cond;
} ConditionInternal;

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    int             count;
} SemaphoreInternal;

#define SN_RW_MAX_READERS (1 << 30)

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t  writer_cond;  /* writer waits for readers to drain */
    pthread_cond_t  reader_cond;  /* readers wait when a writer is pending */
    int             reader_count; /* positive = active readers, negative = writer pending */
    int             reader_wait;  /* readers the current writer is waiting for */
    pthread_mutex_t writer_mutex; /* serializes writers */
} ReaderWriteLockInternal;

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    int             count;
} WaitGroupInternal;

/* ---- Thread pool work queue ---- */

typedef struct WorkItem {
    void            *closure;  /* heap copy of the Sindarin closure */
    struct WorkItem *next;
} WorkItem;

typedef struct {
    pthread_mutex_t  mutex;
    pthread_cond_t   work_cond;    /* workers wait for items */
    pthread_cond_t   idle_cond;    /* shutdown waits for drain */
    WorkItem        *head;
    WorkItem        *tail;
    int              pending;      /* items in queue + currently executing */
    int              shutdown;
    int              worker_count;
    pthread_t       *workers;
} ThreadPoolInternal;

/* ---- Thread scope spawned-thread list ---- */

typedef struct SpawnedThread {
    pthread_t             thread;
    struct SpawnedThread *next;
} SpawnedThread;

typedef struct {
    pthread_mutex_t  mutex;
    SpawnedThread   *threads;
} ThreadScopeInternal;

/* ============================================================================
 * Closure calling convention helpers
 *
 * Sindarin closure layout (from sn_core.h):
 *   [0                          ]  void *fn       — function pointer
 *   [sizeof(void *)             ]  size_t size    — total closure bytes
 *   [sizeof(void *)+sizeof(sz) ]  void *cleanup  — cleanup function (or NULL)
 *   [remainder                  ]  captured variables
 * ============================================================================ */

/*
 * Shallow-copy a closure to the heap.
 * The cleanup pointer is zeroed in the copy — the thread owns the raw bytes
 * only and simply free()s them after the function returns. Captured objects
 * (arrays, structs, etc.) are shared with the parent scope; the user must
 * ensure they remain alive until the thread finishes (e.g. via join()).
 */
static inline void *sn_closure_copy(void *closure)
{
    size_t size = *(size_t *)((char *)closure + sizeof(void *));
    void *copy = malloc(size);
    if (!copy) { fprintf(stderr, "sn_closure_copy: out of memory\n"); exit(1); }
    memcpy(copy, closure, size);
    /* Zero the cleanup pointer so the thread does not double-free captures */
    void **cleanup = (void **)((char *)copy + sizeof(void *) + sizeof(size_t));
    *cleanup = NULL;
    return copy;
}

/* Call a fn(): void closure. */
static inline void sn_closure_call_void(void *closure)
{
    typedef void (*VoidFn)(void *);
    ((VoidFn)*(void **)closure)(closure);
}

/* Call a fn(): bool closure and return the result. */
static inline bool sn_closure_call_bool(void *closure)
{
    typedef bool (*BoolFn)(void *);
    return ((BoolFn)*(void **)closure)(closure);
}

#endif /* THREADS_INTERNAL_H */
