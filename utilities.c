// Plik: utilities.c
// Opis: Plik zawiera implementację funkcji wspólnych dla całego projektu. Obsługuje logowanie, interfejs konsoli, semafory oraz zarządzanie pamięcią współdzieloną.

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
int log_count = 0;
struct log_message log_messages[MAX_LOGS];

// Kolory ANSI dla terminala
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define RED_BG "\033[41m"

// Funkcja log_event zapisuje wiadomość do pamięci (bufor logów) i jednocześnie do pliku logów,
// zapewniając synchronizację dostępu za pomocą mutexa.
// Dzięki temu wiele wątków może bezpiecznie korzystać z tej funkcji.
void log_event(const char *message) {
    pthread_mutex_lock(&screen_mutex); // Funkcja blokuje dostęp do sekcji krytycznej chronionej przez mutex

    // Dodawanie wiadomości do bufora logów.
    if (log_count < MAX_LOGS) {
        snprintf(log_messages[log_count++].content, sizeof(log_messages[0].content), "%s", message);
    } else {
        // Jeśli bufor jest pełny, usuwa najstarszy log i przesuwa pozostałe.
        for (int i = 1; i < MAX_LOGS; i++) {
            log_messages[i - 1] = log_messages[i];
        }
        snprintf(log_messages[MAX_LOGS - 1].content, sizeof(log_messages[0].content), "%s", message);
    }

    FILE *log_file = fopen("debug.log", "a");
    if (log_file) {
        fprintf(log_file, "%s\n", message);
        fclose(log_file);
    }

    pthread_mutex_unlock(&screen_mutex); // Odblokowanie mutexa, aby inne wątki mogły korzystać z tej funkcji.
}

void display_interface(Table *tables, int table_count) {
    pthread_mutex_lock(&screen_mutex);
    system("clear");
    printf("============================================================\n");
    printf("                     %sPIZZERIA%s\n", CYAN, RESET);
    printf("============================================================\n\n");

    printf("%s[STOLIKI]%s\n------------------------------------------------------------\n", CYAN, RESET);
    for (int i = 0; i < table_count; i++) {
        printf("Stolik %d | %s%s%s   | Zamówienie: %s%s%s\n", 
               i + 1,
               tables[i].occupied ? RED : GREEN,
               tables[i].occupied ? "Zajęty" : "Wolny", RESET,
               tables[i].order_status[0] ? BLUE : YELLOW,
               tables[i].order_status[0] ? tables[i].order_status : "Brak", RESET);
    }

    printf("\n%s[DOCHÓD]%s\n------------------------------------------------------------\n", CYAN, RESET);
    printf("Łączny dochód: %s%.2f PLN%s\n\n", YELLOW, total_revenue, RESET);

    printf("%s[LOGI]%s\n------------------------------------------------------------\n", CYAN, RESET);
    for (int i = 0; i < log_count; i++) {
        const char *color = WHITE; // Domyślny kolor
        if (strstr(log_messages[i].content, "[Klient")) {
            color = CYAN; // Logi klientów w kolorze CYAN
        } else if (strstr(log_messages[i].content, "[Kasjer")) {
            color = GREEN; // Logi kasjera w kolorze GREEN
        } else if (strstr(log_messages[i].content, "[Strażak")) {
            color = RED_BG; // Logi alertów w kolorze tła RED_BG
        } else if (strstr(log_messages[i].content, "ALERT")) {
            color = RED_BG; // Logi alertów w kolorze tła RED_BG
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
    FILE *debug_log = fopen("debug.log", "w");
    if (debug_log) {
        fprintf(debug_log, "Logi z uruchomienia pizzerii\n");
        fclose(debug_log);
    } else {
        perror("Nie można utworzyć debug.log");
    }
}

void cleanup_console() {
    printf("Zamknięcie pizzerii. Dziękujemy!\n");
}

// Blokuje dostęp do pamięci współdzielonej, aby zapobiec konfliktom między procesami.
void sem_lock(int sem_id) {
    struct sembuf op = {0, -1, 0};
    if (semop(sem_id, &op, 1) == -1) {
        perror("Semafor: błąd blokowania");
        exit(EXIT_FAILURE);
    }
}

void sem_unlock(int sem_id) {
    struct sembuf op = {0, 1, 0};
    if (semop(sem_id, &op, 1) == -1) {
        perror("Semafor: błąd odblokowywania");
        exit(EXIT_FAILURE);
    }
}