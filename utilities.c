// Updated utilities.c with improved ncurses visualization and logging
#include "utilities.h"
#include <time.h>

pthread_mutex_t screen_mutex = PTHREAD_MUTEX_INITIALIZER;
float total_revenue = 0;
int log_count = 0;
char log_messages[MAX_LOGS][200];

void log_event(const char *message) {
    pthread_mutex_lock(&screen_mutex);
    char timestamp[20];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", t);

    if (log_count < MAX_LOGS) {
        snprintf(log_messages[log_count], sizeof(log_messages[log_count]), "%s %s", timestamp, message);
        log_count++;
    } else {
        for (int i = 1; i < MAX_LOGS; i++) {
            strncpy(log_messages[i - 1], log_messages[i], sizeof(log_messages[i - 1]));
        }
        snprintf(log_messages[MAX_LOGS - 1], sizeof(log_messages[MAX_LOGS - 1]), "%s %s", timestamp, message);
    }
    pthread_mutex_unlock(&screen_mutex);
}

void update_table_status(int table_index, int occupied, int capacity, const char *status) {
    pthread_mutex_lock(&screen_mutex);
    mvprintw(table_index + 1, 0, "[%d] %s: %d/%d", table_index + 1, status, occupied, capacity);
    refresh();
    pthread_mutex_unlock(&screen_mutex);
}

void update_order_status(int table_index, const char *status) {
    pthread_mutex_lock(&screen_mutex);
    mvprintw(table_index + 1, 30, "Stolik %d: %s", table_index + 1, status);
    refresh();
    pthread_mutex_unlock(&screen_mutex);
}

void update_revenue(float amount) {
    pthread_mutex_lock(&screen_mutex);
    total_revenue += amount;
    mvprintw(MAX_TABLES + 5, 0, "DOCHÓD: %.2f PLN", total_revenue);
    refresh();
    pthread_mutex_unlock(&screen_mutex);
}

void init_ncurses() {
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE);
    draw_interface();
}

void end_ncurses() {
    clear();
    refresh();
    endwin();
}

void draw_interface() {
    Table *tables = shmat(shm_id, NULL, 0);
    if (tables == (void *)-1) {
        perror("[Utilities] Błąd dołączania pamięci dzielonej w draw_interface");
        return;
    }

    clear();

    // Sekcja STOLIKI
    mvprintw(0, 0, "STOLIKI:");
    for (int i = 0; i < MAX_TABLES; i++) {
        mvprintw(i + 1, 0, "[%d] %s: %d/%d", 
            i + 1, 
            tables[i].occupied > 0 ? "Zajęty" : "Wolny", 
            tables[i].occupied, 
            tables[i].capacity
        );
        mvprintw(i + 1, 30, "Status: %s", tables[i].order_status);
    }

    // Sekcja ZAMÓWIENIA
    mvprintw(MAX_TABLES + 2, 0, "ZAMÓWIENIA:");
    for (int i = 0; i < log_count && i < MAX_LOGS; i++) {
        mvprintw(MAX_TABLES + 3 + i, 0, "%s", log_messages[i]);
    }

    // Sekcja DOCHÓD
    mvprintw(MAX_TABLES + MAX_LOGS + 4, 0, "DOCHÓD: %.2f PLN", total_revenue);

    refresh();
    shmdt(tables);
}

void sem_lock(int sem_id) {
    struct sembuf op = {0, -1, 0};
    if (semop(sem_id, &op, 1) == -1) {
        perror("[Semafor] Błąd blokowania");
        exit(EXIT_FAILURE);
    }
}

void sem_unlock(int sem_id) {
    struct sembuf op = {0, 1, 0};
    if (semop(sem_id, &op, 1) == -1) {
        perror("[Semafor] Błąd odblokowywania");
        exit(EXIT_FAILURE);
    }
}