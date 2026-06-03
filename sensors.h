#ifndef SENSORS_H
#define SENSORS_H

#include <pthread.h>
#include <semaphore.h>

#define N_SENSORS 3
#define SENSOR_QUEUE_SIZE 10

typedef struct {
    int patient_id;
    int heart_rate;
    float temperature;
    int spo2;
    int bp_systolic;
    int bp_diastolic;
} SensorData;

typedef struct {
    SensorData buffer[SENSOR_QUEUE_SIZE];
    int in, out;
    int count;
    pthread_mutex_t mutex;
    sem_t empty;
    sem_t full;
} SensorQueue;

void sensor_queue_init(SensorQueue *q);
void sensor_queue_destroy(SensorQueue *q);
void sensor_queue_push(SensorQueue *q, SensorData data);
SensorData sensor_queue_pop(SensorQueue *q);

void sensors_start(pthread_t *threads);
void sensors_join(pthread_t *threads);
void set_running_flag(int val);
SensorQueue* get_sensor_queue(void);

void *sensor_thread(void *arg);

#endif