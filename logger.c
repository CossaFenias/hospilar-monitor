#include "logger.h"
#include "synchronization.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static FILE *log_file = NULL;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void log_init(const char *filename) {
    log_file = fopen(filename, "a");
    if (!log_file) {
        perror("fopen log");
        exit(EXIT_FAILURE);
    }
    setvbuf(log_file, NULL, _IOLBF, 0); // line buffered
}

void log_event(const char *tag, const char *message) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_buf[64];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);

    MUTEX_LOCK(&log_mutex);
    fprintf(log_file, "[%s] [%s] %s\n", time_buf, tag, message);
    fflush(log_file);
    MUTEX_UNLOCK(&log_mutex);
}

void log_close(void) {
    if (log_file) fclose(log_file);
}