#ifndef UTILITIES_H
#define UTILITIES_H

#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>

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

// Inline functions to handle semaphores
static inline void sem_lock(int sem_id) {
    struct sembuf sb = {0, -1, 0}; // P operation (lock)
    if (semop(sem_id, &sb, 1) == -1) {
        perror("Semaphore operation error: lock");
    }
}

static inline void sem_unlock(int sem_id) {
    struct sembuf sb = {0, 1, 0}; // V operation (unlock)
    if (semop(sem_id, &sb, 1) == -1) {
        perror("Semaphore operation error: unlock");
    }
}


#endif // UTILITIES_H