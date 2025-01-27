// Updated main.c with dynamic interface updates
#include "utilities.h"
#include <signal.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

// Globalne zmienne
int is_open = 1;
int shm_id, sem_id, msg_id;
pthread_t cashier_tid, firefighter_tid, client_spawner_tid;
int next_group_id = 1; // Zmienna do nadawania unikalnych ID grup

// Deklaracje funkcji
void initialize_resources(int X1, int X2, int X3, int X4);
void cleanup_resources();
void handle_signal(int sig);

int main(int argc, char *argv[]) {
    int X1 = 1, X2 = 1, X3 = 1, X4 = 1; // Domyslne wartosci stolikow

    if (argc == 5) {
        X1 = atoi(argv[1]);
        X2 = atoi(argv[2]);
        X3 = atoi(argv[3]);
        X4 = atoi(argv[4]);
    } else if (argc != 1) {
        fprintf(stderr, "Usage: %s [X1 X2 X3 X4]\\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGUSR1, handle_signal);
    signal(SIGINT, handle_signal);

    initialize_resources(X1, X2, X3, X4);

    // Uruchamianie kasjera i strażaka
    if (pthread_create(&cashier_tid, NULL, cashier_behavior, NULL) != 0) {
        perror("Failed to create cashier thread");
        cleanup_resources();
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&firefighter_tid, NULL, firefighter_behavior, NULL) != 0) {
        perror("Failed to create firefighter thread");
        cleanup_resources();
        exit(EXIT_FAILURE);
    }

    // Uruchamianie generowania klientów w tle
    int num_clients = 3;
    if (pthread_create(&client_spawner_tid, NULL, client_spawner, &num_clients) != 0) {
        perror("Failed to create client spawner thread");
        cleanup_resources();
        exit(EXIT_FAILURE);
    }

    // Pętla główna do odświeżania interfejsu
    while (is_open) {
        Table *tables = shmat(shm_id, NULL, 0);
        if (tables == (void *)-1) {
            perror("Blad dolaczania pamieci dzielonej");
            break;
        }

        display_interface(tables, X1 + X2 + X3 + X4); // Wyświetlanie interfejsu

        shmdt(tables);
        usleep(500000); // Odświeżanie co 0.5 sekundy
    }

    // Czekanie na zakończenie wątków
    pthread_join(cashier_tid, NULL);
    pthread_join(firefighter_tid, NULL);
    pthread_join(client_spawner_tid, NULL);

    cleanup_resources();
    printf("Pizzeria closed successfully.\n");
    return 0;
}

void initialize_resources(int X1, int X2, int X3, int X4) {
    shm_id = shmget(SHM_KEY, sizeof(Table) * (X1 + X2 + X3 + X4), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Shared memory creation failed");
        exit(EXIT_FAILURE);
    }

    Table *tables = shmat(shm_id, NULL, 0);
    if (tables == (void *)-1) {
        perror("Shared memory attachment failed");
        exit(EXIT_FAILURE);
    }

    int index = 0;
    for (int i = 0; i < X1; i++) {
        tables[index].capacity = 1;
        tables[index].occupied = 0;
        snprintf(tables[index].order_status, sizeof(tables[index].order_status), "Wolne (0/1)");
        index++;
    }
    for (int i = 0; i < X2; i++) {
        tables[index].capacity = 2;
        tables[index].occupied = 0;
        snprintf(tables[index].order_status, sizeof(tables[index].order_status), "Wolne (0/2)");
        index++;
    }
    for (int i = 0; i < X3; i++) {
        tables[index].capacity = 3;
        tables[index].occupied = 0;
        snprintf(tables[index].order_status, sizeof(tables[index].order_status), "Wolne (0/3)");
        index++;
    }
    for (int i = 0; i < X4; i++) {
        tables[index].capacity = 4;
        tables[index].occupied = 0;
        snprintf(tables[index].order_status, sizeof(tables[index].order_status), "Wolne (0/4)");
        index++;
    }

    shmdt(tables);

    sem_id = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("Semaphore creation failed");
        exit(EXIT_FAILURE);
    }
    semctl(sem_id, 0, SETVAL, 1);

    msg_id = msgget(MSG_KEY, IPC_CREAT | 0666);
    if (msg_id == -1) {
        perror("Message queue creation failed");
        exit(EXIT_FAILURE);
    }

    init_console();
    log_event("Pizzeria opened successfully.");
}

void cleanup_resources() {
    shmctl(shm_id, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID);
    msgctl(msg_id, IPC_RMID, NULL);
    cleanup_console();
}

void handle_signal(int sig) {
    if (sig == SIGUSR1 || sig == SIGINT) {
        log_event("Signal received. Closing pizzeria.");
        is_open = 0;
        cleanup_resources();
        exit(EXIT_SUCCESS);
    }
}