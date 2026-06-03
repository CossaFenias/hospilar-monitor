#ifndef GUI_H
#define GUI_H
#include "synchronization.h"

void init_gui();
void* gui_thread(void* arg);
void cleanup_gui();
#endif