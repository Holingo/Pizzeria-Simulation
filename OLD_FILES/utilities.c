// Updated utilities.c for structured interface with continuous logs
#include "utilities.h"
#include <time.h>

pthread_mutex_t screen_mutex = PTHREAD_MUTEX_INITIALIZER;
float total_revenue = 0;
int log_count = 0;
char log_messages[MAX_LOGS][200];

void log_event(const char *message) {
    pthread_mutex_lock(&screen_mutex);

    // Dodawanie logu do tablicy
    if (log_count < MAX_LOGS) {
        snprintf(log_messages[log_count], sizeof(log_messages[log_count]), "%s", message);
        log_count++;
    } else {
        for (int i = 1; i < MAX_LOGS; i++) {
            strncpy(log_messages[i - 1], log_messages[i], sizeof(log_messages[i - 1]));
        }
        snprintf(log_messages[MAX_LOGS - 1], sizeof(log_messages[MAX_LOGS - 1]), "%s", message);
    }

    // Rysowanie logów w sekcji logów
    for (int i = 0; i < log_count && i < MAX_LOGS; i++) {
        mvprintw(MAX_TABLES + 3 + i, 0, "%s", log_messages[i]);
    }
    refresh(); // Odświeżenie ekranu ncurses

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
    mvprintw(table_index + 1, 30, "Status: %s", status); // Pozycja na ekranie dla statusu
    refresh();
    pthread_mutex_unlock(&screen_mutex);
}

void update_revenue(float amount) {
    pthread_mutex_lock(&screen_mutex);
    total_revenue += amount;
    mvprintw(MAX_TABLES + 2, 0, "DOCHOD: %.2f PLN", total_revenue);
    refresh();
    pthread_mutex_unlock(&screen_mutex);
}

void init_ncurses() {
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE);

    // Inicjalizacja interfejsu
    mvprintw(0, 0, "STOLIKI:");
    mvprintw(MAX_TABLES + 2, 0, "DOCHOD:");
    mvprintw(MAX_TABLES + 3, 0, "LOGI:");
    refresh();
}

void end_ncurses() {
    clear();
    refresh();
    endwin();
}

void draw_interface() {
    Table *tables = shmat(shm_id, NULL, 0);
    if (tables == (void *)-1) {
        perror("[Utilities] Blad dolaczania pamieci dzielonej w draw_interface");
        return;
    }

    // Rysowanie statusu stolikow
    for (int i = 0; i < MAX_TABLES; i++) {
        mvprintw(i + 1, 0, "[%d] %s: %d/%d", 
            i + 1, 
            tables[i].occupied > 0 ? "Zajety" : "Wolny", 
            tables[i].occupied, 
            tables[i].capacity
        );
    }

    mvprintw(MAX_TABLES + 2, 0, "DOCHOD: %.2f PLN", total_revenue);

    refresh();
    shmdt(tables);
}

void sem_lock(int sem_id) {
    struct sembuf op = {0, -1, 0};
    if (semop(sem_id, &op, 1) == -1) {
        perror("[Semafor] Blad blokowania");
        exit(EXIT_FAILURE);
    }
}

void sem_unlock(int sem_id) {
    struct sembuf op = {0, 1, 0};
    if (semop(sem_id, &op, 1) == -1) {
        perror("[Semafor] Blad odblokowywania");
        exit(EXIT_FAILURE);
    }
}