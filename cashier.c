#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "utilities.h" // Nagłówek wspólny dla wszystkich modułów

// Struktura komunikatu
struct message {
    long msg_type;
    char text[100];
};

void *cashier_behavior(void *arg) {
    printf("[Kasjer] Rozpoczynam pracę.\n");

    // Uzyskanie dostępu do kolejki komunikatów
    int msg_id = msgget(MSG_KEY, 0666);
    if (msg_id == -1) {
        perror("[Kasjer] Błąd uzyskiwania dostępu do kolejki komunikatów");
        return NULL;
    }

    while (1) {
        struct message msg;

        // Oczekiwanie na komunikat od klienta
        if (msgrcv(msg_id, &msg, sizeof(msg.text), 1, 0) == -1) {
            perror("[Kasjer] Błąd odbierania komunikatu");
            continue;
        }

        printf("[Kasjer] Otrzymano zamówienie: %s\n", msg.text);

        // Symulacja realizacji zamówienia
        sleep(2);
        printf("[Kasjer] Zamówienie '%s' zostało zrealizowane.\n", msg.text);

        // Sformatowanie potwierdzenia w buforze tymczasowym
        char temp_text[100];
        snprintf(temp_text, sizeof(temp_text), "Zamówienie '%s' zrealizowane.", msg.text);

        // Skopiowanie potwierdzenia do struktury komunikatu
        strncpy(msg.text, temp_text, sizeof(msg.text) - 1);
        msg.text[sizeof(msg.text) - 1] = '\0'; // Upewnienie się, że tekst jest zakończony null

        // Wysyłanie potwierdzenia realizacji zamówienia
        msg.msg_type = 2; // Typ komunikatu dla potwierdzeń

        if (msgsnd(msg_id, &msg, sizeof(msg.text), 0) == -1) {
            perror("[Kasjer] Błąd wysyłania potwierdzenia");
        }
    }

    return NULL;
}