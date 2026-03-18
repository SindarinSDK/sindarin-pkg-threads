#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include "sn_minimal.h"

/* Struct: WaitGroup (native) */
typedef struct {
    long long internal;
} __sn__WaitGroup;

static inline __sn__WaitGroup *__sn__WaitGroup_alloc(void) {
    return calloc(1, sizeof(__sn__WaitGroup));
}

void sn_waitgroup_dispose(__sn__WaitGroup *);
static inline void __sn__WaitGroup_release(__sn__WaitGroup **p) {
    if (*p) {
        sn_waitgroup_dispose(*p);
        free(*p);
    }
    *p = NULL;
}

#define sn_auto_WaitGroup __attribute__((cleanup(__sn__WaitGroup_release)))
#define sn_auto_ref_WaitGroup __attribute__((cleanup(__sn__WaitGroup_release)))


long long __sn__wg_counter = 0LL;
pthread_mutex_t __sn__wg_counter_mutex = PTHREAD_MUTEX_INITIALIZER;

long long __sn__wg_total = 0LL;
pthread_mutex_t __sn__wg_total_mutex = PTHREAD_MUTEX_INITIALIZER;

void __sn__wg_worker(__sn__WaitGroup *);
void __sn__wg_batch(__sn__WaitGroup *, long long);
__sn__WaitGroup * __sn__WaitGroup_new();
void sn_waitgroup_add(__sn__WaitGroup *, long long);
void sn_waitgroup_done(__sn__WaitGroup *);
void sn_waitgroup_wait(__sn__WaitGroup *);
void sn_waitgroup_dispose(__sn__WaitGroup *);
#include "waitgroup.sn.c"

typedef struct {
    __sn__WaitGroup * arg0;
    int _padding;
} __ThreadArgs_0__;

static void *__thread_wrapper_0__(void *arg) {
    SnThread *__th__ = (SnThread *)arg;
    __ThreadArgs_0__ *args = (__ThreadArgs_0__ *)__th__->result;
    __ThreadArgs_0__ __args_copy__ = *args;
    args = &__args_copy__;

    __sn__wg_worker(args->arg0);
    free(__th__->result); __th__->result = NULL;
    free(__th__);
    return NULL;
}

typedef struct {
    __sn__WaitGroup * arg0;
    int _padding;
} __ThreadArgs_1__;

static void *__thread_wrapper_1__(void *arg) {
    SnThread *__th__ = (SnThread *)arg;
    __ThreadArgs_1__ *args = (__ThreadArgs_1__ *)__th__->result;
    __ThreadArgs_1__ __args_copy__ = *args;
    args = &__args_copy__;

    __sn__wg_worker(args->arg0);
    free(__th__->result); __th__->result = NULL;
    free(__th__);
    return NULL;
}

typedef struct {
    __sn__WaitGroup * arg0;
    int _padding;
} __ThreadArgs_2__;

static void *__thread_wrapper_2__(void *arg) {
    SnThread *__th__ = (SnThread *)arg;
    __ThreadArgs_2__ *args = (__ThreadArgs_2__ *)__th__->result;
    __ThreadArgs_2__ __args_copy__ = *args;
    args = &__args_copy__;

    __sn__wg_worker(args->arg0);
    free(__th__->result); __th__->result = NULL;
    free(__th__);
    return NULL;
}

typedef struct {
    __sn__WaitGroup * arg0;
    int _padding;
} __ThreadArgs_3__;

static void *__thread_wrapper_3__(void *arg) {
    SnThread *__th__ = (SnThread *)arg;
    __ThreadArgs_3__ *args = (__ThreadArgs_3__ *)__th__->result;
    __ThreadArgs_3__ __args_copy__ = *args;
    args = &__args_copy__;

    __sn__wg_worker(args->arg0);
    free(__th__->result); __th__->result = NULL;
    free(__th__);
    return NULL;
}

typedef struct {
    __sn__WaitGroup * arg0;
    int _padding;
} __ThreadArgs_4__;

static void *__thread_wrapper_4__(void *arg) {
    SnThread *__th__ = (SnThread *)arg;
    __ThreadArgs_4__ *args = (__ThreadArgs_4__ *)__th__->result;
    __ThreadArgs_4__ __args_copy__ = *args;
    args = &__args_copy__;

    __sn__wg_worker(args->arg0);
    free(__th__->result); __th__->result = NULL;
    free(__th__);
    return NULL;
}

