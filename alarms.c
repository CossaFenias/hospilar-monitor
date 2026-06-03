#include "alarms.h"
#include <stdio.h>
#include <string.h>

void* alarm_thread(void* arg) {
    Event ev;
    while (system_running) {
        // Consumer: blocks here until an event is available (sem_wait)
        if (dequeue_event(&ev)) {
            const char* prio_str = (ev.priority == PRIORITY_EMERGENCY) ? "!!! EMERGENCY !!!" : "!! URGENT !!";
            char log_msg[256];
            sprintf(log_msg, "ALARM TRIGGERED - Patient %d: %s | %s", ev.patient_id, prio_str, ev.description);
            
            log_event("ALARM", log_msg);
            
            // Simulate alarm handling time (e.g., notifying nurses)
            usleep(200000); 
        }
    }
    return NULL;
}