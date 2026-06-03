#include "sensors.h"
#include "events.h"   // Added for get_timestamp and enqueue_event
#include "logger.h"   // Added for log_event
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void* sensor_thread(void* arg) {
    int patient_id = *(int*)arg;
    Patient* p = &patients[patient_id - 1];

    while (system_running) {
        // Simulate sensor reading delay (0.5 to 1.0 seconds)
        usleep(500000 + rand() % 500000); 

        // Generate realistic but occasionally anomalous data
        int hr = 60 + rand() % 80; // 60-140
        if (rand() % 20 == 0) hr = 130 + rand() % 20; // Spike
        
        float temp = 36.0 + (rand() % 40) / 10.0; // 36.0 - 39.9
        int spo2 = 95 + rand() % 6; // 95-100
        if (rand() % 25 == 0) spo2 = 85 + rand() % 5; // Drop

        int sys = 110 + rand() % 40; // 110-150
        int dia = 70 + rand() % 20;  // 70-90

        // LOCK: Protect patient data update
        pthread_mutex_lock(&p->lock);
        p->heart_rate = hr;
        p->temperature = temp;
        p->oxygen_sat = spo2;
        p->bp_systolic = sys;
        p->bp_diastolic = dia;

        // Check for anomalies and generate event
        bool anomaly = false;
        Priority prio = PRIORITY_NORMAL;
        char desc[128] = "";

        if (hr > 120 || hr < 40) {
            anomaly = true; prio = PRIORITY_EMERGENCY;
            sprintf(desc, "Critical Heart Rate: %d bpm", hr);
            p->status = STATUS_EMERGENCY;
        } else if (temp > 39.0) {
            anomaly = true; prio = PRIORITY_URGENT;
            sprintf(desc, "High Fever: %.1f C", temp);
            p->status = STATUS_URGENT;
        } else if (spo2 < 90) {
            anomaly = true; prio = PRIORITY_EMERGENCY;
            sprintf(desc, "Hypoxia: SpO2 %d%%", spo2);
            p->status = STATUS_EMERGENCY;
        } else if (sys > 140 || dia > 90 || sys < 90) {
            anomaly = true; prio = PRIORITY_URGENT;
            sprintf(desc, "Abnormal BP: %d/%d", sys, dia);
            p->status = STATUS_URGENT;
        } else {
            p->status = STATUS_NORMAL;
        }

        pthread_mutex_unlock(&p->lock); // UNLOCK immediately to prevent deadlock

        if (anomaly) {
            Event ev;
            ev.patient_id = p->id;
            ev.priority = prio;
            strncpy(ev.description, desc, sizeof(ev.description));
            get_timestamp(ev.timestamp);
            
            enqueue_event(&ev);
            log_event(prio == PRIORITY_EMERGENCY ? "EMERGENCY" : "URGENT", desc);
        }

        // Signal monitor that data has changed
        pthread_mutex_lock(&gui_mutex); 
        pthread_cond_signal(&monitor_cv);
        pthread_mutex_unlock(&gui_mutex);
    }
    return NULL;
}