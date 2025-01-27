// Plik: client.c
// Opis: Plik odpowiedzialny za symulację zachowań klientów w pizzerii. Klienci przychodzą w grupach, zajmują stoliki i składają zamówienia. Plik zarządza tworzeniem procesów dla klientów i ich obsługą.

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

        // Tworzy proces potomny dla każdego klienta.
        pid_t pid = fork();
        if (pid == 0) {
            pthread_mutex_destroy(&screen_mutex); // Usuwa istniejący mutex, aby upewnić się, że nie ma konfliktów w synchronizacji.
            pthread_mutex_init(&screen_mutex, NULL); // Następnie ponownie inicjalizuje mutex z domyślnymi atrybutami.

            srand(time(NULL) ^ getpid());
            int group_size = (rand() % 3) + 1;

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

void handle_client_signal(int sig) {
    if (sig == SIGUSR1) {
        is_open = 0; // Oznacza zamknięcie lokalu
    }
}

void simulate_client(int group_id, int group_size) {
    // Rejestracja obsługi sygnału dla klienta
    signal(SIGUSR1, handle_client_signal);

    struct log_message log;
    log.msg_type = 1;

    // Na początku sprawdź, czy lokal jest otwarty
    if (!is_open) {
        snprintf(log.content, sizeof(log.content), "[Klient ID:%d] Lokal zamknięty. Odrzucono wejście.", group_id);
        msgsnd(msg_id, &log, sizeof(log.content), 0);
        exit(EXIT_SUCCESS);
    }

    // Logowanie rozpoczęcia symulacji
    snprintf(log.content, sizeof(log.content), "[Klient ID:%d] Rozpoczyna symulację (grupa %d osób)", group_id, group_size);
    msgsnd(msg_id, &log, sizeof(log.content), 0);

    // Dołączanie do pamięci współdzielonej
    Table *tables = shmat(shm_id, NULL, 0);
    if (tables == (void *)-1) {
        snprintf(log.content, sizeof(log.content), "[Klient ID:%d] Błąd dołączania do pamięci współdzielonej", group_id);
        msgsnd(msg_id, &log, sizeof(log.content), 0);
        exit(EXIT_FAILURE);
    }

    // Szukanie wolnego stolika
    int table_id = -1;
    sem_lock(sem_id);
    for (int i = 0; i < MAX_TABLES; i++) {
        if (!is_open) break; // Jeśli lokal został zamknięty w trakcie
        if (tables[i].capacity >= group_size && tables[i].occupied == 0) {
            tables[i].occupied = group_size;
            table_id = i;
            snprintf(tables[i].order_status, sizeof(tables[i].order_status), "Grupa %d (%d/%d)", group_id, group_size, tables[i].capacity);
            break;
        }
    }
    sem_unlock(sem_id);

    if (table_id == -1 || !is_open) {
        snprintf(log.content, sizeof(log.content), "[Klient ID:%d] Nie może wejść. Brak wolnych miejsc lub lokal zamknięty.", group_id);
        msgsnd(msg_id, &log, sizeof(log.content), 0);
        shmdt(tables);
        exit(EXIT_SUCCESS);
    }

    // Logowanie zajęcia stolika
    snprintf(log.content, sizeof(log.content), "[Klient ID:%d] Zajął stolik %d", group_id, table_id + 1);
    msgsnd(msg_id, &log, sizeof(log.content), 0);

    // Składanie zamówienia
    FILE *menu = fopen(MENU_FILE, "r");
    if (!menu) {
        snprintf(log.content, sizeof(log.content), "[Klient ID:%d] Nie udało się otworzyć menu", group_id);
        msgsnd(msg_id, &log, sizeof(log.content), 0);
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

    struct order_message order_msg;
    order_msg.msg_type = 1;
    order_msg.table_id = table_id;
    order_msg.group_id = group_id;
    snprintf(order_msg.order, sizeof(order_msg.order), "%s", items[choice]);
    order_msg.price = prices[choice];

    if (msgsnd(msg_id, &order_msg, sizeof(order_msg) - sizeof(long), 0) == -1) {
        snprintf(log.content, sizeof(log.content), "[Klient ID:%d] Nie udało się wysłać zamówienia", group_id);
        msgsnd(msg_id, &log, sizeof(log.content), 0);
        shmdt(tables);
        exit(EXIT_FAILURE);
    }

    snprintf(log.content, sizeof(log.content), "[Klient ID:%d] Zamówił %s (%.2f PLN)", group_id, items[choice], prices[choice]);
    msgsnd(msg_id, &log, sizeof(log.content), 0);

    // Symulacja jedzenia z ciągłą kontrolą zamknięcia lokalu
    for (int i = 0; i < 3; i++) {
        if (!is_open) {
            snprintf(log.content, sizeof(log.content), "[Klient ID:%d] Otrzymano sygnał zamknięcia lokalu. Opuszczam stolik.", group_id);
            msgsnd(msg_id, &log, sizeof(log.content), 0);
            break;
        }
        sleep(1); // Symulacja jedzenia (3 sekundy)
    }

    // Opuszczanie stolika
    sem_lock(sem_id);
    tables[table_id].occupied = 0;
    snprintf(tables[table_id].order_status, sizeof(tables[table_id].order_status), "Wolny (0/%d)", tables[table_id].capacity);
    sem_unlock(sem_id);

    snprintf(log.content, sizeof(log.content), "[Klient ID:%d] Opuszcza stolik %d", group_id, table_id + 1);
    msgsnd(msg_id, &log, sizeof(log.content), 0);

    shmdt(tables);
    exit(EXIT_SUCCESS);
}