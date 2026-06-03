#ifndef ALARMS_H
#define ALARMS_H

#include <pthread.h>
#include "events.h"

void alarm_start(pthread_t *thread, EventQueue *eq);
void alarm_join(pthread_t thread);

#endif