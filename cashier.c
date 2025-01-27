// Plik: cashier.c
// Opis: Plik obsługujący logikę pracy kasjera w pizzerii. Kasjer odbiera zamówienia, aktualizuje status stolików i zarządza przychodem lokalu.

#include "utilities.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

void *cashier_behavior(void *arg) {
    log_event("[Kasjer] Rozpoczynam pracę");

    while (is_open) {
        struct order_message msg;

        // Funkcja służy do odbierania wiadomości z kolejki komunikatów w systemie IPC (Inter Process Communication).
        if (msgrcv(msg_id, &msg, sizeof(msg) - sizeof(long), 1, 0) != -1) {
            char log_message[200];
            snprintf(log_message, sizeof(log_message), "[Kasjer] Realizacja zamówienia dla stolika %d. Zamówiono: %s (%.2f PLN)",
                     msg.table_id + 1, msg.order, msg.price);
            log_event(log_message);

            update_table_status(msg.table_id, "Realizacja w toku");
            sleep(1); // Symulacja realizacji zamówienia

            snprintf(log_message, sizeof(log_message), "[Kasjer] Zamówienie dla stolika %d zostało zrealizowane (%.2f PLN)",
                     msg.table_id + 1, msg.price);
            log_event(log_message);

            update_revenue(msg.price);
            update_table_status(msg.table_id, "Zrealizowane");
        } else {
            usleep(100000); // Krótka przerwa, gdy brak zamówień
        }
    }

    log_event("[Kasjer] Pizzeria zamknięta, kończę pracę");
    return NULL;
}