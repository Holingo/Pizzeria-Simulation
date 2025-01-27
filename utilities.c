// Updated utilities.c with console output using printf and ANSI colors
#include "utilities.h"
#include <time.h>

pthread_mutex_t screen_mutex = PTHREAD_MUTEX_INITIALIZER;
float total_revenue = 0;
int log_count = 0;
char log_messages[MAX_LOGS][200];

// ANSI Color Codes
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define RED_BG "\033[41m"

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

    pthread_mutex_unlock(&screen_mutex);
}

void display_interface(Table *tables, int table_count) {
    pthread_mutex_lock(&screen_mutex);
    system("clear"); // Czyszczenie konsoli

    // Naglowek
    printf("============================================================\n");
    printf("                     %sPIZZERIA%s\n", CYAN, RESET);
    printf("============================================================\n\n");

    // Stoliki
    printf("%s[STOLIKI]%s\n", CYAN, RESET);
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < table_count; i++) {
        printf("Stolik %d | %s%s%s   | Zamowienie: %s%s%s\n", 
               i + 1,
               tables[i].occupied > 0 ? RED : GREEN,
               tables[i].occupied > 0 ? "Zajety" : "Wolny", RESET,
               tables[i].order_status[0] ? BLUE : YELLOW,
               tables[i].order_status[0] ? tables[i].order_status : "Brak", RESET);
    }

    // Dochod
    printf("\n%s[DOCHOD]%s\n", CYAN, RESET);
    printf("------------------------------------------------------------\n");
    printf("Laczny dochod: %s%.2f PLN%s\n\n", YELLOW, total_revenue, RESET);

    // Logi
    printf("%s[LOGI]%s\n", CYAN, RESET);
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < log_count; i++) {
        printf("%s%s%s\n", strstr(log_messages[i], "ALERT") ? RED_BG : WHITE, log_messages[i], RESET);
    }

    printf("============================================================\n");
    pthread_mutex_unlock(&screen_mutex);
}

void update_table_status(int table_index, int occupied, int capacity, const char *status) {
    pthread_mutex_lock(&screen_mutex);
    snprintf(log_messages[log_count], sizeof(log_messages[log_count]), "[STOLIK %d] Status: %s", table_index + 1, status);
    log_count = (log_count + 1) % MAX_LOGS;
    pthread_mutex_unlock(&screen_mutex);
}

void update_revenue(float amount) {
    pthread_mutex_lock(&screen_mutex);
    total_revenue += amount;
    pthread_mutex_unlock(&screen_mutex);
}

void init_console() {
    printf("Pizzeria uruchomiona!\n");
}

void cleanup_console() {
    printf("Zamkniecie pizzerii. Dziekujemy!\n");
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