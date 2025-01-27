// Updated main.c for better error handling and resource cleanup
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
pthread_t cashier_tid, firefighter_tid;
int next_group_id = 1; // Zmienna do nadawania unikalnych ID grup

// Deklaracje funkcji
void initialize_resources();
void cleanup_resources();
void spawn_client_processes();

// Funkcja obslugi sygnalu
void handle_sigint(int sig) {
    log_event("[Main] Otrzymano SIGINT. Zamykanie programu...");
    is_open = 0; // Ustawienie flagi do zatrzymania glownej petli
    cleanup_resources(); // Zwolnienie zasobow
    end_ncurses(); // Zakonczenie ncurses (jesli uzywane)
    printf("\nProgram zakonczyl dzialanie.\n");
    exit(EXIT_SUCCESS); // Wyjscie z programu
}

int main() {
    // Rejestracja sygnalu pozaru
    signal(SIGUSR1, handle_fire_signal);

    // Rejestracja obslugi SIGINT
    signal(SIGINT, handle_sigint);

    // Inicjalizacja zasobow IPC
    initialize_resources();

    // Tworzenie watku kasjera
    if (pthread_create(&cashier_tid, NULL, cashier_behavior, NULL) != 0) {
        perror("[Main] Nie udalo sie utworzyc watku kasjera");
        cleanup_resources();
        exit(EXIT_FAILURE);
    }

    // Tworzenie watku strazaka
    if (pthread_create(&firefighter_tid, NULL, firefighter_behavior, NULL) != 0) {
        perror("[Main] Nie udalo sie utworzyc watku strazaka");
        cleanup_resources();
        exit(EXIT_FAILURE);
    }

    // Tworzenie klientow jako procesy
    spawn_client_processes();

    // Czekanie na zakonczenie watkow
    pthread_join(cashier_tid, NULL);
    pthread_join(firefighter_tid, NULL);

    // Czyszczenie zasobow
    cleanup_resources();
    printf("[Main] Pizzeria zakonczyla dzialanie.\n");
    return 0;
}

void initialize_resources() {
    // Tworzenie pamieci dzielonej dla stolikow
    shm_id = shmget(SHM_KEY, sizeof(Table) * MAX_TABLES, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("[Main] Blad tworzenia pamieci dzielonej");
        exit(EXIT_FAILURE);
    }

    Table *tables = shmat(shm_id, NULL, 0);
    if (tables == (void *)-1) {
        perror("[Main] Blad dolaczania pamieci dzielonej");
        shmctl(shm_id, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }

    // Inicjalizacja stolikow
    for (int i = 0; i < MAX_TABLES; i++) {
        tables[i].capacity = i + 1; // Stolik 1-osobowy, 2-osobowy itd.
        tables[i].occupied = 0;
        snprintf(tables[i].order_status, sizeof(tables[i].order_status), "Pusty");
    }

    shmdt(tables); // Odlaczenie pamieci dzielonej

    // Tworzenie semafora do synchronizacji
    sem_id = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("[Main] Blad tworzenia semafora");
        shmctl(shm_id, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }
    semctl(sem_id, 0, SETVAL, 1); // Ustawienie semafora na 1 (odblokowany)

    // Tworzenie kolejki komunikatow
    msg_id = msgget(MSG_KEY, IPC_CREAT | 0666);
    if (msg_id == -1) {
        perror("[Main] Blad tworzenia kolejki komunikatow");
        shmctl(shm_id, IPC_RMID, NULL);
        semctl(sem_id, 0, IPC_RMID);
        exit(EXIT_FAILURE);
    }

    // Inicjalizacja ncurses
    init_ncurses();
    log_event("[Main] Pizzeria otwarta!");
}

void cleanup_resources() {
    // Usuwanie pamieci dzielonej
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("[Main] Blad usuwania pamieci dzielonej");
    }

    // Usuwanie semafora
    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("[Main] Blad usuwania semafora");
    }

    // Usuwanie kolejki komunikatow
    if (msgctl(msg_id, IPC_RMID, NULL) == -1) {
        perror("[Main] Blad usuwania kolejki komunikatow");
    }

    // Konczenie ncurses
    end_ncurses();
}

// Funkcja do uruchamiania procesow klientow
void spawn_client_processes() {
    srand(time(NULL)); // Inicjalizacja losowosci

    for (int i = 0; i < 10 && is_open; i++) { // Tworzenie 10 klientow
        pid_t pid = fork();
        if (pid == 0) {
            // Proces potomny - klient
            int group_id = next_group_id++;       // Kolejne ID grupy
            int group_size = (rand() % 3) + 1;    // Grupa 1-3 osobowa
            simulate_client(group_id, group_size); // Wywolanie funkcji z client.c
            exit(EXIT_SUCCESS);
        } else if (pid < 0) {
            perror("[Main] Blad tworzenia procesu klienta");
        }
        sleep(1); // Odstep miedzy kolejnymi klientami
    }
}