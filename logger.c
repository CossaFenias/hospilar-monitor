#include "logger.h"
#include "events.h"  // Added this line to resolve the implicit declaration error
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

// Simple thread-safe logging using a dedicated mutex
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void log_event(const char* level, const char* message) {
    pthread_mutex_lock(&log_mutex);
    char timestamp[32];
    get_timestamp(timestamp);
    
    FILE* f = fopen("hospital.log", "a");
    if (f) {
        fprintf(f, "[%s] [%s] %s\n", timestamp, level, message);
        fclose(f);
    }
    pthread_mutex_unlock(&log_mutex);
}