// Updated utilities.h to align with console-based interface
#ifndef UTILITIES_H
#define UTILITIES_H

#include <pthread.h>
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

#define MAX_LOGS 200
#define MAX_TABLES 4
#define MENU_FILE "menu.txt"

struct order_message {
    long msg_type;
    int table_id;
    int group_id;
    char order[100];
    float price;
};

struct log_message {
    long msg_type; // Typ wiadomości
    char content[200]; // Treść loga
};

// Globalne zmienne logowania
extern pthread_mutex_t screen_mutex;
extern float total_revenue;
extern int log_count;
extern struct log_message log_messages[MAX_LOGS];

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
void display_interface(Table *tables, int table_count);
void update_table_status(int table_index, const char *status);
void update_revenue(float amount);
void init_console();
void cleanup_console();

// Semafory
void sem_lock(int sem_id);
void sem_unlock(int sem_id);

// Function prototypes
void simulate_client(int group_id, int group_size);
void *client_spawner(void *arg);
void send_log(const char *message);

// Pozar
void *firefighter_behavior(void *arg);
void handle_fire_signal(int sig);

// Kasjer
void *cashier_behavior(void *arg);

// Zarzadzanie zasobami
void initialize_resources(int X1, int X2, int X3, int X4);
void cleanup_resources();

#endif