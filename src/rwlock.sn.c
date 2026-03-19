/* =============================================================================
 * sindarin-pkg-threads/rwlock.sn.c — Fair Reader-Writer Lock
 * =============================================================================
 * Fair RwLock using mutex + condition variables.
 *
 * Fairness: when a writer arrives it sets reader_count negative, blocking new
 * readers. Existing active readers continue. Once they drain the writer runs.
 * After the writer releases, all blocked readers are woken before the next
 * writer can acquire.
 * ============================================================================= */

#include "threads_internal.h"

typedef __sn__ReaderWriterLock RtReaderWriterLock;

RtReaderWriterLock *sn_rwlock_new(void)
{
    RtReaderWriterLock *lock = (RtReaderWriterLock *)calloc(1, sizeof(RtReaderWriterLock));
    if (!lock) { fprintf(stderr, "sn_rwlock_new: out of memory\n"); exit(1); }

    ReaderWriterLockInternal *internal = (ReaderWriterLockInternal *)calloc(1, sizeof(ReaderWriterLockInternal));
    if (!internal) { fprintf(stderr, "sn_rwlock_new: out of memory\n"); exit(1); }

    pthread_mutex_init(&internal->mutex,        NULL);
    pthread_cond_init (&internal->writer_cond,  NULL);
    pthread_cond_init (&internal->reader_cond,  NULL);
    pthread_mutex_init(&internal->writer_mutex, NULL);
    internal->reader_count = 0;
    internal->reader_wait  = 0;

    lock->internal = (long long)(uintptr_t)internal;
    return lock;
}

void sn_rwlock_read_lock(RtReaderWriterLock *self)
{
    ReaderWriterLockInternal *rw = (ReaderWriterLockInternal *)(uintptr_t)self->internal;
    pthread_mutex_lock(&rw->mutex);
    rw->reader_count++;
    if (rw->reader_count <= 0)
        /* reader_count is negative — a writer is pending, block */
        pthread_cond_wait(&rw->reader_cond, &rw->mutex);
    pthread_mutex_unlock(&rw->mutex);
}

void sn_rwlock_read_unlock(RtReaderWriterLock *self)
{
    ReaderWriterLockInternal *rw = (ReaderWriterLockInternal *)(uintptr_t)self->internal;
    pthread_mutex_lock(&rw->mutex);
    rw->reader_count--;
    if (rw->reader_count < 0) {
        /* writer is pending — decrement the count it is waiting for */
        rw->reader_wait--;
        if (rw->reader_wait == 0)
            pthread_cond_signal(&rw->writer_cond);
    }
    pthread_mutex_unlock(&rw->mutex);
}

void sn_rwlock_write_lock(RtReaderWriterLock *self)
{
    ReaderWriterLockInternal *rw = (ReaderWriterLockInternal *)(uintptr_t)self->internal;
    pthread_mutex_lock(&rw->writer_mutex);

    pthread_mutex_lock(&rw->mutex);
    rw->reader_count -= SN_RW_MAX_READERS; /* go negative — blocks new readers */
    int active = rw->reader_count + SN_RW_MAX_READERS;
    rw->reader_wait += active;
    if (rw->reader_wait != 0)
        pthread_cond_wait(&rw->writer_cond, &rw->mutex);
    pthread_mutex_unlock(&rw->mutex);
}

void sn_rwlock_write_unlock(RtReaderWriterLock *self)
{
    ReaderWriterLockInternal *rw = (ReaderWriterLockInternal *)(uintptr_t)self->internal;
    pthread_mutex_lock(&rw->mutex);
    rw->reader_count += SN_RW_MAX_READERS; /* restore to positive */
    int blocked = rw->reader_count;
    for (int i = 0; i < blocked; i++)
        pthread_cond_signal(&rw->reader_cond);
    pthread_mutex_unlock(&rw->mutex);

    pthread_mutex_unlock(&rw->writer_mutex);
}

void sn_rwlock_dispose(RtReaderWriterLock *self)
{
    if (!self->internal) return;
    ReaderWriterLockInternal *rw = (ReaderWriterLockInternal *)(uintptr_t)self->internal;
    pthread_mutex_destroy(&rw->mutex);
    pthread_cond_destroy (&rw->writer_cond);
    pthread_cond_destroy (&rw->reader_cond);
    pthread_mutex_destroy(&rw->writer_mutex);
    free(rw);
    self->internal = 0;
}
