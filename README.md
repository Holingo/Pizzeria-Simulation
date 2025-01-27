# Raport i README projektu: Pizzeria

## Informacje ogólne
**Autor:** Oskar Świątek  
**Numer albumu:** 152699  
**Temat projektu:** Pizzeria, Temat nr 9  
**Repozytorium:** [Link do GitHub](https://github.com/Holingo/Pizzeria-Simulation/)  

---

## Założenia projektowe
Projekt polega na stworzeniu symulacji pizzerii, która zarządza:

- Stolikami o różnej pojemności (1-, 2-, 3- i 4-osobowe).
- Klientami wchodzącymi do lokalu w grupach (1–4 osoby).
- Obsługą zamówień realizowanych przez kasjera.
- Obsługą sytuacji awaryjnych (np. pożar sygnalizowany sygnałem `SIGUSR1`).

System korzysta z procesów i wątków do symulacji współbieżnych działań oraz z mechanizmów IPC (kolejki komunikatów, pamięć współdzielona, semafory) do komunikacji i synchronizacji między elementami.

---

## Struktura projektu

### Pliki źródłowe
1. **`main.c`** [Link do kodu ](https://github.com/Holingo/Pizzeria-Simulation/blob/39ae00a1973232b8a0e7dfcfd632da460a91fe53/main.c#L36)
   - Zarządza główną pętlą programu.
   - Inicjalizuje zasoby i wątki.
   - Obsługuje sygnały (`SIGUSR1` i `SIGINT`).

2. **`cashier.c`** [Link do kodu ](https://github.com/Holingo/Pizzeria-Simulation/blob/39ae00a1973232b8a0e7dfcfd632da460a91fe53/cashier.c#L9)
   - Odpowiada za obsługę zamówień przez kasjera.
   - Aktualizuje status stolików i dochód.

3. **`client.c`** [Link do kodu ](https://github.com/Holingo/Pizzeria-Simulation/blob/39ae00a1973232b8a0e7dfcfd632da460a91fe53/client.c#L16)
   - Symuluje klientów wchodzących do pizzerii.
   - Tworzy procesy dla klientów, którzy zajmują stoliki i składają zamówienia.

4. **`firefighter.c`** [Link do kodu ](https://github.com/Holingo/Pizzeria-Simulation/blob/39ae00a1973232b8a0e7dfcfd632da460a91fe53/firefighter.c#L19)
   - Obsługuje sygnał pożaru (`SIGUSR1`).
   - Zamyka lokal w przypadku sytuacji awaryjnej.

5. **`utilities.c` i `utilities.h`** [Link do kodu 1](https://github.com/Holingo/Pizzeria-Simulation/blob/39ae00a1973232b8a0e7dfcfd632da460a91fe53/utilities.c#L48)[ Link do kodu 2](https://github.com/Holingo/Pizzeria-Simulation/blob/39ae00a1973232b8a0e7dfcfd632da460a91fe53/utilities.h#L1)
   - Zawiera funkcje pomocnicze, takie jak logowanie, zarządzanie interfejsem, obsługa semaforów i pamięci współdzielonej.

---

## Funkcjonalności
1. **Logowanie zdarzeń:**
   - Wszystkie kluczowe zdarzenia są rejestrowane w pliku `debug.log` oraz wyświetlane w konsoli.
2. **Interfejs użytkownika:**
   - Wyświetla status stolików, zamówienia i logi w przejrzysty sposób z użyciem kolorów.
3. **Obsługa sygnałów:**
   - `SIGUSR1` - sygnał pożaru, powodujący natychmiastowe zamknięcie lokalu.
   - `SIGINT` - sygnał zamknięcia programu.
4. **Synchronizacja:**
   - Wykorzystuje semafory do zarządzania dostępem do pamięci współdzielonej.
5. **Komunikacja między procesami:**
   - Kolejki komunikatów do przesyłania zamówień i logów.

---

## Kompilacja i uruchomienie

### Kompilacja
Użyj poniższego polecenia, aby skompilować projekt:
```bash
gcc -Wall -pthread main.c cashier.c client.c firefighter.c utilities.c -o pizzeria

albo 

make
```

### Uruchomienie
Aby uruchomić program, użyj:
```bash
./pizzeria [X1] [X2] [X3] [X4]
```
- X1, X2, X3, X4 - liczba stolików 1-, 2-, 3- i 4-osobowych.
- Przykład:
  ```bash
  ./pizzeria 1 1 1 1 (Domyślnie)
  ```

---

## Obsługa programu
1. **Interfejs użytkownika:**
   - Sekcje:
     - [STOLIKI] - pokazuje status stolików.
     - [DOCHÓD] - wyświetla łączny dochód.
     - [LOGI] - loguje wszystkie zdarzenia.
2. **Zamykanie programu:**
   - Naciśnij `Ctrl+C`, aby zamknąć program.
   - Możesz wysłać sygnał `SIGUSR1`, aby zasymulować pożar:
     ```bash
     kill -USR1 <PID>
     ```

---

## Testy

### Scenariusze testowe
1. **Standardowe działanie:**
   - Uruchomienie programu z różnymi konfiguracjami stolików.
   - Klienci wchodzą, zajmują stoliki i opuszczają lokal.
2. **Sygnał `SIGUSR1`:**
   - Wysłanie sygnału podczas działania programu.
   - Sprawdzenie, czy wszyscy klienci opuszczają lokal, a kasjer kończy pracę.
3. **Zamykanie programu (`SIGINT`):**
   - Sprawdzenie, czy wszystkie zasoby systemowe są poprawnie usuwane.
4. **Obsługa błędów:**
   - Próba uruchomienia programu bez wystarczających uprawnień.
   - Próba zajęcia stolika przez zbyt dużą grupę.

---

## Przykładowe wyniki
- Wszystkie logi zdarzeń są poprawnie wyświetlane i zapisywane do pliku.
- Po wysłaniu sygnału `SIGUSR1` wszyscy klienci opuszczają lokal, a program kontynuuje działanie.
- Program kończy działanie bez pozostawiania zasobów IPC.

---

## Struktury danych
- [Link do kodu ]**`Table`**(https://github.com/Holingo/Pizzeria-Simulation/blob/117f5d711715f1a151978dbfdbe5ca00d009f600/utilities.h#L53-L58) - przechowuje informacje o stolikach.
```c
typedef struct {
    int capacity;       // Liczba miejsc przy stoliku
    int occupied;       // Liczba zajętych miejsc
    char order_status[100]; // Status zamówienia
} Table;
```
- [Link do kodu ]**`order_message`**(https://github.com/Holingo/Pizzeria-Simulation/blob/117f5d711715f1a151978dbfdbe5ca00d009f600/utilities.h#L30-L37) - struktura zamówienia przesyłanego przez kolejkę komunikatów.
```c
struct order_message {
    long msg_type;      // Typ wiadomości
    int table_id;       // ID stolika
    int group_id;       // ID grupy klientów
    char order[100];    // Zamówienie
    float price;        // Cena zamówienia
};
```
- [Link do kodu ]**`log_message`**(https://github.com/Holingo/Pizzeria-Simulation/blob/117f5d711715f1a151978dbfdbe5ca00d009f600/utilities.h#L39-L42) - struktura do logowania zdarzeń.
```c
struct log_message {
    long msg_type;      // Typ wiadomości
    char content[200];  // Treść loga
};
```

---

## Linki do istotnych fragmentów kodu
1. **Tworzenie procesów:** [Link do kodu ](https://github.com/Holingo/Pizzeria-Simulation/blob/8baf40f4b5225a00f75f2fed852f2c1657d43369/client.c#L27)[`fork()`](#)[ w ](#)[`client.c`](#)
```c
        pid_t pid = fork();
        if (pid == 0) {
            pthread_mutex_destroy(&screen_mutex);
            pthread_mutex_init(&screen_mutex, NULL);


            srand(time(NULL) ^ getpid());
            int group_size = (rand() % 3) + 1;


            simulate_client(i + 1, group_size);
            exit(EXIT_SUCCESS);
```
2. **Tworzenie wątków:** [Link do kodu ](https://github.com/Holingo/Pizzeria-Simulation/blob/62f5e4b67b6a51fd38400a6715c707cc6a8def30/main.c#L56-L79)[`pthread_create()`](#)[ w ](#)[`main.c`](#)
```c
    if (pthread_create(&log_listener_tid, NULL, log_listener, NULL) != 0) {
        perror("Nie udało się utworzyć wątku nasłuchującego logi");
        cleanup_resources();
        exit(EXIT_FAILURE);
    }


    if (pthread_create(&cashier_tid, NULL, cashier_behavior, NULL) != 0) {
        perror("Nie udało się utworzyć wątku kasjera");
        cleanup_resources();
        exit(EXIT_FAILURE);
    }


    if (pthread_create(&firefighter_tid, NULL, firefighter_behavior, NULL) != 0) {
        perror("Nie udało się utworzyć wątku strażaka");
        cleanup_resources();
        exit(EXIT_FAILURE);
    }


    int num_clients = 5;
    if (pthread_create(&client_spawner_tid, NULL, client_spawner, &num_clients) != 0) {
        perror("Nie udało się utworzyć wątku klientów");
        cleanup_resources();
        exit(EXIT_FAILURE);
    }
```
3. **Obsługa sygnałów:** [Link do kodu ](https://github.com/Holingo/Pizzeria-Simulation/blob/62f5e4b67b6a51fd38400a6715c707cc6a8def30/firefighter.c#L20-L25)[`signal()`](#)[ w ](#)[`firefighter.c`](#)
```c
    struct sigaction sa;
    sa.sa_handler = handle_fire_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
```
4. **Synchronizacja semaforów:** [Link do kodu ](https://github.com/Holingo/Pizzeria-Simulation/blob/62f5e4b67b6a51fd38400a6715c707cc6a8def30/utilities.c#L113-L127)[`semop()`](#)[ w ](#)[`utilities.c`](#)
```c
void sem_lock(int sem_id) {
    struct sembuf op = {0, -1, 0};
    if (semop(sem_id, &op, 1) == -1) {
        perror("Semafor: błąd blokowania");
        exit(EXIT_FAILURE);
    }
}


void sem_unlock(int sem_id) {
    struct sembuf op = {0, 1, 0};
    if (semop(sem_id, &op, 1) == -1) {
        perror("Semafor: błąd odblokowywania");
        exit(EXIT_FAILURE);
    }
}
```
5. **Pamięć współdzielona:** [Link do kodu ](https://github.com/Holingo/Pizzeria-Simulation/blob/62f5e4b67b6a51fd38400a6715c707cc6a8def30/main.c#L126)[`shmget()`](#)[ w ](#)[`main.c`](#)
```c
    shm_id = shmget(SHM_KEY, sizeof(Table) * (X1 + X2 + X3 + X4), IPC_CREAT | 0666);
```
6. **Kolejki komunikatów:** [Link do kodu ](https://github.com/Holingo/Pizzeria-Simulation/blob/8baf40f4b5225a00f75f2fed852f2c1657d43369/client.c#L68-L70)[`msgsnd()`](#)[ w ](#)[`client.c`](#)
```c
    // Logowanie rozpoczęcia symulacji
    snprintf(log.content, sizeof(log.content), "[Klient ID:%d] Rozpoczyna symulację (grupa %d osób)", group_id, group_size);
    msgsnd(msg_id, &log, sizeof(log.content), 0);
```

---

## Podsumowanie
Projekt został zrealizowany zgodnie z wymaganiami, z uwzględnieniem dodatkowych funkcjonalności, takich jak kolorowy interfejs użytkownika i zaawansowana obsługa błędów. Wszystkie testy zakończyły się pomyślnie, a kod spełnia kryteria czytelności i modularności.