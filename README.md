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
- **`Table`** - przechowuje informacje o stolikach.
https://github.com/Holingo/Pizzeria-Simulation/blob/117f5d711715f1a151978dbfdbe5ca00d009f600/utilities.h#L53-L58

- **`order_message`** - struktura zamówienia przesyłanego przez kolejkę komunikatów.
https://github.com/Holingo/Pizzeria-Simulation/blob/117f5d711715f1a151978dbfdbe5ca00d009f600/utilities.h#L30-L37

- **`log_message`** - struktura do logowania zdarzeń.
https://github.com/Holingo/Pizzeria-Simulation/blob/117f5d711715f1a151978dbfdbe5ca00d009f600/utilities.h#L39-L42

---

## Linki do istotnych fragmentów kodu
1. **Tworzenie procesów:** [Link do kodu ](https://github.com/Holingo/Pizzeria-Simulation/blob/ea16ec1b44f86495edf30289592be05284fdf39a/client.c#L27-L41)[`fork()`](#)[ w ](#)[`client.c`](#)
https://github.com/Holingo/Pizzeria-Simulation/blob/ea16ec1b44f86495edf30289592be05284fdf39a/client.c#L27-L41

2. **Tworzenie wątków:** [Link do kodu ](https://github.com/Holingo/Pizzeria-Simulation/blob/62f5e4b67b6a51fd38400a6715c707cc6a8def30/main.c#L56-L79)[`pthread_create()`](#)[ w ](#)[`main.c`](#)
https://github.com/Holingo/Pizzeria-Simulation/blob/62f5e4b67b6a51fd38400a6715c707cc6a8def30/main.c#L56-L79

3. **Obsługa sygnałów:** [Link do kodu ](https://github.com/Holingo/Pizzeria-Simulation/blob/62f5e4b67b6a51fd38400a6715c707cc6a8def30/firefighter.c#L20-L25)[`signal()`](#)[ w ](#)[`firefighter.c`](#)
https://github.com/Holingo/Pizzeria-Simulation/blob/62f5e4b67b6a51fd38400a6715c707cc6a8def30/firefighter.c#L20-L25

4. **Synchronizacja semaforów:** [Link do kodu ](https://github.com/Holingo/Pizzeria-Simulation/blob/62f5e4b67b6a51fd38400a6715c707cc6a8def30/utilities.c#L113-L127)[`semop()`](#)[ w ](#)[`utilities.c`](#)
https://github.com/Holingo/Pizzeria-Simulation/blob/62f5e4b67b6a51fd38400a6715c707cc6a8def30/utilities.c#L113-L127

5. **Pamięć współdzielona:** [Link do kodu ](https://github.com/Holingo/Pizzeria-Simulation/blob/62f5e4b67b6a51fd38400a6715c707cc6a8def30/main.c#L126)[`shmget()`](#)[ w ](#)[`main.c`](#)
https://github.com/Holingo/Pizzeria-Simulation/blob/62f5e4b67b6a51fd38400a6715c707cc6a8def30/main.c#L126

6. **Kolejki komunikatów:** [Link do kodu ](https://github.com/Holingo/Pizzeria-Simulation/blob/8baf40f4b5225a00f75f2fed852f2c1657d43369/client.c#L68-L70)[`msgsnd()`](#)[ w ](#)[`client.c`](#)
https://github.com/Holingo/Pizzeria-Simulation/blob/8baf40f4b5225a00f75f2fed852f2c1657d43369/client.c#L68-L70

---

## Podsumowanie
Projekt został zrealizowany zgodnie z wymaganiami, z uwzględnieniem dodatkowych funkcjonalności, takich jak kolorowy interfejs użytkownika i zaawansowana obsługa błędów. Wszystkie testy zakończyły się pomyślnie, a kod spełnia kryteria czytelności i modularności.
