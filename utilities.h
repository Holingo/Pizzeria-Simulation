#ifndef UTILITIES_H
#define UTILITIES_H

#include <sys/sem.h>

// Klucze do IPC
#define SHM_KEY 1234
#define SEM_KEY 5678
#define MSG_KEY 9101

// Maksymalna liczba stolików
#define MAX_TABLES 10

// Struktura reprezentująca stan stolików
typedef struct {
    int tables[MAX_TABLES]; // Liczba miejsc przy każdym stoliku
} TableState;

// Inline funkcje do obsługi semaforów
static inline void sem_lock(int sem_id) {
    struct sembuf sb = {0, -1, 0};
    if (semop(sem_id, &sb, 1) == -1) {
        perror("Błąd operacji semafora: lock");
    }
}

static inline void sem_unlock(int sem_id) {
    struct sembuf sb = {0, 1, 0};
    if (semop(sem_id, &sb, 1) == -1) {
        perror("Błąd operacji semafora: unlock");
    }
}

#endif // UTILITIES_H