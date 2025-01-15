#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <pthread.h>
#include "utilities.h" // Naglowek wspolny dla wszystkich modulow

void *client_behavior(void *arg) {
    int group_size = *(int *)arg;
    free(arg);

    printf("[Klient] Grupa %d osobowa wchodzi do pizzerii.\n", group_size);

    // Uzyskanie dostepu do pamieci dzielonej
    int shm_id = shmget(SHM_KEY, sizeof(TableState), 0666);
    if (shm_id == -1) {
        perror("[Klient] Błąd uzyskiwania dostepu do pamieci dzielonej");
        return NULL;
    }

    TableState *table_state = (TableState *)shmat(shm_id, NULL, 0);
    if (table_state == (void *)-1) {
        perror("[Klient] Błąd dołączania pamięci dzielonej");
        return NULL;
    }

    // Uzyskanie dostepu do semafora
    int sem_id = semget(SEM_KEY, 1, 0666);
    if (sem_id == -1) {
        perror("[Klient] Błąd uzyskiwania dostepu do semofora");
        shmdt(table_state);
        return NULL;
    }

    // Rezerwacja stolika (sekcja krytyczna)
    sem_lock(sem_id);

    int table_found = 0;
    for (int i = 0; i < MAX_TABLES; i++) {
        if (table_state->tables[i] >= group_size) {
            printf("[Klient] Grupa %d osobowa zajmuje stolik %d.\n", group_size, i);
            table_found = 1;
            break;
        }   
    }

    sem_unlock(sem_id);

    if (!table_found) {
        printf("[Klient] Brak wolnego miejsca dla grupy %d osobowej.\n", group_size);
        shmdt(table_state);
        return NULL;
    }

    // Symulacja jedzenia pizzy
    sleep(3);

    // Zwolnienia stolika
    sem_lock(sem_id);
    for (int i = 0; i < MAX_TABLES; i++) {
        if (table_state->tables[i] + group_size <= ((i % 4) + 1)) {
            printf("[Klient] Grupa %d osobowa zwalnia stolik %d.\n", group_size, i);
            table_state->tables[i] += group_size;
            break;
        }
    }
    sem_unlock(sem_id);

    shmdt(table_state);

    printf("[Klient] Grupa %d osobowa opuszcza pizzerię.\n", group_size);

    return NULL;
}