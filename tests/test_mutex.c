#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include "sn_minimal.h"

/* Struct: Mutex (native) */
typedef struct {
    long long internal;
} __sn__Mutex;

static inline __sn__Mutex *__sn__Mutex_alloc(void) {
    return calloc(1, sizeof(__sn__Mutex));
}

void sn_mutex_dispose(__sn__Mutex *);
static inline void __sn__Mutex_release(__sn__Mutex **p) {
    if (*p) {
        sn_mutex_dispose(*p);
        free(*p);
    }
    *p = NULL;
}

#define sn_auto_Mutex __attribute__((cleanup(__sn__Mutex_release)))
#define sn_auto_ref_Mutex __attribute__((cleanup(__sn__Mutex_release)))


long long __sn__counter = 0LL;
pthread_mutex_t __sn__counter_mutex = PTHREAD_MUTEX_INITIALIZER;

long long __sn__incr(__sn__Mutex *);
__sn__Mutex * __sn__Mutex_new();
void sn_mutex_lock(__sn__Mutex *);
void sn_mutex_unlock(__sn__Mutex *);
bool sn_mutex_try_lock(__sn__Mutex *);
void __sn__Mutex_withLock(__sn__Mutex *, void *);
bool __sn__Mutex_tryWithLock(__sn__Mutex *, void *);
void sn_mutex_dispose(__sn__Mutex *);
#include "mutex.sn.c"

typedef struct {
    __sn__Mutex * arg0;
    int _padding;
} __ThreadArgs_0__;

static void *__thread_wrapper_0__(void *arg) {
    SnThread *__th__ = (SnThread *)arg;
    __ThreadArgs_0__ *args = (__ThreadArgs_0__ *)__th__->result;
    __ThreadArgs_0__ __args_copy__ = *args;
    args = &__args_copy__;

    long long __result__ = __sn__incr(args->arg0);
    free(__th__->result); __th__->result = NULL;
    if (!__th__->result) __th__->result = calloc(1, sizeof(long long));
    *(long long *)__th__->result = __result__;
    return NULL;
}

typedef struct {
    __sn__Mutex * arg0;
    int _padding;
} __ThreadArgs_1__;

static void *__thread_wrapper_1__(void *arg) {
    SnThread *__th__ = (SnThread *)arg;
    __ThreadArgs_1__ *args = (__ThreadArgs_1__ *)__th__->result;
    __ThreadArgs_1__ __args_copy__ = *args;
    args = &__args_copy__;

    long long __result__ = __sn__incr(args->arg0);
    free(__th__->result); __th__->result = NULL;
    if (!__th__->result) __th__->result = calloc(1, sizeof(long long));
    *(long long *)__th__->result = __result__;
    return NULL;
}

typedef struct {
    __sn__Mutex * arg0;
    int _padding;
} __ThreadArgs_2__;

static void *__thread_wrapper_2__(void *arg) {
    SnThread *__th__ = (SnThread *)arg;
    __ThreadArgs_2__ *args = (__ThreadArgs_2__ *)__th__->result;
    __ThreadArgs_2__ __args_copy__ = *args;
    args = &__args_copy__;

    long long __result__ = __sn__incr(args->arg0);
    free(__th__->result); __th__->result = NULL;
    if (!__th__->result) __th__->result = calloc(1, sizeof(long long));
    *(long long *)__th__->result = __result__;
    return NULL;
}

typedef struct {
    __sn__Mutex * arg0;
    int _padding;
} __ThreadArgs_3__;

static void *__thread_wrapper_3__(void *arg) {
    SnThread *__th__ = (SnThread *)arg;
    __ThreadArgs_3__ *args = (__ThreadArgs_3__ *)__th__->result;
    __ThreadArgs_3__ __args_copy__ = *args;
    args = &__args_copy__;

    long long __result__ = __sn__incr(args->arg0);
    free(__th__->result); __th__->result = NULL;
    if (!__th__->result) __th__->result = calloc(1, sizeof(long long));
    *(long long *)__th__->result = __result__;
    return NULL;
}

typedef struct {
    __sn__Mutex * arg0;
    int _padding;
} __ThreadArgs_4__;

static void *__thread_wrapper_4__(void *arg) {
    SnThread *__th__ = (SnThread *)arg;
    __ThreadArgs_4__ *args = (__ThreadArgs_4__ *)__th__->result;
    __ThreadArgs_4__ __args_copy__ = *args;
    args = &__args_copy__;

    long long __result__ = __sn__incr(args->arg0);
    free(__th__->result); __th__->result = NULL;
    if (!__th__->result) __th__->result = calloc(1, sizeof(long long));
    *(long long *)__th__->result = __result__;
    return NULL;
}
typedef struct __Closure__ {
    void *fn;
    size_t size;
    void (*__cleanup__)(void *);
} __Closure__;

