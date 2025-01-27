// Plik: firefighter.c
// Opis: Plik obsługujący logikę zachowań strażaka w pizzerii. Strażak nasłuchuje sygnałów o pożarze (SIGUSR1) i zarządza procedurą awaryjną, w tym opuszczaniem lokalu przez klientów.

#include "utilities.h"
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

void handle_fire_signal(int sig) {
    if (sig == SIGUSR1) {
        log_event("[Strażak] Otrzymano sygnał pożaru! Wszyscy klienci opuszczają lokal.");
        is_open = 0; // Zamykanie pizzerii

        log_event("[Strażak] Lokal zostanie zamknięty.");
        sleep(2); // Symulacja reakcji na pożar
    }
}

void *firefighter_behavior(void *arg) {
    struct sigaction sa;
    sa.sa_handler = handle_fire_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("[Strażak] Błąd rejestracji obsługi sygnału");
        pthread_exit(NULL);
    }

    while (is_open) {
        pause(); // Oczekiwanie na sygnał pożaru
    }

    log_event("[Strażak] Lokal został zamknięty z powodu pożaru.");
    pthread_exit(NULL);
}
