#include "gui.h"
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void init_gui() {
    initscr();
    cbreak();
    noecho();
    curs_set(0); // Hide cursor
    nodelay(stdscr, TRUE); // Non-blocking input
    keypad(stdscr, TRUE);

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);  // Normal
        init_pair(2, COLOR_YELLOW, COLOR_BLACK); // Urgent
        init_pair(3, COLOR_RED, COLOR_BLACK);    // Emergency
        init_pair(4, COLOR_CYAN, COLOR_BLACK);   // Dashboard
        init_pair(5, COLOR_WHITE, COLOR_BLUE);   // Header
    }
}

void cleanup_gui() {
    curs_set(1);
    endwin();
}

void* gui_thread(void* arg) {
    int active_alarms = 0;
    int total_events = 0;

    while (system_running) {
        clear();
        
        // Header
        attron(COLOR_PAIR(5) | A_BOLD);
        mvprintw(0, 0, " HOSPITAL MONITORING SYSTEM (Press 'q' to stop) ");
        attroff(COLOR_PAIR(5) | A_BOLD);

        // Dashboard
        attron(COLOR_PAIR(4));
        mvprintw(2, 0, "DASHBOARD: Patients: %d | System Status: %s", MAX_PATIENTS, system_running ? "RUNNING" : "STOPPED");
        attroff(COLOR_PAIR(4));

        // Patient Table Header
        mvprintw(4, 0, "ID | Name           | HR  | Temp | SpO2 | BP      | Status");
        mvprintw(5, 0, "-------------------------------------------------------------------");

        // Read Patient Data (Thread-Safe Copy)
        for (int i = 0; i < MAX_PATIENTS; i++) {
            Patient p_copy;
            pthread_mutex_lock(&patients[i].lock);
            p_copy = patients[i]; // Atomic copy of struct
            pthread_mutex_unlock(&patients[i].lock);

            int color_pair = 1; // Green (Normal)
            char* status_str = "NORMAL";
            if (p_copy.status == STATUS_URGENT) { color_pair = 2; status_str = "URGENT"; }
            if (p_copy.status == STATUS_EMERGENCY) { color_pair = 3; status_str = "EMERGENCY"; }

            attron(COLOR_PAIR(color_pair) | A_BOLD);
            mvprintw(6 + i, 0, "%-2d | %-14s | %-3d | %.1f | %-4d | %-3d/%-3d | %s",
                     p_copy.id, p_copy.name, p_copy.heart_rate, p_copy.temperature,
                     p_copy.oxygen_sat, p_copy.bp_systolic, p_copy.bp_diastolic, status_str);
            attroff(COLOR_PAIR(color_pair) | A_BOLD);
        }

        // Active Alarms Panel (Simulated by reading queue count)
        pthread_mutex_lock(&event_queue.mutex);
        int queue_size = event_queue.count;
        pthread_mutex_unlock(&event_queue.mutex);

        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(13, 0, "ACTIVE ALARMS IN QUEUE: %d", queue_size);
        attroff(COLOR_PAIR(3) | A_BOLD);

        // Footer instructions
        mvprintw(22, 0, "Legend: [Green] Normal  [Yellow] Urgent  [Red] Emergency");
        
        refresh();

        // Check for user input to stop system
        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            system_running = false;
            break;
        }

        usleep(250000); // Update GUI at ~4 FPS
    }
    return NULL;
}