typedef struct {
    __sn__WaitGroup * arg0;
    long long arg1;
    int _padding;
} __ThreadArgs_5__;

static void *__thread_wrapper_5__(void *arg) {
    SnThread *__th__ = (SnThread *)arg;
    __ThreadArgs_5__ *args = (__ThreadArgs_5__ *)__th__->result;
    __ThreadArgs_5__ __args_copy__ = *args;
    args = &__args_copy__;

    __sn__wg_batch(args->arg0, args->arg1);
    free(__th__->result); __th__->result = NULL;
    free(__th__);
    return NULL;
}

typedef struct {
    __sn__WaitGroup * arg0;
    long long arg1;
    int _padding;
} __ThreadArgs_6__;

static void *__thread_wrapper_6__(void *arg) {
    SnThread *__th__ = (SnThread *)arg;
    __ThreadArgs_6__ *args = (__ThreadArgs_6__ *)__th__->result;
    __ThreadArgs_6__ __args_copy__ = *args;
    args = &__args_copy__;

    __sn__wg_batch(args->arg0, args->arg1);
    free(__th__->result); __th__->result = NULL;
    free(__th__);
    return NULL;
}

typedef struct {
    __sn__WaitGroup * arg0;
    long long arg1;
    int _padding;
} __ThreadArgs_7__;

static void *__thread_wrapper_7__(void *arg) {
    SnThread *__th__ = (SnThread *)arg;
    __ThreadArgs_7__ *args = (__ThreadArgs_7__ *)__th__->result;
    __ThreadArgs_7__ __args_copy__ = *args;
    args = &__args_copy__;

    __sn__wg_batch(args->arg0, args->arg1);
    free(__th__->result); __th__->result = NULL;
    free(__th__);
    return NULL;
}
typedef struct __Closure__ {
    void *fn;
    size_t size;
    void (*__cleanup__)(void *);
} __Closure__;


void __sn__wg_worker(__sn__WaitGroup * __sn__wg) {

    __sn__wg_counter = __sn__wg_counter + 1LL;
    

    sn_waitgroup_done(__sn__wg);
    
}


void __sn__wg_batch(__sn__WaitGroup * __sn__wg, long long __sn__n) {

    __sn__wg_total = __sn__wg_total + __sn__n;
    

    sn_waitgroup_done(__sn__wg);
    
}


__sn__WaitGroup * __sn__WaitGroup_new() {

    return sn_waitgroup_new();}

