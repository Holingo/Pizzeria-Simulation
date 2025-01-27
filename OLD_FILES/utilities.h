// Updated utilities.h for better resource management and consistency
#ifndef UTILITIES_H
#define UTILITIES_H

#include <pthread.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>

// Klucze IPC
#define SHM_KEY 1234
#define SEM_KEY 5678
#define MSG_KEY 9101

#define MAX_LOGS 100
#define MAX_TABLES 4
#define MAX_ORDERS 4
#define MENU_FILE "menu.txt"

struct order_message {
    long msg_type;
    int table_id;
    int group_id;
    char order[100];
    float price;
};

// Globalne zmienne dla logow
extern pthread_mutex_t screen_mutex;
extern float total_revenue;
extern int log_count;
extern char log_messages[MAX_LOGS][200];

// Globalne zmienne IPC
extern int is_open;
extern int shm_id, sem_id, msg_id;

// Struktura stolika
typedef struct {
    int capacity;    // Liczba miejsc
    int occupied;    // Liczba zajetych miejsc
    char order_status[100]; // Status zamowienia
} Table;

// Deklaracja funkcji logowania
void log_event(const char *message);
void update_table_status(int table_index, int occupied, int capacity, const char *status);
void update_order_status(int table_index, const char *status);
void update_revenue(float amount);

// Ncurses utility functions
void init_ncurses();
void end_ncurses();
void draw_interface();

// Semafory
void sem_lock(int sem_id);
void sem_unlock(int sem_id);

// Klienci
void simulate_client(int group_id, int group_size);

// Pozar
void *firefighter_behavior(void *arg);
void handle_fire_signal(int sig);

// Kasjer
void *cashier_behavior(void *arg);

// Zarzadzanie zasobami
void initialize_resources();
void cleanup_resources();

#endif