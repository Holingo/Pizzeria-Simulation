#include "common.h"

SharedData *shared_data;
int msgqid, semid;

void sig_handler(int sig) {
    (void)sig;
    printf(COLOR_RED "[Klient PID: %d] Pożar! Uciekam!\n" COLOR_RESET, getpid());
    exit(0);
}

int load_menu(Pizza *menu, int max_pizzas) {
    FILE *fp = fopen(MENU_FILE, "r");
    if (!fp) {
        perror(COLOR_RED "[Klient] Błąd otwarcia menu" COLOR_RESET);
        return 0;
    }
    
    int i = 0;
    while (i < max_pizzas && fscanf(fp, "%49s %d", menu[i].name, &menu[i].price) == 2) {
        i++;
    }
    fclose(fp);
    return i;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Użycie: %s <liczba_osób>\n", argv[0]);
        exit(1);
    }

    time_t start_service = time(NULL); // Obliczanie czas procesu

    // Inicjalizacja
    int group_size = atoi(argv[1]);
    signal(SIGTERM, sig_handler);
    signal(SIGUSR1, sig_handler);

    // Wczytaj menu
    Pizza menu[10];
    int menu_size = load_menu(menu, 10);
    if (menu_size == 0) {
        fprintf(stderr, COLOR_RED "[Klient PID: %d] Brak dostępnych pizz!\n" COLOR_RESET, getpid());
        exit(1);
    }
    srand(time(NULL) ^ getpid()); // Przed wyborem pizzy
    int chosen_pizza = rand() % menu_size;

    // Sekcję łączenia z semaforem
    semid = semget(SEM_KEY, 1, 0666);
    if (semid == -1) {
        if (errno == ENOENT) {
            fprintf(stderr, COLOR_RED "[Klient PID: %d] Kasjer nieaktywny. Odrzucam.\n" COLOR_RESET, getpid());
            exit(0); // Wyjdź "miękko" zamiast zgłaszać błąd
        } else {
            perror("[Klient] Błąd semafora");
            exit(1);
        }
    }

    int shmid = shmget(SHM_KEY, sizeof(SharedData), 0666);
    shared_data = (SharedData*)shmat(shmid, NULL, 0);
    msgqid = msgget(MSG_KEY, 0666);

    if (shared_data->fire_alarm) {
        printf("[Klient] Pizzeria zamknięta. Wychodzę.\n");
        exit(0);
    }

    // Wyślij żądanie stolika
    RequestMsg req = {.mtype = REQUEST_MTYPE, .pid = getpid(), .group_size = group_size};
    msgsnd(msgqid, &req, sizeof(req) - sizeof(long), 0);

    // Odbierz odpowiedź
    ReleaseMsg resp;
    msgrcv(msgqid, &resp, sizeof(resp) - sizeof(long), getpid(), 0);
    shared_data->stats.total_messages_received++;

    if (resp.table_id == -1) {
        lock_sem(semid); // Zabezpieczenie aktualizacji danych współdzielonych
        shared_data->stats.total_customers_rejected++;
        unlock_sem(semid);
        
        printf(COLOR_RED "[Klient PID: %d] Brak stolika. Wychodzę.\n" COLOR_RESET, getpid());
        exit(0);
    }

    // Zaktualizuj zarobki (tylko jeśli dostał stolik!)
    lock_sem(semid);
    shared_data->total_earnings += menu[chosen_pizza].price;
    unlock_sem(semid);

    printf(COLOR_CYAN "[Klient PID: %d] Zamówił: %s (%d zł) | Stolik %d\n" COLOR_RESET,
           getpid(), menu[chosen_pizza].name, menu[chosen_pizza].price, resp.table_id);

    // Symulacja jedzenia
    printf(COLOR_WHITE "[Klient PID: %d] Jem pizzę...\n" COLOR_RESET, getpid());
    sleep(rand() % 3 + 1);

    // Zwolnij stolik
    ReleaseMsg rel = {.mtype = RELEASE_MTYPE, .pid = getpid(), .table_id = resp.table_id, .group_size = group_size};
    msgsnd(msgqid, &rel, sizeof(rel) - sizeof(long), 0);

    printf(COLOR_WHITE "[Klient PID: %d] Zjadłem. Wychodzę.\n" COLOR_RESET, getpid());
    
    // Po zakończeniu obsługi:
    time_t end_service = time(NULL);
    double service_time = difftime(end_service, start_service);
    shared_data->stats.average_service_time = 
        ((shared_data->stats.average_service_time * (shared_data->stats.total_customers_served - 1)) + service_time) 
        / shared_data->stats.total_customers_served;
    return 0;
}