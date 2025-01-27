// Updated firefighter.c to use console-based logging and interface
#include "utilities.h"
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

void handle_fire_signal(int sig) {
    if (sig == SIGUSR1) {
        log_event("[Strazak] Otrzymano sygnal pozaru! Wszyscy klienci opuszczaja lokal.");
        is_open = 0; // Zamykanie pizzerii

        // Informowanie o stanie pizzerii
        log_event("[Strazak] Lokal zostanie zamkniety.");
        sleep(2); // Symulacja reakcji na pozar
    }
}

void *firefighter_behavior(void *arg) {
    // Rejestracja obslugi sygnalu
    struct sigaction sa;
    sa.sa_handler = handle_fire_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("[Strazak] Blad rejestracji obslugi sygnalu");
        pthread_exit(NULL);
    }

    while (is_open) {
        pause(); // Oczekiwanie na sygnal pozaru
    }

    log_event("[Strazak] Lokal zostal zamkniety z powodu pozaru.");
    pthread_exit(NULL);
}