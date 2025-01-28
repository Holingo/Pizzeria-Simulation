#include "common.h"

pid_t launch_process(const char *program, char *arg) {
    pid_t pid = fork();
    switch(pid) {
        case -1:
            perror("fork failed");
            exit(EXIT_FAILURE);
        
        case 0: // Child
            if(arg) {
                execl(program, program, arg, (char *)NULL);
            } else {
                execl(program, program, (char *)NULL);
            }
            perror("execl failed");
            exit(EXIT_FAILURE);
        
        default: // Parent
            printf(COLOR_YELLOW "[Main] Uruchomiono %s (PID: %d)\n" COLOR_RESET, program, pid);
            return pid;
    }
}

int main() {
    srand(time(NULL));
    
    // Uruchomienie kasjera
    pid_t cashier_pid = launch_process("./cashier", NULL);
    sleep(2);
    
    // Podłącz się do pamięci współdzielonej
    int shmid = shmget(SHM_KEY, sizeof(SharedData), 0666);
    SharedData *shared_data = (SharedData*)shmat(shmid, NULL, 0);

    // Pętlę generującą klientów
    for(int i=0; i<TOTAL_CUSTOMERS; i++) {
        // Sprawdź czy kasjer nadal działa
        int cashier_status;
        if (waitpid(cashier_pid, &cashier_status, WNOHANG) != 0 || shared_data->fire_alarm) {
            printf(COLOR_RED "[Main] Kasjer zakończył działanie. Przerywanie generacji klientów.\n" COLOR_RESET);
            break;
        }

        // Reszta kodu generującego klientów
        int group_size = (rand() % (MAX_GROUP_SIZE - MIN_GROUP_SIZE + 1)) + MIN_GROUP_SIZE;
        char group_str[10];
        snprintf(group_str, sizeof(group_str), "%d", group_size);

        launch_process("./customer", group_str);
        shared_data->stats.total_customers_generated++; // Do statystyki
        sleep(rand() % (RANDOM_DELAY_MAX + 1));
    }

    /*/ Uruchomienie strażaka z poprawnym PID kasjera
    sleep(DELAY_BEFORE_FIREFIGHTER);
    char pid_str[20];
    snprintf(pid_str, sizeof(pid_str), "%d", cashier_pid);
    launch_process("./firefighter", pid_str);*/

    // Zakończ główny proces, jeśli kasjer nie żyje
    sleep(DELAY_BEFORE_FIREFIGHTER);
    kill(cashier_pid, SIGTERM); // Upewnij się, że kasjer jest zabity

    // Oczekiwanie na zakończenie kasjera
    int status;
    while(wait(&status) > 0);
    waitpid(cashier_pid, &status, 0);

    // Wyświetlanie statystyk
    //print_statistics();
    //print_resource_usage();

    // Odbierz zasoby IPC
    shmdt(shared_data);
    printf(COLOR_YELLOW "[Main] Wszystkie procesy zakończone\n" COLOR_RESET);
    return 0;
}