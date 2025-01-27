// Updated client.c with improved logging and clearer order handling
#include "utilities.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <time.h>

void simulate_client(int group_id, int group_size) {
    // Dolaczenie do segmentu pamieci dzielonej
    Table *tables = shmat(shm_id, NULL, 0);
    if (tables == (void *)-1) {
        perror("[Klient] Blad dolaczania pamieci dzielonej");
        exit(EXIT_FAILURE);
    }

    // Wybieranie stolika
    int table_id = -1;
    sem_lock(sem_id);
    for (int i = 0; i < MAX_TABLES; i++) {
        if (tables[i].capacity - tables[i].occupied >= group_size) {
            tables[i].occupied += group_size;
            table_id = i;
            snprintf(tables[i].order_status, sizeof(tables[i].order_status), "Grupa %d", group_id);
            break;
        }
    }
    sem_unlock(sem_id);

    if (table_id == -1) {
        log_event("[Klient] Brak wolnych stolikow");
        shmdt(tables);
        exit(EXIT_FAILURE);
    }

    char log_message[200];
    snprintf(log_message, sizeof(log_message), "[Klient ID:%d] Grupa %d zajmuje stolik %d", group_id, group_size, table_id + 1);
    log_event(log_message);

    update_table_status(table_id, tables[table_id].occupied, tables[table_id].capacity, "Zajety");

    // Symulacja zamowienia
    FILE *menu = fopen(MENU_FILE, "r");
    if (!menu) {
        perror("[Klient] Nie udalo sie otworzyc menu");
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
        perror("[Klient] Nie udalo sie wyslac zamowienia");
        shmdt(tables);
        exit(EXIT_FAILURE);
    }

    snprintf(log_message, sizeof(log_message), "[Klient ID:%d] Zamowil %s (%.2f PLN)", group_id, msg.order, msg.price);
    log_event(log_message);
    update_order_status(table_id, "Oczekuje na realizacje");

    sleep(3); // Symulacja jedzenia

    sem_lock(sem_id);
    tables[table_id].occupied -= group_size;
    snprintf(tables[table_id].order_status, sizeof(tables[table_id].order_status), "Wolny");
    sem_unlock(sem_id);

    snprintf(log_message, sizeof(log_message), "[Klient ID:%d] Grupa opuszcza stolik %d", group_id, table_id + 1);
    log_event(log_message);

    update_table_status(table_id, tables[table_id].occupied, tables[table_id].capacity, "Wolny");

    shmdt(tables);
    exit(EXIT_SUCCESS);
}