static void __lambda_0__(void *__closure__);

typedef struct __closure_1__ {
    void *fn;
    size_t size;
    void (*__cleanup__)(void *);
    bool *acquired;
} __closure_1__;
static void __closure_1_free__(void *p) {
    __closure_1__ *cl = (__closure_1__ *)p;
    free(cl->acquired);
    free(cl);
}
static void __closure_1_cleanup__(void **p) {
    if (*p) {
        free(*p);
    }
    *p = NULL;
}
#define sn_auto_closure_1 __attribute__((cleanup(__closure_1_cleanup__)))
static void __lambda_1__(void *__closure__);

typedef struct __closure_2__ {
    void *fn;
    size_t size;
    void (*__cleanup__)(void *);
    bool *acquired;
} __closure_2__;
static void __closure_2_free__(void *p) {
    __closure_2__ *cl = (__closure_2__ *)p;
    free(cl->acquired);
    free(cl);
}
static void __closure_2_cleanup__(void **p) {
    if (*p) {
        free(*p);
    }
    *p = NULL;
}
#define sn_auto_closure_2 __attribute__((cleanup(__closure_2_cleanup__)))
static void __lambda_2__(void *__closure__);


long long __sn__incr(__sn__Mutex * __sn__m) {

    __sn__Mutex_withLock(__sn__m, ({
        __Closure__ *__cl__ = malloc(sizeof(__Closure__));
        __cl__->fn = (void *)__lambda_0__;
        __cl__->size = sizeof(__Closure__);
        __cl__->__cleanup__ = NULL;
        __cl__;
    }));
    

    return 1LL;}


__sn__Mutex * __sn__Mutex_new() {

    return sn_mutex_new();}

void __sn__Mutex_withLock(__sn__Mutex *__sn__self, void * __sn__body) {

    sn_mutex_lock(__sn__self);
    

    ((void (*)(void *))((__Closure__ *)__sn__body)->fn)(__sn__body);
    

    sn_mutex_unlock(__sn__self);
    
}

bool __sn__Mutex_tryWithLock(__sn__Mutex *__sn__self, void * __sn__body) {

    if (sn_mutex_try_lock(__sn__self)) {
        ((void (*)(void *))((__Closure__ *)__sn__body)->fn)(__sn__body);
        
        sn_mutex_unlock(__sn__self);
        
        return true;}

    return false;}

