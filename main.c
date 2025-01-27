// Plik: main.c
// Opis: Główny plik programu symulującego działanie pizzerii. Odpowiada za inicjalizację zasobów, obsługę sygnałów oraz zarządzanie głównymi wątkami programu.

#include "utilities.h"
#include <signal.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

// Zmienne globalne
int is_open = 1;
int shm_id, sem_id, msg_id;
pthread_t cashier_tid, firefighter_tid, client_spawner_tid, log_listener_tid;

// Prototypy funkcji
void initialize_resources(int X1, int X2, int X3, int X4);
void cleanup_resources();
void handle_signal(int sig);

void *log_listener(void *arg) {
    struct log_message log;

    while (is_open) {
        if (msgrcv(msg_id, &log, sizeof(log.content), 1, IPC_NOWAIT) != -1) {
            log_event(log.content); // Dodaj log do bufora i pliku
        }
        usleep(100000); // Odświeżanie co 0.1 sekundy
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    int X1 = 1, X2 = 1, X3 = 1, X4 = 1;

    if (argc == 5) {
        X1 = atoi(argv[1]);
        X2 = atoi(argv[2]);
        X3 = atoi(argv[3]);
        X4 = atoi(argv[4]);
    } else if (argc != 1) {
        fprintf(stderr, "Użycie: %s [X1 X2 X3 X4]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Ignorowanie procesów zombie
    signal(SIGCHLD, SIG_IGN);
    signal(SIGUSR1, handle_signal);
    signal(SIGINT, handle_signal);

    initialize_resources(X1, X2, X3, X4);

    if (pthread_create(&log_listener_tid, NULL, log_listener, NULL) != 0) {
        perror("Nie udało się utworzyć wątku nasłuchującego logi");
        cleanup_resources();
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&cashier_tid, NULL, cashier_behavior, NULL) != 0) {
        perror("Nie udało się utworzyć wątku kasjera");
        cleanup_resources();
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&firefighter_tid, NULL, firefighter_behavior, NULL) != 0) {
        perror("Nie udało się utworzyć wątku strażaka");
        cleanup_resources();
        exit(EXIT_FAILURE);
    }

    int num_clients = 5;
    if (pthread_create(&client_spawner_tid, NULL, client_spawner, &num_clients) != 0) {
        perror("Nie udało się utworzyć wątku klientów");
        cleanup_resources();
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Oczekiwanie na zakończenie procesów klientów
        while (waitpid(-1, NULL, WNOHANG) > 0);

        Table *tables = shmat(shm_id, NULL, 0);
        if (tables == (void *)-1) {
            perror("Błąd dołączania pamięci współdzielonej");
            break;
        }

        display_interface(tables, X1 + X2 + X3 + X4); // Odświeżanie interfejsu

        // Jeśli lokal jest zamknięty, sprawdź, czy wszystkie stoliki są puste
        if (!is_open) {
            int all_tables_empty = 1;
            for (int i = 0; i < X1 + X2 + X3 + X4; i++) {
                if (tables[i].occupied > 0) {
                    all_tables_empty = 0;
                    break;
                }
            }

            // Jeśli wszystkie stoliki są puste, zakończ pracę kasjera i wyjdź z pętli
            if (all_tables_empty) {
                log_event("[Główna pętla] Wszystkie stoliki są puste. Lokal zamknięty.");
                shmdt(tables);
                break;
            }
        }

        shmdt(tables);
        usleep(500000); // Odświeżanie co 0.5 sekundy
    }

    pthread_join(cashier_tid, NULL);
    pthread_join(firefighter_tid, NULL);
    pthread_join(client_spawner_tid, NULL);
    pthread_join(log_listener_tid, NULL);

    cleanup_resources();
    printf("Pizzeria zamknięta pomyślnie.\n");
    return 0;
}

void initialize_resources(int X1, int X2, int X3, int X4) {
    shm_id = shmget(SHM_KEY, sizeof(Table) * (X1 + X2 + X3 + X4), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Błąd tworzenia pamięci współdzielonej");
        exit(EXIT_FAILURE);
    }

    Table *tables = shmat(shm_id, NULL, 0);
    if (tables == (void *)-1) {
        perror("Błąd dołączania pamięci współdzielonej");
        exit(EXIT_FAILURE);
    }

    int index = 0;
    for (int i = 0; i < X1; i++, index++) {
        tables[index].capacity = 1;
        tables[index].occupied = 0;
        snprintf(tables[index].order_status, sizeof(tables[index].order_status), "Wolne (0/1)");
    }
    for (int i = 0; i < X2; i++, index++) {
        tables[index].capacity = 2;
        tables[index].occupied = 0;
        snprintf(tables[index].order_status, sizeof(tables[index].order_status), "Wolne (0/2)");
    }
    for (int i = 0; i < X3; i++, index++) {
        tables[index].capacity = 3;
        tables[index].occupied = 0;
        snprintf(tables[index].order_status, sizeof(tables[index].order_status), "Wolne (0/3)");
    }
    for (int i = 0; i < X4; i++, index++) {
        tables[index].capacity = 4;
        tables[index].occupied = 0;
        snprintf(tables[index].order_status, sizeof(tables[index].order_status), "Wolne (0/4)");
    }

    shmdt(tables);

    sem_id = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("Błąd tworzenia semafora");
        exit(EXIT_FAILURE);
    }
    semctl(sem_id, 0, SETVAL, 1);

    msg_id = msgget(MSG_KEY, IPC_CREAT | 0666);
    if (msg_id == -1) {
        perror("Błąd tworzenia kolejki komunikatów");
        exit(EXIT_FAILURE);
    }

    init_console();
    log_event("Pizzeria otwarta pomyślnie.");
}

void cleanup_resources() {
    shmctl(shm_id, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID);
    msgctl(msg_id, IPC_RMID, NULL);
    cleanup_console();
}

void handle_signal(int sig) {
    if (sig == SIGUSR1 || sig == SIGINT) {
        log_event("Otrzymano sygnał. Zamykamy pizzerię.");
        is_open = 0;
        cleanup_resources();
        exit(EXIT_SUCCESS);
    }
}