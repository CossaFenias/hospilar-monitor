#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <time.h>

void log_event(const char *tag, const char *message);
void log_init(const char *filename);
void log_close(void);

#endif