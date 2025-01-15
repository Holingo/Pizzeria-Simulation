#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <pthread.h>

// Stałe konfiguracyjne
#define MAX_TABLES 10
#define SHM_KEY 1234
#define SEM_KEY 5678
#define MSG_KEY 9101

// Struktura dla stanu stolików
typedef struct {
    int tables[MAX_TABLES]; // Liczba miejsc przy każdym stoliku
} TableState;

// Deklaracje funkcji
void initialize_resources();
void cleanup_resources();
void handle_fire_signal(int sig);
void *client_thread(void *arg);
void *cashier_thread(void *arg);
void *firefighter_thread(void *arg);

// Globalne zmienne
int shm_id;
TableState *table_state;
int sem_id;
int msg_id;
pthread_t cashier_tid, firefighter_tid;

int main() {
    // Rejestracja obsługi sygnału pożaru
    signal(SIGUSR1, handle_fire_signal);

    // Inicjalizacja zasobów IPC
    initialize_resources();

    // Tworzenie wątku kasjera
    if (pthread_create(&cashier_tid, NULL, cashier_thread, NULL) != 0) {
        perror("Nie udało się utworzyć wątku kasjera");
        cleanup_resources();
        exit(EXIT_FAILURE);
    }

    // Tworzenie wątku strażaka
    if (pthread_create(&firefighter_tid, NULL, firefighter_thread, NULL) != 0) {
        perror("Nie udało się utworzyć wątku strażaka");
        cleanup_resources();
        exit(EXIT_FAILURE);
    }

    // Symulacja klientów (przykładowe grupy)
    pthread_t client_tid;
    for (int i = 0; i < 5; i++) {
        int *group_size = malloc(sizeof(int));
        *group_size = (i % 3) + 1; // Grupa 1-3 osobowa
        if (pthread_create(&client_tid, NULL, client_thread, group_size) != 0) {
            perror("Nie udało się utworzyć wątku klienta");
        }
        sleep(1); // Symulacja czasu między przyjściem klientów
    }

    // Oczekiwanie na zakończenie wątków
    pthread_join(cashier_tid, NULL);
    pthread_join(firefighter_tid, NULL);

    // Czyszczenie zasobów
    cleanup_resources();

    return 0;
}

void initialize_resources() {
    // Tworzenie pamięci dzielonej
    shm_id = shmget(SHM_KEY, sizeof(TableState), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Błąd tworzenia pamięci dzielonej");
        exit(EXIT_FAILURE);
    }

    table_state = (TableState *)shmat(shm_id, NULL, 0);
    if (table_state == (void *)-1) {
        perror("Błąd dołączania pamięci dzielonej");
        exit(EXIT_FAILURE);
    }

    // Inicjalizacja stolików
    for (int i = 0; i < MAX_TABLES; i++) {
        table_state->tables[i] = (i % 4) + 1; // Stolik 1-4 osobowy
    }

    // Tworzenie semafora
    sem_id = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("Błąd tworzenia semafora");
        exit(EXIT_FAILURE);
    }
    semctl(sem_id, 0, SETVAL, 1);

    // Tworzenie kolejki komunikatów
    msg_id = msgget(MSG_KEY, IPC_CREAT | 0666);
    if (msg_id == -1) {
        perror("Błąd tworzenia kolejki komunikatów");
        exit(EXIT_FAILURE);
    }
}

void cleanup_resources() {
    // Usuwanie pamięci dzielonej
    if (shmdt(table_state) == -1) {
        perror("Błąd odłączania pamięci dzielonej");
    }
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("Błąd usuwania pamięci dzielonej");
    }

    // Usuwanie semafora
    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("Błąd usuwania semafora");
    }

    // Usuwanie kolejki komunikatów
    if (msgctl(msg_id, IPC_RMID, NULL) == -1) {
        perror("Błąd usuwania kolejki komunikatów");
    }
}

void handle_fire_signal(int sig) {
    printf("Otrzymano sygnał pożaru! Ewakuacja w toku...\n");
    // Symulacja ewakuacji klientów
    sleep(2);
    printf("Ewakuacja zakończona. Zamknięcie lokalu.\n");
    exit(EXIT_SUCCESS);
}

void *client_thread(void *arg) {
    int group_size = *(int *)arg;
    free(arg);

    printf("Nowa grupa klientów: %d osobowa\n", group_size);
    // Symulacja składania zamówienia i zajmowania miejsca
    sleep(2);

    printf("Grupa %d osobowa zakończyła wizytę.\n", group_size);
    return NULL;
}

void *cashier_thread(void *arg) {
    printf("Kasjer rozpoczął pracę.\n");
    while (1) {
        // Symulacja obsługi zamówień
        sleep(5);
        printf("Kasjer obsługuje zamówienia...\n");
    }
    return NULL;
}

void *firefighter_thread(void *arg) {
    printf("Strażak w gotowości.\n");
    while (1) {
        // Czeka na sygnał pożaru
        pause();
    }
    return NULL;
}