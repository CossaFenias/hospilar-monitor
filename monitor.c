#include "monitor.h"
#include <stdio.h>
#include <unistd.h>

void* monitor_thread(void* arg) {
    while (system_running) {
        // Wait for signal from sensors (eliminates busy waiting)
        pthread_mutex_lock(&gui_mutex);
        pthread_cond_wait(&monitor_cv, &gui_mutex);
        pthread_mutex_unlock(&gui_mutex);

        // Monitor can perform aggregate analysis here
        // e.g., checking if multiple patients are in emergency, triggering hospital-wide alert
        // For this architecture, sensors directly enqueue, but monitor validates system health.
        
        int emergency_count = 0;
        for (int i = 0; i < MAX_PATIENTS; i++) {
            pthread_mutex_lock(&patients[i].lock);
            if (patients[i].status == STATUS_EMERGENCY) {
                emergency_count++;
            }
            pthread_mutex_unlock(&patients[i].lock);
        }

        if (emergency_count >= 2) {
            log_event("SYSTEM", "MULTIPLE EMERGENCIES DETECTED");
        }
        
        usleep(100000); // Prevent monitor from spinning too fast if signaled repeatedly
    }
    return NULL;
}