#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "utilities.h" // Nagłówek wspólny dla wszystkich modułów

void handle_fire_signal(int sig) {
    if (sig == SIGUSR1) {
        printf("[Strażak] Otrzymano sygnał pożaru! Ewakuacja w toku ...\n");

        // Symulacja ewakuacji
        sleep(3);

        printf("[Strażak] Ewakuacja zakończona. Lokal jest pusty.\n");
        exit(EXIT_SUCCESS);
    }
}

void *firefighter_behavior(void *arg) {
    printf("[Strażak] W gotowości na sygnał pożaru.\n");

    // Rejestracja obsługi sygnału
    if (signal(SIGUSR1, handle_fire_signal) == SIG_ERR) {
        perror("[Strażak] Błąd rejestracji obsługi sygnału");
        return NULL;
    }

    // Niekończąca się pętla oczekiwania na sygnał
    while (1) {
        pause(); // Oczekiwanie na sygnał
    }
    
    return NULL;
}