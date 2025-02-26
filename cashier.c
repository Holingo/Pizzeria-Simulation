#include "common.h"

SharedData *shared_data;
int shmid, semid, msgqid;

// Deklaracje funkcji
void print_statistics();
void print_resource_usage();

void cleanup() {
    // Usuń wszystkie zasoby IPC, nawet jeśli kasjer jest "właścicielem"
    shmdt(shared_data);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID, 0);
    msgctl(msgqid, IPC_RMID, NULL);
}

void sig_handler(int sig) {
    if (sig == SIGUSR1) {
        printf(COLOR_RED "\n[Kasjer] POŻAR! Ewakuacja wszystkich klientów!\n" COLOR_RESET);
        shared_data->fire_alarm = 1; // Oznacz pożar w pamięci współdzielonej

        // Najpierw zabij wszystkich klientów
        for (int i = 0; i < shared_data->num_customers; i++) {
            kill(shared_data->customer_pids[i], SIGTERM);
        }

        // Natychmiastowe zniszczenie zasobów IPC
        print_statistics();
        print_resource_usage();
        cleanup();
        exit(0); // Zamknij kasjera BEZ CZEKANIA
    }
}

void init_resources() {
    shmid = shmget(SHM_KEY, sizeof(SharedData), IPC_CREAT | 0666);
    shared_data = (SharedData*)shmat(shmid, NULL, 0);
    
    // Inicjalizacja stolików z konfiguracji
    int table_config[] = {X1, X2, X3, X4};
    int capacities[] = {1, 2, 3, 4}; 
    shared_data->num_tables = 0;
    
    for(int i=0; i<4; i++) {
        for(int j=0; j<table_config[i]; j++) {
            if(shared_data->num_tables >= MAX_TABLES) break;
            shared_data->tables[shared_data->num_tables].capacity = capacities[i];
            shared_data->tables[shared_data->num_tables].group_size = 0;
            shared_data->tables[shared_data->num_tables].occupied = 0;
            shared_data->num_tables++;
        }
    }
    
    shared_data->fire_alarm = 0;
    shared_data->num_customers = 0;
    shared_data->total_served = 0;    // Dodano inicjalizację
    shared_data->total_earnings = 0;  // Dodano inicjalizację

    // Zwiazane dodawaniem informacji na koncu do raportu projektu
    shared_data->stats.total_customers_generated = 0;
    shared_data->stats.total_customers_served = 0;
    shared_data->stats.total_customers_rejected = 0;
    shared_data->stats.total_messages_received = 0;
    shared_data->stats.average_service_time = 0.0;
    shared_data->stats.start_time = time(NULL);
    shared_data->stats.end_time = 0;

    semid = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);

    msgqid = msgget(MSG_KEY, IPC_CREAT | 0666);
}

int assign_table(int group_size, pid_t pid) {
    lock_sem(semid);
    int table_id = -1;

    for(int i=0; i<shared_data->num_tables; i++) {
        Table *t = &shared_data->tables[i];
        
        if(group_size > t->capacity) continue;
        
        if(t->group_size == 0 || t->group_size == group_size) {
            if(t->occupied + group_size <= t->capacity) {
                t->group_size = group_size;
                t->occupied += group_size;
                table_id = i;
                shared_data->total_served++; // Dodano!
                shared_data->stats.total_customers_served++;
                break;
            }
        }
    }

    if(table_id != -1) {
        shared_data->customer_pids[shared_data->num_customers++] = pid;
    }

    unlock_sem(semid);
    return table_id;
}

void process_release(pid_t pid, int table_id, int group_size) {
    lock_sem(semid);
    Table *t = &shared_data->tables[table_id];
    t->occupied -= group_size;
    if (t->occupied == 0) t->group_size = 0;

    for (int i = 0; i < shared_data->num_customers; i++) {
        if (shared_data->customer_pids[i] == pid) {
            shared_data->customer_pids[i] = shared_data->customer_pids[--shared_data->num_customers];
            break;
        }
    }
    unlock_sem(semid);
}

int main() {
    signal(SIGUSR1, sig_handler);
    init_resources();
    printf("[Kasjer] PID: %d\n", getpid());

    while (1) {
        RequestMsg req;
        if (msgrcv(msgqid, &req, sizeof(req) - sizeof(long), REQUEST_MTYPE, IPC_NOWAIT) > 0) {
            int table_id = assign_table(req.group_size, req.pid);
            ReleaseMsg resp = {.mtype = req.pid, .table_id = table_id, .group_size = req.group_size};
            msgsnd(msgqid, &resp, sizeof(resp) - sizeof(long), 0);
            printf("[Kasjer] Grupa %d (PID %d) dostała stolik %d\n", req.group_size, req.pid, table_id);
        }

        ReleaseMsg rel;
        if (msgrcv(msgqid, &rel, sizeof(rel) - sizeof(long), RELEASE_MTYPE, IPC_NOWAIT) > 0) {
            process_release(rel.pid, rel.table_id, rel.group_size);
            printf("[Kasjer] Stolik %d zwolniony przez grupę %d (PID %d)\n", rel.table_id, rel.group_size, rel.pid);
        }

       if (shared_data->total_served + shared_data->stats.total_customers_rejected >= TOTAL_CUSTOMERS && shared_data->num_customers == 0) {
            printf(COLOR_GREEN "[Kasjer] Wszyscy klienci obsłużeni. Łączna kwota: %d zł. Zamykanie reustaracji.\n" COLOR_RESET, shared_data->total_earnings);
            print_statistics();
            print_resource_usage();
            cleanup();
            exit(0);
        }
    }
}

void print_statistics() {
    lock_sem(semid); // Zabezpieczenie dostępu do shared_data
    shared_data->stats.end_time = time(NULL);
    double total_duration = difftime(shared_data->stats.end_time, shared_data->stats.start_time);

    printf(COLOR_YELLOW "\n= = = Statystyki programu = = =\n" COLOR_RESET);
    printf("Czas trwania programu: %.2f sekund\n", total_duration);
    printf("Łączna liczba klientów wygenerowanych: %d\n", shared_data->stats.total_customers_generated);
    printf("Liczba klientów obsłużonych: %d\n", shared_data->stats.total_customers_served);
    printf("Liczba klientów odrzuconych: %d\n", shared_data->stats.total_customers_rejected);
    printf("Łączna liczba odebranych komunikatów: %d\n", shared_data->stats.total_messages_received);
    printf("Średni czas obsługi klienta: %.2f sekund\n", shared_data->stats.average_service_time);
    unlock_sem(semid);
}

void print_resource_usage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);

    printf("Maksymalne użycie pamięci: %ld KB\n", usage.ru_maxrss);
    printf("Łączne obciążenie CPU: %.2f sekund\n", 
           usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1e6 +
           usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1e6);
}