#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#define MAX_PATIENTS 5
#define MAX_EVENTS 50

// Priority Levels
typedef enum { PRIORITY_NORMAL = 3, PRIORITY_URGENT = 2, PRIORITY_EMERGENCY = 1 } Priority;

// Patient Status
typedef enum { STATUS_NORMAL, STATUS_URGENT, STATUS_EMERGENCY } PatientStatus;

// Shared Patient Structure
typedef struct {
    int id;
    char name[32];
    int heart_rate;      // bpm
    float temperature;   // Celsius
    int oxygen_sat;      // %
    int bp_systolic;     // mmHg
    int bp_diastolic;    // mmHg
    PatientStatus status;
    pthread_mutex_t lock; // Protects this specific patient's data
} Patient;

// Event Structure
typedef struct {
    int patient_id;
    Priority priority;
    char description[128];
    char timestamp[32];
} Event;

// Bounded Buffer for Producer-Consumer
typedef struct {
    Event events[MAX_EVENTS];
    int head;
    int tail;
    int count;
    pthread_mutex_t mutex;
    sem_t empty; // Counts empty slots
    sem_t full;  // Counts filled slots
} EventQueue;

// Global Synchronization Objects
extern Patient patients[MAX_PATIENTS];
extern EventQueue event_queue;
extern pthread_mutex_t gui_mutex; // Protects GUI read operations
extern pthread_cond_t monitor_cv; // Wakes up monitor thread
extern bool system_running;

void sync_init();
void sync_destroy();

#endif