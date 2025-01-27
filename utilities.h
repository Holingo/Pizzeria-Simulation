// Plik: utilities.h
// Opis: Plik nagłówkowy zawierający deklaracje funkcji i zmiennych wspólnych dla projektu pizzerii.

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
#include <sys/wait.h>

// Klucze IPC
#define SHM_KEY 1234
#define SEM_KEY 5678
#define MSG_KEY 9101

// Stałe
#define CLIENTS 5
#define MAX_LOGS 200
#define MAX_TABLES 20
#define MENU_FILE "menu.txt"

// Struktury danych
struct order_message {
    long msg_type;      // Typ wiadomości
    int table_id;       // ID stolika
    int group_id;       // ID grupy klientów
    char order[100];    // Zamówienie
    float price;        // Cena zamówienia
};

struct log_message {
    long msg_type;      // Typ wiadomości
    char content[200];  // Treść loga
};

// Zmienne globalne
extern pthread_mutex_t screen_mutex;
extern float total_revenue;
extern int log_count;
extern struct log_message log_messages[MAX_LOGS];

extern int is_open;
extern int shm_id, sem_id, msg_id;

// Struktura stolika
typedef struct {
    int capacity;       // Liczba miejsc przy stoliku
    int occupied;       // Liczba zajętych miejsc
    char order_status[100]; // Status zamówienia
} Table;

// Deklaracje funkcji
void log_event(const char *message);
void display_interface(Table *tables, int table_count);
void update_table_status(int table_index, const char *status);
void update_revenue(float amount);
void init_console();
void cleanup_console();

// Funkcje semaforów
void sem_lock(int sem_id);
void sem_unlock(int sem_id);

// Funkcje związane z klientami
void simulate_client(int group_id, int group_size);
void *client_spawner(void *arg);

// Funkcje związane z kasjerem
void *cashier_behavior(void *arg);

// Funkcje związane ze strażakiem
void *firefighter_behavior(void *arg);
void handle_fire_signal(int sig);

// Zarządzanie zasobami
void initialize_resources(int X1, int X2, int X3, int X4);
void cleanup_resources();

#endif