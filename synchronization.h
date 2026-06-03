#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

/* Wrapper for mutex operations with error checking */
#define MUTEX_LOCK(m) do { \
    if (pthread_mutex_lock(m) != 0) { \
        perror("pthread_mutex_lock"); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

#define MUTEX_UNLOCK(m) do { \
    if (pthread_mutex_unlock(m) != 0) { \
        perror("pthread_mutex_unlock"); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

#define MUTEX_INIT(m) do { \
    if (pthread_mutex_init(m, NULL) != 0) { \
        perror("pthread_mutex_init"); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

#define MUTEX_DESTROY(m) pthread_mutex_destroy(m)

/* Condition variable wrappers */
#define COND_WAIT(c, m) do { \
    if (pthread_cond_wait(c, m) != 0) { \
        perror("pthread_cond_wait"); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

#define COND_SIGNAL(c) do { \
    if (pthread_cond_signal(c) != 0) { \
        perror("pthread_cond_signal"); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

#define COND_BROADCAST(c) do { \
    if (pthread_cond_broadcast(c) != 0) { \
        perror("pthread_cond_broadcast"); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

#define COND_INIT(c) do { \
    if (pthread_cond_init(c, NULL) != 0) { \
        perror("pthread_cond_init"); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

#define COND_DESTROY(c) pthread_cond_destroy(c)

/* Semaphore wrappers */
#define SEM_WAIT(s) do { \
    if (sem_wait(s) != 0) { \
        perror("sem_wait"); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

#define SEM_POST(s) do { \
    if (sem_post(s) != 0) { \
        perror("sem_post"); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

#define SEM_INIT(s, pshared, value) do { \
    if (sem_init(s, pshared, value) != 0) { \
        perror("sem_init"); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

#define SEM_DESTROY(s) sem_destroy(s)

#endif