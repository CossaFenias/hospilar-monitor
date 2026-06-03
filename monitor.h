#ifndef MONITOR_H
#define MONITOR_H

#include <pthread.h>
#include "events.h"

#define N_PATIENTS 3

typedef struct {
    int patient_id;
    int last_heart_rate;
    float last_temperature;
    int last_spo2;
    int last_bp_systolic;
    int last_bp_diastolic;
    pthread_mutex_t mutex;
} PatientRecord;

void patient_records_init(PatientRecord *records);
void monitor_start(pthread_t *thread, PatientRecord *records, EventQueue *eq);
void monitor_join(pthread_t thread);

#endif