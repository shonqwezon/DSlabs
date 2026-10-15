#ifndef _MY_RWLOCK_H_
#define _MY_RWLOCK_H_

#include <pthread.h>

typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t readers_cond;
    pthread_cond_t writers_cond;

    int readers;
    int waiting_readers;
    int waiting_writers;
    int writer_active;
} my_rwlock_t;

void my_rwlock_init(my_rwlock_t *lock);
void my_rwlock_destroy(my_rwlock_t *lock);
void my_rwlock_rdlock(my_rwlock_t *lock);
void my_rwlock_wrlock(my_rwlock_t *lock);
void my_rwlock_unlock(my_rwlock_t *lock);

#endif
