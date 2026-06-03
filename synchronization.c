#include "synchronization.h"
#include <stdio.h>
#include <string.h>

Patient patients[MAX_PATIENTS];
EventQueue event_queue;
pthread_mutex_t gui_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t monitor_cv = PTHREAD_COND_INITIALIZER;
bool system_running = true;

void sync_init() {
    // Initialize patients
    const char* names[] = {"John Doe", "Jane Smith", "Bob Johnson", "Alice Brown", "Charlie Davis"};
    for (int i = 0; i < MAX_PATIENTS; i++) {
        patients[i].id = i + 1;
        strncpy(patients[i].name, names[i], sizeof(patients[i].name));
        patients[i].heart_rate = 75;
        patients[i].temperature = 36.6;
        patients[i].oxygen_sat = 98;
        patients[i].bp_systolic = 120;
        patients[i].bp_diastolic = 80;
        patients[i].status = STATUS_NORMAL;
        pthread_mutex_init(&patients[i].lock, NULL);
    }

    // Initialize Queue
    event_queue.head = 0;
    event_queue.tail = 0;
    event_queue.count = 0;
    pthread_mutex_init(&event_queue.mutex, NULL);
    sem_init(&event_queue.empty, 0, MAX_EVENTS);
    sem_init(&event_queue.full, 0, 0);
}

void sync_destroy() {
    for (int i = 0; i < MAX_PATIENTS; i++) {
        pthread_mutex_destroy(&patients[i].lock);
    }
    pthread_mutex_destroy(&event_queue.mutex);
    pthread_mutex_destroy(&gui_mutex);
    pthread_cond_destroy(&monitor_cv);
    sem_destroy(&event_queue.empty);
    sem_destroy(&event_queue.full);
}