# Raport z projektu: Pizzeria NR9

## Informacje ogólne

**Autor:** Oskar Świątek\
**Numer albumu:** [Numer Albumu]\
**Temat projektu:** Pizzeria, Temat nr. 9\
**Repozytorium:** [\[Link do GitHub\]](https://github.com/Holingo/Pizzeria-Simulation/)

---

## Założenia projektowe

Projekt polega na stworzeniu symulacji pizzerii, która zarządza:

- Stolikami o różnej pojemności (1-, 2-, 3- i 4-osobowe).
- Klientami wchodzącymi do lokalu w grupach (1–4 osoby).
- Obsługą zamówień realizowanych przez kasjera.
- Obsługą sytuacji awaryjnych (np. pożar sygnalizowany sygnałem `SIGUSR1`).

System korzysta z procesów i wątków do symulacji współbieżnych działań oraz z mechanizmów IPC (kolejki komunikatów, pamięć współdzielona, semafory) do komunikacji i synchronizacji między elementami.

---

## Opis kodu

### Struktura projektu

1. **`main.c`**

   - Inicjalizuje zasoby systemowe, takie jak pamięć współdzielona, kolejki komunikatów i semafory.
   - Tworzy wątki: kasjera, strażaka, klienta oraz log listenera.
   - Zarządza główną pętlą programu i reaguje na sygnały systemowe (`SIGUSR1` i `SIGINT`).

2. **`cashier.c`**

   - Kasjer odbiera zamówienia z kolejki komunikatów.
   - Aktualizuje status stolików i dochód pizzerii.

3. **`client.c`**

   - Generuje procesy klientów wchodzących do lokalu.
   - Każdy klient zajmuje stolik, składa zamówienie i opuszcza lokal w przypadku zamknięcia.

4. **`firefighter.c`**

   - Obsługuje sygnał `SIGUSR1` (pożar).
   - Powoduje wyjście wszystkich klientów z lokalu i zamknięcie kasjera.

5. **`utilities.c`**\*\* i \*\***`utilities.h`**

   - Zawiera funkcje wspólne, takie jak logowanie, obsługa interfejsu użytkownika, zarządzanie semaforami i pamięcią współdzieloną.
   - Obsługuje synchronizację i wyświetlanie w terminalu.

### Najważniejsze funkcjonalności

- **Logowanie zdarzeń:** Wszystkie działania są rejestrowane w pliku `debug.log` i wyświetlane w terminalu.
- **Interfejs użytkownika:** Dynamicznie aktualizowany widok z podziałem na sekcje:
  - Status stolików.
  - Łączny dochód.
  - Lista logów.
- **Obsługa sygnałów:**
  - `SIGUSR1`: Pożar – klienci opuszczają lokal.
  - `SIGINT`: Zamknięcie programu.
- **Synchronizacja:** Semafory do ochrony pamięci współdzielonej.
- **Komunikacja między procesami:** Kolejki komunikatów do wymiany zamówień i logów.

---

## Co udało się zrobić

- Zaimplementowano wszystkie wymagane funkcjonalności opisane w temacie projektu.
- Obsłużono dynamiczne aktualizowanie interfejsu użytkownika z użyciem kolorów.
- Zapewniono obsługę błędów dla funkcji systemowych (`perror`, `errno`).
- Użyto semaforów, pamięci współdzielonej oraz kolejek komunikatów z minimalnymi prawami dostępu.
- Wszystkie zasoby systemowe są usuwane po zakończeniu programu.

---

## Problemy napotkane podczas realizacji

1. **Obsługa sygnału ********`SIGUSR1`******** dla wszystkich procesów:**
   - Rozwiązano, rejestrując sygnał dla każdego procesu i odpowiednio reagując na jego odbiór.
2. **Synchronizacja dostępu do stolików:**
   - Wdrożono semafory, aby zapobiec sytuacjom wyścigu między procesami klientów.
3. **Odświeżanie interfejsu po zamknięciu lokalu:**
   - Zmieniono logikę pętli głównej, aby odświeżała interfejs aż do całkowitego zwolnienia stolików.

---

## Dodane elementy specjalne

- Kolorowy interfejs użytkownika ułatwiający czytanie logów i monitorowanie stanu pizzerii.
- Obsługa błędów dla wszystkich kluczowych funkcji systemowych.
- Logowanie w czasie rzeczywistym do pliku i terminala.

---

## Testy

### Scenariusze testowe

1. **Standardowe działanie:**
   - Uruchomienie programu z różnymi konfiguracjami stolików.
   - Klienci wchodzą, zajmują stoliki i opuszczają lokal.
2. **Sygnał ********`SIGUSR1`********:**
   - Wysłanie sygnału podczas działania programu.
   - Sprawdzenie, czy wszyscy klienci opuszczają lokal, a kasjer kończy pracę.
3. **Zamykanie programu (********`SIGINT`********):**
   - Sprawdzenie, czy wszystkie zasoby systemowe są poprawnie usuwane.
4. **Obsługa błędów:**
   - Próba uruchomienia programu bez wystarczających uprawnień.
   - Próba zajęcia stolika przez zbyt dużą grupę.

### Przykładowe wyniki

- Wszystkie logi zdarzeń są poprawnie wyświetlane i zapisywane do pliku.
- Po wysłaniu sygnału `SIGUSR1` wszyscy klienci opuszczają lokal, a program kontynuuje działanie.
- Program kończy działanie bez pozostawiania zasobów IPC.

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

**Autor:** Oskar Świątek

