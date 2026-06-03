#include "events.h"
#include <stdio.h>
#include <string.h>

void get_timestamp(char* buffer) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(buffer, 32, "%H:%M:%S", t);
}

void enqueue_event(Event* event) {
    // Producer: Wait for empty slot
    sem_wait(&event_queue.empty);
    
    pthread_mutex_lock(&event_queue.mutex);
    event_queue.events[event_queue.tail] = *event;
    event_queue.tail = (event_queue.tail + 1) % MAX_EVENTS;
    event_queue.count++;
    pthread_mutex_unlock(&event_queue.mutex);
    
    // Signal consumer
    sem_post(&event_queue.full);
}

bool dequeue_event(Event* event) {
    // Consumer: Wait for full slot
    sem_wait(&event_queue.full);
    
    pthread_mutex_lock(&event_queue.mutex);
    *event = event_queue.events[event_queue.head];
    event_queue.head = (event_queue.head + 1) % MAX_EVENTS;
    event_queue.count--;
    pthread_mutex_unlock(&event_queue.mutex);
    
    // Signal producer
    sem_post(&event_queue.empty);
    return true;
}