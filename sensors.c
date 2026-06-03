#include "sensors.h"
#include "logger.h"
#include "synchronization.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

static SensorQueue sensor_q;
static volatile int running = 1;

void sensor_queue_init(SensorQueue *q) {
    q->in = q->out = q->count = 0;
    MUTEX_INIT(&q->mutex);
    SEM_INIT(&q->empty, 0, SENSOR_QUEUE_SIZE);
    SEM_INIT(&q->full, 0, 0);
}

void sensor_queue_destroy(SensorQueue *q) {
    MUTEX_DESTROY(&q->mutex);
    SEM_DESTROY(&q->empty);
    SEM_DESTROY(&q->full);
}

static volatile int running = 1;

void set_running_flag(int val) {
    running = val;
}

SensorQueue* get_sensor_queue(void) {
    return &sensor_q;
}

void sensors_start(pthread_t *threads) {
    sensor_queue_init(&sensor_q);
    for (long i = 0; i < N_SENSORS; i++) {
        pthread_create(&threads[i], NULL, sensor_thread, (void*)i);
    }
}

void sensors_join(pthread_t *threads) {
    for (int i = 0; i < N_SENSORS; i++) {
        pthread_join(threads[i], NULL);
    }
    sensor_queue_destroy(&sensor_q);
}

void sensor_queue_push(SensorQueue *q, SensorData data) {
    SEM_WAIT(&q->empty);
    MUTEX_LOCK(&q->mutex);
    q->buffer[q->in] = data;
    q->in = (q->in + 1) % SENSOR_QUEUE_SIZE;
    q->count++;
    MUTEX_UNLOCK(&q->mutex);
    SEM_POST(&q->full);
}

SensorData sensor_queue_pop(SensorQueue *q) {
    SEM_WAIT(&q->full);
    MUTEX_LOCK(&q->mutex);
    SensorData data = q->buffer[q->out];
    q->out = (q->out + 1) % SENSOR_QUEUE_SIZE;
    q->count--;
    MUTEX_UNLOCK(&q->mutex);
    SEM_POST(&q->empty);
    return data;
}

static int rand_range(int min, int max) {
    return min + rand() % (max - min + 1);
}

void *sensor_thread(void *arg) {
    long id = (long)arg;
    char log_buf[128];
    snprintf(log_buf, sizeof(log_buf), "Sensor %ld started", id);
    log_event("SENSOR", log_buf);

    while (running) {
        SensorData data;
        data.patient_id = rand_range(1, 3);   // 3 patients
        data.heart_rate = rand_range(50, 150);
        data.temperature = rand_range(360, 410) / 10.0f; // 36.0..41.0
        data.spo2 = rand_range(80, 100);
        data.bp_systolic = rand_range(90, 160);
        data.bp_diastolic = rand_range(60, 100);

        sensor_queue_push(&sensor_q, data);

        snprintf(log_buf, sizeof(log_buf),
                 "Patient %d: HR=%d, T=%.1f, SpO2=%d, BP=%d/%d",
                 data.patient_id, data.heart_rate, data.temperature,
                 data.spo2, data.bp_systolic, data.bp_diastolic);
        log_event("SENSOR_DATA", log_buf);

        usleep(200000 + rand() % 300000); // 0.2-0.5 sec
    }
    log_event("SENSOR", "exiting");
    return NULL;
}

void sensors_start(pthread_t *threads) {
    sensor_queue_init(&sensor_q);
    for (long i = 0; i < N_SENSORS; i++) {
        pthread_create(&threads[i], NULL, sensor_thread, (void*)i);
    }
}

void sensors_join(pthread_t *threads) {
    for (int i = 0; i < N_SENSORS; i++) {
        pthread_join(threads[i], NULL);
    }
    sensor_queue_destroy(&sensor_q);
}

// Make sensor queue accessible to monitor thread
SensorQueue* get_sensor_queue(void) {
    return &sensor_q;
}

void set_running_flag(int val) {
    running = val;
}