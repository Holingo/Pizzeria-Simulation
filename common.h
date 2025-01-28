#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

// Konfiguracja stolików
#define X1 1  // Liczba stolików 1-osobowych
#define X2 1  // Liczba stolików 2-osobowych
#define X3 1  // Liczba stolików 3-osobowych
#define X4 1  // Liczba stolików 4-osobowych

// Konfiguracja symulacji
#define TOTAL_CUSTOMERS 10     // Łączna liczba grup
#define MAX_GROUP_SIZE 3       // Maksymalny rozmiar grupy
#define MIN_GROUP_SIZE 1       // Minimalny rozmiar grupy
#define RANDOM_DELAY_MAX 2     // Maksymalne opóźnienie między grupami
#define DELAY_BEFORE_FIREFIGHTER 10 // Opóźnienie przed uruchomieniem strażaka

// Dodaj pliki zapisujace
#define MENU_FILE "menu.txt"
#define EARNINGS_FILE "earnings.txt"

#define MAX_TABLES (X1+X2+X3+X4)
#define SHM_KEY 0x1234
#define SEM_KEY 0x5678
#define MSG_KEY 0x91011
#define REQUEST_MTYPE 1
#define RELEASE_MTYPE 2

typedef struct {
    int capacity;     // Pojemność stolika (1-4)
    int group_size;   // Rozmiar grupy która zajęła stolik (0 = wolny)
    int occupied;     // Zajęte miejsca
} Table;

typedef struct {
    char name[50];
    int price;
} Pizza;

typedef struct {
    Table tables[MAX_TABLES];
    int num_tables;
    volatile sig_atomic_t fire_alarm;
    pid_t customer_pids[TOTAL_CUSTOMERS];
    int num_customers;
    int total_served; // Liczba obsłużonych klientów
    int total_earnings;  // Nowe pole dla zarobków
} SharedData;

typedef struct {
    long mtype;
    pid_t pid;
    int group_size;
} RequestMsg;

typedef struct {
    long mtype;
    pid_t pid;
    int table_id;
    int group_size;
} ReleaseMsg;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

static inline void lock_sem(int semid) {
    struct sembuf op = {0, -1, 0};
    semop(semid, &op, 1);
}

static inline void unlock_sem(int semid) {
    struct sembuf op = {0, 1, 0};
    semop(semid, &op, 1);
}

#define RED_BG "\033[41m"
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\x1B[31m"
#define COLOR_GREEN "\x1B[32m"
#define COLOR_YELLOW "\x1B[33m"
#define COLOR_BLUE "\x1B[34m"
#define COLOR_MAGENTA "\x1B[35m"
#define COLOR_CYAN "\x1B[36m"
#define COLOR_WHITE "\033[37m"

#endif // COMMON_H