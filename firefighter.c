#include "common.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Użycie: %s <PID_kasjera>\n", argv[0]);
        exit(1);
    }

    pid_t cashier_pid = atoi(argv[1]);
    printf(COLOR_RED "[Strażak] Wysyłam sygnał pożaru do kasjera (PID: %d)\n" COLOR_RESET, cashier_pid);
    kill(cashier_pid, SIGUSR1);

    return 0;
}