#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "synchronization.h"
#include "sensors.h"
#include "monitor.h"
#include "alarms.h"
#include "gui.h"
#include "logger.h"

int main() {
    printf("Initializing Hospital Monitoring System...\n");
    
    // 1. Initialize Synchronization Primitives
    sync_init();
    log_event("SYSTEM", "System initialized");

    // 2. Initialize GUI
    init_gui();

    // 3. Create Threads
    pthread_t gui_tid;
    pthread_t monitor_tid;
    pthread_t alarm_tid;
    pthread_t sensor_tids[MAX_PATIENTS];
    int patient_ids[MAX_PATIENTS];

    pthread_create(&gui_tid, NULL, gui_thread, NULL);
    pthread_create(&monitor_tid, NULL, monitor_thread, NULL);
    pthread_create(&alarm_tid, NULL, alarm_thread, NULL);

    for (int i = 0; i < MAX_PATIENTS; i++) {
        patient_ids[i] = i + 1;
        pthread_create(&sensor_tids[i], NULL, sensor_thread, &patient_ids[i]);
    }

    log_event("SYSTEM", "All threads started");

    // 4. Wait for GUI to signal shutdown (via 'q' key)
    pthread_join(gui_tid, NULL);
    
    // 5. Graceful Shutdown
    system_running = false;
    log_event("SYSTEM", "Shutdown initiated");

    // Wake up sleeping threads so they can check system_running and exit
    pthread_cond_broadcast(&monitor_cv);
    
    // Inject dummy events to wake up blocked semaphore consumers if any
    for(int i=0; i<MAX_PATIENTS; i++) {
        sem_post(&event_queue.full); 
    }

    for (int i = 0; i < MAX_PATIENTS; i++) {
        pthread_join(sensor_tids[i], NULL);
    }
    pthread_join(monitor_tid, NULL);
    pthread_join(alarm_tid, NULL);

    // 6. Cleanup
    cleanup_gui();
    sync_destroy();
    log_event("SYSTEM", "System terminated cleanly");
    printf("System stopped. Check hospital.log for details.\n");

    return 0;
}