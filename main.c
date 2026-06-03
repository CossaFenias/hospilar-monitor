#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "sensors.h"
#include "monitor.h"
#include "alarms.h"
#include "logger.h"
#include "events.h"

static volatile int running = 1;
static EventQueue event_queue;
static PatientRecord patients[N_PATIENTS];
static pthread_t sensor_threads[N_SENSORS];
static pthread_t monitor_thr, alarm_thr;

void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        running = 0;
        // wake up threads waiting on semaphores/condvars
        // For semaphores, we need to post to break wait; simpler: let them finish after queue empties
        // but we can also set a global flag and force posts. For brevity, we just set flag.
        // The monitor thread will exit after consuming any pending data.
        // The alarm thread will exit after processing remaining events.
        log_event("MAIN", "Shutdown signal received");
    }
}

int main(void) {
    srand(time(NULL));
    log_init("hospital.log");
    log_event("MAIN", "Hospital Monitoring System started");

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Initialize data structures
    patient_records_init(patients);
    event_queue_init(&event_queue, 10);

    // Start threads
    sensors_start(sensor_threads);
    monitor_start(&monitor_thr, patients, &event_queue);
    alarm_start(&alarm_thr, &event_queue);

    // Let the system run for 30 seconds (or until Ctrl+C)
    printf("Hospital Monitor running. Press Ctrl+C to stop.\n");
    int remaining = 30;
    while (running && remaining > 0) {
        sleep(1);
        remaining--;
        if (remaining == 0) running = 0;
    }

    // Shutdown
    set_running_flag(0);         // sensors
    monitor_set_running(0);
    alarm_set_running(0);

    // Wake up any blocked threads by posting dummy semaphores (simplified)
    // Actually, the queues may block. We'll force unblock by pushing dummy events.
    // For sensor queue: we can push a dummy with patient 0 as sentinel, but ignore.
    // For event queue: push a dummy event with priority 3 to unblock alarm.
    // Simpler: we rely on the fact that after running=0, loops will exit when queue becomes empty.
    // To avoid deadlock on sem_wait, we can post to empty/full. We'll do that for robustness.
    SensorQueue *sq = get_sensor_queue();
    sem_post(&sq->full); // wake monitor if waiting on empty? Actually monitor waits on full.
    // Actually monitor waits on full in sensor_queue_pop. Post one extra to break.
    // But careful: it may cause extra pop. We'll just join after small delay.
    usleep(500000);

    sensors_join(sensor_threads);
    monitor_join(monitor_thr);
    alarm_join(alarm_thr);

    event_queue_destroy(&event_queue);
    log_close();

    printf("System stopped.\n");
    return 0;
}