int main() {
    sn_auto_WaitGroup __sn__WaitGroup * __sn__wg = __sn__WaitGroup_new();
    sn_waitgroup_add(__sn__wg, 5LL);
    
    { SnThread *__ff__ = ({
        SnThread *__th__ = sn_thread_create();
        __ThreadArgs_0__ *__args__ = malloc(sizeof(__ThreadArgs_0__));
        __args__->arg0 = __sn__wg;
        __th__->result = __args__;
        __th__->result_size = sizeof(void);
        pthread_create(&__th__->thread, NULL, __thread_wrapper_0__, __th__);
        __th__;
    }); pthread_detach(__ff__->thread); }
    
    { SnThread *__ff__ = ({
        SnThread *__th__ = sn_thread_create();
        __ThreadArgs_1__ *__args__ = malloc(sizeof(__ThreadArgs_1__));
        __args__->arg0 = __sn__wg;
        __th__->result = __args__;
        __th__->result_size = sizeof(void);
        pthread_create(&__th__->thread, NULL, __thread_wrapper_1__, __th__);
        __th__;
    }); pthread_detach(__ff__->thread); }
    
    { SnThread *__ff__ = ({
        SnThread *__th__ = sn_thread_create();
        __ThreadArgs_2__ *__args__ = malloc(sizeof(__ThreadArgs_2__));
        __args__->arg0 = __sn__wg;
        __th__->result = __args__;
        __th__->result_size = sizeof(void);
        pthread_create(&__th__->thread, NULL, __thread_wrapper_2__, __th__);
        __th__;
    }); pthread_detach(__ff__->thread); }
    
    { SnThread *__ff__ = ({
        SnThread *__th__ = sn_thread_create();
        __ThreadArgs_3__ *__args__ = malloc(sizeof(__ThreadArgs_3__));
        __args__->arg0 = __sn__wg;
        __th__->result = __args__;
        __th__->result_size = sizeof(void);
        pthread_create(&__th__->thread, NULL, __thread_wrapper_3__, __th__);
        __th__;
    }); pthread_detach(__ff__->thread); }
    
    { SnThread *__ff__ = ({
        SnThread *__th__ = sn_thread_create();
        __ThreadArgs_4__ *__args__ = malloc(sizeof(__ThreadArgs_4__));
        __args__->arg0 = __sn__wg;
        __th__->result = __args__;
        __th__->result_size = sizeof(void);
        pthread_create(&__th__->thread, NULL, __thread_wrapper_4__, __th__);
        __th__;
    }); pthread_detach(__ff__->thread); }
    
    sn_waitgroup_wait(__sn__wg);
    
    if ((__sn__wg_counter != 5LL)) {
        { sn_auto_str char *__ps__ = ({
                char __is_buf__[1024];
                int __is_off__ = 0;
                __is_buf__[0] = '\0';
                __is_off__ += snprintf(__is_buf__ + __is_off__, sizeof(__is_buf__) - __is_off__, "%s", "FAIL: expected counter=5, got ");
                __is_off__ += snprintf(__is_buf__ + __is_off__, sizeof(__is_buf__) - __is_off__, "%lld", (long long)(__sn__wg_counter));
                __is_off__ += snprintf(__is_buf__ + __is_off__, sizeof(__is_buf__) - __is_off__, "%s", "\n");
                strdup(__is_buf__);
            }); sn_print(__ps__); };
        
        sn_exit(1LL);
        
    }
    sn_waitgroup_add(__sn__wg, 3LL);
    
    { SnThread *__ff__ = ({
        SnThread *__th__ = sn_thread_create();
        __ThreadArgs_5__ *__args__ = malloc(sizeof(__ThreadArgs_5__));
        __args__->arg0 = __sn__wg;
        __args__->arg1 = 10LL;
        __th__->result = __args__;
        __th__->result_size = sizeof(void);
        pthread_create(&__th__->thread, NULL, __thread_wrapper_5__, __th__);
        __th__;
    }); pthread_detach(__ff__->thread); }
    
    { SnThread *__ff__ = ({
        SnThread *__th__ = sn_thread_create();
        __ThreadArgs_6__ *__args__ = malloc(sizeof(__ThreadArgs_6__));
        __args__->arg0 = __sn__wg;
        __args__->arg1 = 20LL;
        __th__->result = __args__;
        __th__->result_size = sizeof(void);
        pthread_create(&__th__->thread, NULL, __thread_wrapper_6__, __th__);
        __th__;
    }); pthread_detach(__ff__->thread); }
    
    { SnThread *__ff__ = ({
        SnThread *__th__ = sn_thread_create();
        __ThreadArgs_7__ *__args__ = malloc(sizeof(__ThreadArgs_7__));
        __args__->arg0 = __sn__wg;
        __args__->arg1 = 30LL;
        __th__->result = __args__;
        __th__->result_size = sizeof(void);
        pthread_create(&__th__->thread, NULL, __thread_wrapper_7__, __th__);
        __th__;
    }); pthread_detach(__ff__->thread); }
    
    sn_waitgroup_wait(__sn__wg);
    
    if ((__sn__wg_total != 60LL)) {
        { sn_auto_str char *__ps__ = ({
                char __is_buf__[1024];
                int __is_off__ = 0;
                __is_buf__[0] = '\0';
                __is_off__ += snprintf(__is_buf__ + __is_off__, sizeof(__is_buf__) - __is_off__, "%s", "FAIL: expected total=60, got ");
                __is_off__ += snprintf(__is_buf__ + __is_off__, sizeof(__is_buf__) - __is_off__, "%lld", (long long)(__sn__wg_total));
                __is_off__ += snprintf(__is_buf__ + __is_off__, sizeof(__is_buf__) - __is_off__, "%s", "\n");
                strdup(__is_buf__);
            }); sn_print(__ps__); };
        
        sn_exit(1LL);
        
    }
    sn_waitgroup_dispose(__sn__wg);
    
    sn_print("PASS\n");
    
    fflush(stdout);
    return 0;
}
