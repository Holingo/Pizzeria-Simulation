// Updated cashier.c with improved logging and revenue tracking
#include "utilities.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

void *cashier_behavior(void *arg) {
    log_event("[Kasjer] Rozpoczynam prace");

    while (is_open) {
        struct order_message msg;

        if (msgrcv(msg_id, &msg, sizeof(msg) - sizeof(long), 1, IPC_NOWAIT) != -1) {
            char log_message[200];

            snprintf(log_message, sizeof(log_message), "[Kasjer] Realizacja zamowienia dla stolika %d. Zamowiono: %s (%.2f PLN)",
                     msg.table_id + 1, msg.order, msg.price);
            log_event(log_message);

            update_order_status(msg.table_id, "Realizacja w toku");
            sleep(2); // Symulacja realizacji zamowienia

            snprintf(log_message, sizeof(log_message), "[Kasjer] Zamowienie dla stolika %d zostalo zrealizowane (%.2f PLN)", 
                     msg.table_id + 1, msg.price);
            log_event(log_message);

            update_revenue(msg.price);
            update_order_status(msg.table_id, "Zrealizowane");
        } else {
            usleep(100000); // Krotka przerwa, gdy brak zamowien
        }
    }

    log_event("[Kasjer] Pizzeria zamknieta, koncze prace");
    return NULL;
}