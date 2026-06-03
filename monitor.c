#include "monitor.h"
#include "sensors.h"
#include "events.h"
#include "logger.h"
#include "synchronization.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static volatile int running = 1;

void monitor_set_running(int val) {
    running = val;
}

void patient_records_init(PatientRecord *records) {
    for (int i = 0; i < N_PATIENTS; i++) {
        records[i].patient_id = i + 1;
        records[i].last_heart_rate = 70;
        records[i].last_temperature = 36.5;
        records[i].last_spo2 = 98;
        records[i].last_bp_systolic = 120;
        records[i].last_bp_diastolic = 80;
        MUTEX_INIT(&records[i].mutex);
    }
}

static int compute_priority(const SensorData *data) {
    // Emergency (Priority 1)
    if (data->heart_rate > 150 || data->heart_rate < 30 ||
        data->temperature > 40.0 ||
        data->spo2 < 85 ||
        data->bp_systolic > 180 || data->bp_systolic < 80)
        return 1;
    // Urgent (Priority 2) – thresholds from spec
    if (data->heart_rate > 120 || data->heart_rate < 40 ||
        data->temperature > 39.0 ||
        data->spo2 < 90 ||
        data->bp_systolic > 140 || data->bp_systolic < 90 ||
        data->bp_diastolic > 90)
        return 2;
    // Normal – but we only generate events if threshold violated
    return 3;
}

static void check_and_raise_alarm(PatientRecord *records, const SensorData *data, EventQueue *eq) {
    int priority = compute_priority(data);
    if (priority <= 2) {  // Only generate events for violations (priority 1 or 2)
        AlarmEvent ev;
        ev.priority = priority;
        ev.patient_id = data->patient_id;
        ev.timestamp = time(NULL);
        snprintf(ev.message, sizeof(ev.message),
                 "Patient %d: HR=%d, T=%.1f, SpO2=%d, BP=%d/%d [%s]",
                 data->patient_id, data->heart_rate, data->temperature,
                 data->spo2, data->bp_systolic, data->bp_diastolic,
                 (priority == 1) ? "EMERGENCY" : "URGENT");
        event_queue_push(eq, ev);

        char log_buf[256];
        snprintf(log_buf, sizeof(log_buf), "Alarm queued: %s", ev.message);
        log_event("MONITOR", log_buf);
    }
}

void *monitor_thread(void *arg) {
    struct {
        PatientRecord *records;
        EventQueue *eq;
    } *params = (void*)arg;

    log_event("MONITOR", "started");
    SensorQueue *sq = get_sensor_queue();

    while (running) {
        SensorData data = sensor_queue_pop(sq);

        // Update patient record
        PatientRecord *rec = &params->records[data.patient_id - 1];
        MUTEX_LOCK(&rec->mutex);
        rec->last_heart_rate = data.heart_rate;
        rec->last_temperature = data.temperature;
        rec->last_spo2 = data.spo2;
        rec->last_bp_systolic = data.bp_systolic;
        rec->last_bp_diastolic = data.bp_diastolic;
        MUTEX_UNLOCK(&rec->mutex);

        // Evaluate and possibly raise alarm
        check_and_raise_alarm(params->records, &data, params->eq);
    }
    log_event("MONITOR", "exiting");
    return NULL;
}

void monitor_start(pthread_t *thread, PatientRecord *records, EventQueue *eq) {
    static struct {
        PatientRecord *records;
        EventQueue *eq;
    } params;
    params.records = records;
    params.eq = eq;
    pthread_create(thread, NULL, monitor_thread, &params);
}

void monitor_join(pthread_t thread) {
    pthread_join(thread, NULL);
}

void monitor_set_running(int val) {
    running = val;
}