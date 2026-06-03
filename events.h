#ifndef EVENTS_H
#define EVENTS_H

#include <time.h>
#include "synchronization.h"

typedef struct {
    int priority;        // 1=Emergency, 2=Urgent, 3=Normal
    int patient_id;
    time_t timestamp;
    char message[256];
} AlarmEvent;

// Priority queue (max-heap by lower priority number)
typedef struct {
    AlarmEvent *heap;
    int capacity;
    int size;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} EventQueue;

void event_queue_init(EventQueue *q, int capacity);
void event_queue_destroy(EventQueue *q);
void event_queue_push(EventQueue *q, AlarmEvent ev);
AlarmEvent event_queue_pop(EventQueue *q);
int event_queue_empty(EventQueue *q);

#endif