int main() {
    sn_auto_Mutex __sn__Mutex * __sn__m = __sn__Mutex_new();
    sn_mutex_lock(__sn__m);
    
    sn_mutex_unlock(__sn__m);
    
    long long __sn__t1 = 0; sn_auto_thread SnThread * __sn__t1__th__ = ({
        SnThread *__th__ = sn_thread_create();
        __ThreadArgs_0__ *__args__ = malloc(sizeof(__ThreadArgs_0__));
        __args__->arg0 = __sn__m;
        __th__->result = __args__;
        __th__->result_size = sizeof(long long);
        pthread_create(&__th__->thread, NULL, __thread_wrapper_0__, __th__);
        __th__;
    });
    long long __sn__t2 = 0; sn_auto_thread SnThread * __sn__t2__th__ = ({
        SnThread *__th__ = sn_thread_create();
        __ThreadArgs_1__ *__args__ = malloc(sizeof(__ThreadArgs_1__));
        __args__->arg0 = __sn__m;
        __th__->result = __args__;
        __th__->result_size = sizeof(long long);
        pthread_create(&__th__->thread, NULL, __thread_wrapper_1__, __th__);
        __th__;
    });
    long long __sn__t3 = 0; sn_auto_thread SnThread * __sn__t3__th__ = ({
        SnThread *__th__ = sn_thread_create();
        __ThreadArgs_2__ *__args__ = malloc(sizeof(__ThreadArgs_2__));
        __args__->arg0 = __sn__m;
        __th__->result = __args__;
        __th__->result_size = sizeof(long long);
        pthread_create(&__th__->thread, NULL, __thread_wrapper_2__, __th__);
        __th__;
    });
    long long __sn__t4 = 0; sn_auto_thread SnThread * __sn__t4__th__ = ({
        SnThread *__th__ = sn_thread_create();
        __ThreadArgs_3__ *__args__ = malloc(sizeof(__ThreadArgs_3__));
        __args__->arg0 = __sn__m;
        __th__->result = __args__;
        __th__->result_size = sizeof(long long);
        pthread_create(&__th__->thread, NULL, __thread_wrapper_3__, __th__);
        __th__;
    });
    long long __sn__t5 = 0; sn_auto_thread SnThread * __sn__t5__th__ = ({
        SnThread *__th__ = sn_thread_create();
        __ThreadArgs_4__ *__args__ = malloc(sizeof(__ThreadArgs_4__));
        __args__->arg0 = __sn__m;
        __th__->result = __args__;
        __th__->result_size = sizeof(long long);
        pthread_create(&__th__->thread, NULL, __thread_wrapper_4__, __th__);
        __th__;
    });
    ({
        { sn_auto_thread SnThread *__sync_th__ = __sn__t1__th__; __sn__t1__th__ = NULL;
        if (__sync_th__) { sn_thread_join(__sync_th__); __sn__t1 = *(long long *)__sync_th__->result; } }
        { sn_auto_thread SnThread *__sync_th__ = __sn__t2__th__; __sn__t2__th__ = NULL;
        if (__sync_th__) { sn_thread_join(__sync_th__); __sn__t2 = *(long long *)__sync_th__->result; } }
        { sn_auto_thread SnThread *__sync_th__ = __sn__t3__th__; __sn__t3__th__ = NULL;
        if (__sync_th__) { sn_thread_join(__sync_th__); __sn__t3 = *(long long *)__sync_th__->result; } }
        { sn_auto_thread SnThread *__sync_th__ = __sn__t4__th__; __sn__t4__th__ = NULL;
        if (__sync_th__) { sn_thread_join(__sync_th__); __sn__t4 = *(long long *)__sync_th__->result; } }
        { sn_auto_thread SnThread *__sync_th__ = __sn__t5__th__; __sn__t5__th__ = NULL;
        if (__sync_th__) { sn_thread_join(__sync_th__); __sn__t5 = *(long long *)__sync_th__->result; } }
        (void)0; })
    ;
    
    if ((__sn__counter != 5LL)) {
        { sn_auto_str char *__ps__ = ({
                char __is_buf__[1024];
                int __is_off__ = 0;
                __is_buf__[0] = '\0';
                __is_off__ += snprintf(__is_buf__ + __is_off__, sizeof(__is_buf__) - __is_off__, "%s", "FAIL: expected counter=5, got ");
                __is_off__ += snprintf(__is_buf__ + __is_off__, sizeof(__is_buf__) - __is_off__, "%lld", (long long)(__sn__counter));
                __is_off__ += snprintf(__is_buf__ + __is_off__, sizeof(__is_buf__) - __is_off__, "%s", "\n");
                strdup(__is_buf__);
            }); sn_print(__ps__); };
        
        sn_exit(1LL);
        
    }
    sn_mutex_lock(__sn__m);
    
    sn_auto_capture bool *__sn__acquired = malloc(sizeof(bool)); *__sn__acquired = false;
    __sn__Mutex_tryWithLock(__sn__m, ({
        __closure_1__ *__cl__ = malloc(sizeof(__closure_1__));
        __cl__->fn = (void *)__lambda_1__;
        __cl__->size = sizeof(__closure_1__);
        __cl__->__cleanup__ = NULL;
        __cl__->acquired = __sn__acquired;
        __cl__;
    }));
    
    if ((*__sn__acquired)) {
        sn_print("FAIL: tryWithLock should not acquire a held lock\n");
        
        sn_exit(1LL);
        
    }
    sn_mutex_unlock(__sn__m);
    
    __sn__Mutex_tryWithLock(__sn__m, ({
        __closure_2__ *__cl__ = malloc(sizeof(__closure_2__));
        __cl__->fn = (void *)__lambda_2__;
        __cl__->size = sizeof(__closure_2__);
        __cl__->__cleanup__ = NULL;
        __cl__->acquired = __sn__acquired;
        __cl__;
    }));
    
    if ((!(*__sn__acquired))) {
        sn_print("FAIL: tryWithLock should acquire a free lock\n");
        
        sn_exit(1LL);
        
    }
    sn_mutex_dispose(__sn__m);
    
    sn_print("PASS\n");
    
    fflush(stdout);
    return 0;
}

static void __lambda_0__(void *__closure__) {

    __sn__counter = __sn__counter + 1LL;
    
}

static void __lambda_1__(void *__closure__) {

    bool *__sn__acquired = ((__closure_1__ *)__closure__)->acquired;

    (*__sn__acquired = true);
    
}

static void __lambda_2__(void *__closure__) {

    bool *__sn__acquired = ((__closure_2__ *)__closure__)->acquired;

    (*__sn__acquired = true);
    
}
