#ifndef EVENTS_H
#define EVENTS_H
#include "synchronization.h"
#include <time.h>

void get_timestamp(char* buffer);
void enqueue_event(Event* event);
bool dequeue_event(Event* event);
#endif