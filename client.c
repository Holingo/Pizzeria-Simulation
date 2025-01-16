#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include "utilities.h"

struct message {
    long msg_type;
    char text[100];
};

void *client_behavior(void *arg) {
    int group_size = *(int *)arg;
    free(arg);

    printf("[Klient] Grupa %d osobowa wchodzi do pizzerii.\n", group_size);

    // Dostęp do pamięci dzielonej
    int shm_id = shmget(SHM_KEY, sizeof(TableState), 0666);
    if (shm_id == -1) {
        perror("[Klient] Błąd dostępu do pamięci dzielonej");
        return NULL;
    }
    TableState *table_state = (TableState *)shmat(shm_id, NULL, 0);

    // Dostęp do semafora
    int sem_id = semget(SEM_KEY, 1, 0666);
    if (sem_id == -1) {
        perror("[Klient] Błąd dostępu do semafora");
        shmdt(table_state);
        return NULL;
    }

    sem_lock(sem_id);

    // Szukanie dostępnego stolika
    int table_found = 0;
    for (int i = 0; i < MAX_TABLES; i++) {
        if (table_state->tables[i] >= group_size) {
            printf("[Klient] Grupa %d osobowa zajmuje stolik %d.\n", group_size, i);
            table_state->tables[i] -= group_size;
            table_found = 1;
            break;
        }
    }

    sem_unlock(sem_id);

    if (!table_found) {
        printf("[Klient] Brak wolnego stolika dla grupy %d osobowej.\n", group_size);
        shmdt(table_state);
        return NULL;
    }

    // Wysyłanie zamówienia do kasjera
    int msg_id = msgget(MSG_KEY, 0666);
    if (msg_id == -1) {
        perror("[Klient] Błąd dostępu do kolejki komunikatów");
        shmdt(table_state);
        return NULL;
    }

    struct message msg;
    msg.msg_type = 1; // Typ komunikatu dla zamówień
    snprintf(msg.text, sizeof(msg.text), "Zamówienie od grupy %d osobowej.", group_size);

    if (msgsnd(msg_id, &msg, sizeof(msg.text), 0) == -1) {
        perror("[Klient] Błąd wysyłania zamówienia");
    } else {
        printf("[Klient] Grupa %d osobowa złożyła zamówienie.\n", group_size);
    }

    // Symulacja jedzenia pizzy
    sleep(3);

    // Zwolnienie stolika
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