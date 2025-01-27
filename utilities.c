// Updated utilities.c to retain historical logs in console
#include "utilities.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>

pthread_mutex_t screen_mutex = PTHREAD_MUTEX_INITIALIZER;
float total_revenue = 0;

// Global log buffer
struct log_message log_messages[MAX_LOGS];
int log_count = 0;

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

    // Dodawanie wiadomości do bufora logów
    if (log_count < MAX_LOGS) {
        snprintf(log_messages[log_count].content, sizeof(log_messages[log_count].content), "%s", message);
        log_messages[log_count].msg_type = 1; // Typ logu
        log_count++;
    } else {
        // Przesuwanie bufora logów w górę
        for (int i = 1; i < MAX_LOGS; i++) {
            log_messages[i - 1] = log_messages[i];
        }
        snprintf(log_messages[MAX_LOGS - 1].content, sizeof(log_messages[MAX_LOGS - 1].content), "%s", message);
        log_messages[MAX_LOGS - 1].msg_type = 1;
    }

    // Zapis do pliku logów
    FILE *log_file = fopen("debug.log", "a");
    if (log_file) {
        fprintf(log_file, "%s\n", message);
        fclose(log_file);
    }

    pthread_mutex_unlock(&screen_mutex);
}

void display_interface(Table *tables, int table_count) {
    pthread_mutex_lock(&screen_mutex);

    // Clear the screen
    system("clear"); // Czyszczenie konsoli
    printf("\033[H\033[J");

    // Header
    printf("============================================================\n");
    printf("                     %sPIZZERIA%s\n", CYAN, RESET);
    printf("============================================================\n\n");

    // Tables
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

    // Revenue
    printf("\n%s[DOCHOD]%s\n", CYAN, RESET);
    printf("------------------------------------------------------------\n");
    printf("Laczny dochod: %s%.2f PLN%s\n\n", YELLOW, total_revenue, RESET);

    // Logs
    printf("%s[LOGI]%s\n", CYAN, RESET);
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < log_count; i++) {
        const char *color = WHITE; // Default color
        if (strstr(log_messages[i].content, "[Klient")) {
            color = CYAN; // Klient in CYAN
        } else if (strstr(log_messages[i].content, "[Kasjer")) {
            color = GREEN; // Kasjer in GREEN
        } else if (strstr(log_messages[i].content, "ALERT")) {
            color = RED_BG; // Alert in RED_BG
        }
        printf("%s%s%s\n", color, log_messages[i].content, RESET);
    }

    printf("============================================================\n");
    pthread_mutex_unlock(&screen_mutex);
}

void update_table_status(int table_index, const char *status) {
    char message[200];
    snprintf(message, sizeof(message), "[STOLIK %d] Status: %s", table_index + 1, status);
    log_event(message);
}

void update_revenue(float amount) {
    pthread_mutex_lock(&screen_mutex);
    total_revenue += amount;
    pthread_mutex_unlock(&screen_mutex);
}

void init_console() {
    printf("Pizzeria uruchomiona!\n");

    // Initialize debug.log file
    FILE *debug_log = fopen("debug.log", "w");
    if (debug_log) {
        fprintf(debug_log, "Logi z uruchomienia pizzerii\n");
        fclose(debug_log);
    } else {
        perror("[Init] Nie można utworzyć pliku debug.log");
    }
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