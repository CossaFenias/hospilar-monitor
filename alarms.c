#include "alarms.h"
#include "logger.h"
#include "synchronization.h"
#include <stdio.h>
#include <string.h>  // ADD THIS for strcpy
#include <unistd.h>

static volatile int running = 1;

void alarm_set_running(int val) {
    running = val;
}

void *alarm_thread(void *arg) {
    EventQueue *eq = (EventQueue*)arg;
    log_event("ALARM", "started");

    while (running) {
        AlarmEvent ev = event_queue_pop(eq);  // blocks if empty
        char priority_str[16];
        switch (ev.priority) {
            case 1: strcpy(priority_str, "EMERGENCY"); break;
            case 2: strcpy(priority_str, "URGENT"); break;
            default: strcpy(priority_str, "NORMAL");
        }
        char log_buf[512];
        snprintf(log_buf, sizeof(log_buf),
                 "ALARM TRIGGERED [%s] %s",
                 priority_str, ev.message);
        log_event("ALARM", log_buf);
        printf("\n*** %s ***\n", log_buf);
        // Simulate alarm handling (e.g., activate buzzer, send notification)
        usleep(100000);
    }
    log_event("ALARM", "exiting");
    return NULL;
}

void alarm_start(pthread_t *thread, EventQueue *eq) {
    pthread_create(thread, NULL, alarm_thread, eq);
}

void alarm_join(pthread_t thread) {
    pthread_join(thread, NULL);
}

void alarm_set_running(int val) {
    running = val;
}