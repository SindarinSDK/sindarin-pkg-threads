/* sindarin-pkg-threads/threadpool.sn.c — Fixed-size thread pool with work queue */

#include "threads_internal.h"

typedef __sn__ThreadPool RtThreadPool;

/* ---- Worker thread ---- */

static void *threadpool_worker(void *arg)
{
    ThreadPoolInternal *pool = (ThreadPoolInternal *)arg;

    for (;;) {
        pthread_mutex_lock(&pool->mutex);

        /* Wait for work or shutdown signal. */
        while (pool->head == NULL && !pool->shutdown)
            pthread_cond_wait(&pool->work_cond, &pool->mutex);

        /* If shutting down and queue is drained, exit. */
        if (pool->head == NULL && pool->shutdown) {
            pthread_mutex_unlock(&pool->mutex);
            break;
        }

        /* Dequeue one item. */
        WorkItem *item = pool->head;
        pool->head = item->next;
        if (pool->head == NULL) pool->tail = NULL;
        pthread_mutex_unlock(&pool->mutex);

        /* Execute the task. */
        sn_closure_call_void(item->closure);
        free(item->closure);
        free(item);

        /* Decrement pending; signal shutdown waiter if queue is now empty. */
        pthread_mutex_lock(&pool->mutex);
        pool->pending--;
        if (pool->pending == 0)
            pthread_cond_broadcast(&pool->idle_cond);
        pthread_mutex_unlock(&pool->mutex);
    }

    return NULL;
}

/* ---- Public API ---- */

RtThreadPool *sn_threadpool_new(long long workers)
{
    RtThreadPool *tp = __sn__ThreadPool__new();

    ThreadPoolInternal *pool = (ThreadPoolInternal *)calloc(1, sizeof(ThreadPoolInternal));
    if (!pool) { fprintf(stderr, "sn_threadpool_new: out of memory\n"); exit(1); }

    pthread_mutex_init(&pool->mutex,     NULL);
    pthread_cond_init (&pool->work_cond, NULL);
    pthread_cond_init (&pool->idle_cond, NULL);
    pool->head         = NULL;
    pool->tail         = NULL;
    pool->pending      = 0;
    pool->shutdown     = 0;
    pool->worker_count = (int)workers;
    pool->workers      = (pthread_t *)calloc((size_t)workers, sizeof(pthread_t));
    if (!pool->workers) { fprintf(stderr, "sn_threadpool_new: out of memory\n"); exit(1); }

    for (int i = 0; i < (int)workers; i++) {
        if (pthread_create(&pool->workers[i], NULL, threadpool_worker, pool) != 0) {
            fprintf(stderr, "sn_threadpool_new: pthread_create failed\n");
            exit(1);
        }
    }

    tp->internal = (long long)(uintptr_t)pool;
    return tp;
}

void sn_threadpool_submit(RtThreadPool *self, void *task)
{
    ThreadPoolInternal *pool = (ThreadPoolInternal *)(uintptr_t)self->internal;

    WorkItem *item = (WorkItem *)calloc(1, sizeof(WorkItem));
    if (!item) { fprintf(stderr, "sn_threadpool_submit: out of memory\n"); exit(1); }
    item->closure = sn_closure_copy(task);
    item->next    = NULL;

    pthread_mutex_lock(&pool->mutex);
    if (pool->tail)
        pool->tail->next = item;
    else
        pool->head = item;
    pool->tail = item;
    pool->pending++;
    pthread_cond_signal(&pool->work_cond);
    pthread_mutex_unlock(&pool->mutex);
}

void sn_threadpool_shutdown(RtThreadPool *self)
{
    ThreadPoolInternal *pool = (ThreadPoolInternal *)(uintptr_t)self->internal;

    pthread_mutex_lock(&pool->mutex);
    /* Wait for all pending tasks to complete. */
    while (pool->pending > 0)
        pthread_cond_wait(&pool->idle_cond, &pool->mutex);
    /* Signal workers to exit. */
    pool->shutdown = 1;
    pthread_cond_broadcast(&pool->work_cond);
    pthread_mutex_unlock(&pool->mutex);

    for (int i = 0; i < pool->worker_count; i++)
        pthread_join(pool->workers[i], NULL);
}

void sn_threadpool_dispose(RtThreadPool *self)
{
    if (!self->internal) return;
    ThreadPoolInternal *pool = (ThreadPoolInternal *)(uintptr_t)self->internal;

    if (!pool->shutdown)
        sn_threadpool_shutdown(self);

    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy (&pool->work_cond);
    pthread_cond_destroy (&pool->idle_cond);
    free(pool->workers);
    free(pool);
    self->internal = 0;
}
