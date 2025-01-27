// Client Spawner and Simulation for Pizzeria Simulation
#include "utilities.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

void *client_spawner(void *arg) {
    int num_clients = *(int *)arg;
    char log_message[200];
    snprintf(log_message, sizeof(log_message), "[Spawner] Rozpoczęcie generowania %d klientów...", num_clients);
    log_event(log_message);

    srand(time(NULL));
    for (int i = 0; i < num_clients && is_open; i++) {
        snprintf(log_message, sizeof(log_message), "[Spawner] Tworzenie klienta %d...", i + 1);
        log_event(log_message);

        pid_t pid = fork();
        if (pid == 0) {
            // Inicjalizacja w procesie potomnym
            pthread_mutex_destroy(&screen_mutex);
            pthread_mutex_init(&screen_mutex, NULL);

            int group_size = (rand() % 3) + 1;
            snprintf(log_message, sizeof(log_message), "[Client %d] Rozpoczyna symulację (grupa %d osób)", i + 1, group_size);
            log_event(log_message);

            simulate_client(i + 1, group_size);
            exit(EXIT_SUCCESS);
        } else if (pid < 0) {
            snprintf(log_message, sizeof(log_message), "[ERROR] Nie można utworzyć procesu klienta %d", i + 1);
            log_event(log_message);
        }
        sleep(1); // Odstęp między klientami
    }
    snprintf(log_message, sizeof(log_message), "[Spawner] Zakończono generowanie klientów.");
    log_event(log_message);
    return NULL;
}

void simulate_client(int group_id, int group_size) {
    setbuf(stdout, NULL); // Wyłączenie buforowania
    setbuf(stderr, NULL);

    log_event("[Client ID] Rozpoczyna symulację");

    // Attach to shared memory
    Table *tables = shmat(shm_id, NULL, 0);
    if (tables == (void *)-1) {
        log_event("[Client] Błąd dolaczania do pamięci dzielonej");
        exit(EXIT_FAILURE);
    }

    // Try to find a suitable table
    int table_id = -1;
    sem_lock(sem_id); // Lock semaphore
    for (int i = 0; i < MAX_TABLES; i++) {
        if (tables[i].capacity >= group_size && tables[i].occupied == 0) {
            tables[i].occupied = group_size;
            table_id = i;
            snprintf(tables[i].order_status, sizeof(tables[i].order_status), "Group %d (%d/%d)", group_id, group_size, tables[i].capacity);
            break;
        }
    }
    sem_unlock(sem_id); // Unlock semaphore

    if (table_id == -1) {
        char log_message[200];
        snprintf(log_message, sizeof(log_message), "[Client ID:%d] Nie może wejść. Brak wolnych miejsc.", group_id);
        log_event(log_message);
        shmdt(tables);
        exit(EXIT_FAILURE);
    }

    // Log seating
    char log_message[200];
    snprintf(log_message, sizeof(log_message), "[Client ID:%d] Zajął stolik %d", group_id, table_id + 1);
    log_event(log_message);

    // Place an order
    FILE *menu = fopen(MENU_FILE, "r");
    if (!menu) {
        log_event("[Client] Nie udało się otworzyć menu");
        shmdt(tables);
        exit(EXIT_FAILURE);
    }

    char items[10][100];
    float prices[10];
    int item_count = 0;

    while (fscanf(menu, "%s %f", items[item_count], &prices[item_count]) == 2) {
        item_count++;
    }
    fclose(menu);

    srand(time(NULL) ^ group_id);
    int choice = rand() % item_count;

    struct order_message msg;
    msg.msg_type = 1;
    msg.table_id = table_id;
    msg.group_id = group_id;
    snprintf(msg.order, sizeof(msg.order), "%s", items[choice]);
    msg.price = prices[choice];

    if (msgsnd(msg_id, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
        log_event("[Client] Nie udało się wysłać zamówienia");
        shmdt(tables);
        exit(EXIT_FAILURE);
    }

    snprintf(log_message, sizeof(log_message), "[Client ID:%d] Zamówił %s (%.2f PLN)", group_id, msg.order, msg.price);
    log_event(log_message);

    // Simulate dining
    sleep(3);

    // Leave table
    sem_lock(sem_id);
    tables[table_id].occupied = 0;
    snprintf(tables[table_id].order_status, sizeof(tables[table_id].order_status), "Wolny (0/%d)", tables[table_id].capacity);
    sem_unlock(sem_id);

    snprintf(log_message, sizeof(log_message), "[Client ID:%d] Opuszcza stolik %d", group_id, table_id + 1);
    log_event(log_message);

    shmdt(tables);
    exit(EXIT_SUCCESS);
}