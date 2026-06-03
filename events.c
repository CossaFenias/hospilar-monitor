#include "events.h"
#include "synchronization.h"
#include <stdlib.h>
#include <string.h>

static void swap(AlarmEvent *a, AlarmEvent *b) {
    AlarmEvent tmp = *a;
    *a = *b;
    *b = tmp;
}

static void heapify_up(AlarmEvent *heap, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (heap[parent].priority <= heap[idx].priority)
            break;
        swap(&heap[parent], &heap[idx]);
        idx = parent;
    }
}

static void heapify_down(AlarmEvent *heap, int size, int idx) {
    while (1) {
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        int smallest = idx;

        if (left < size && heap[left].priority < heap[smallest].priority)
            smallest = left;
        if (right < size && heap[right].priority < heap[smallest].priority)
            smallest = right;
        if (smallest == idx)
            break;
        swap(&heap[idx], &heap[smallest]);
        idx = smallest;
    }
}

void event_queue_init(EventQueue *q, int capacity) {
    q->heap = malloc(sizeof(AlarmEvent) * capacity);
    q->capacity = capacity;
    q->size = 0;
    MUTEX_INIT(&q->mutex);
    COND_INIT(&q->cond);
}

void event_queue_destroy(EventQueue *q) {
    free(q->heap);
    MUTEX_DESTROY(&q->mutex);
    COND_DESTROY(&q->cond);
}

void event_queue_push(EventQueue *q, AlarmEvent ev) {
    MUTEX_LOCK(&q->mutex);
    if (q->size == q->capacity) {
        // Expand capacity (simple reallocation)
        q->capacity *= 2;
        q->heap = realloc(q->heap, sizeof(AlarmEvent) * q->capacity);
    }
    q->heap[q->size] = ev;
    heapify_up(q->heap, q->size);
    q->size++;
    COND_SIGNAL(&q->cond);
    MUTEX_UNLOCK(&q->mutex);
}

AlarmEvent event_queue_pop(EventQueue *q) {
    MUTEX_LOCK(&q->mutex);
    while (q->size == 0) {
        COND_WAIT(&q->cond, &q->mutex);
    }
    AlarmEvent top = q->heap[0];
    q->size--;
    if (q->size > 0) {
        q->heap[0] = q->heap[q->size];
        heapify_down(q->heap, q->size, 0);
    }
    MUTEX_UNLOCK(&q->mutex);
    return top;
}

int event_queue_empty(EventQueue *q) {
    MUTEX_LOCK(&q->mutex);
    int empty = (q->size == 0);
    MUTEX_UNLOCK(&q->mutex);
    return empty;
}