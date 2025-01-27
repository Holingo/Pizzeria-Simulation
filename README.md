# Projekt: Pizzeria

## Opis projektu
Symulacja pizzerii zarządzanej przy użyciu procesów i wątków, w której klienci zamawiają pizzę, zajmują stoliki, a kasjer obsługuje zamówienia. Strażak odpowiada za sytuacje awaryjne, takie jak sygnał o pożarze. Program wykorzystuje zaawansowane mechanizmy IPC (pamięć współdzielona, kolejki komunikatów, semafory) oraz wątki do zarządzania konkurencją między procesami.

---

## Struktura projektu

### Pliki źródłowe
- **main.c**
  - Zarządza główną pętlą programu.
  - Inicjalizuje zasoby i wątki.
  - Obsługuje sygnały.

- **cashier.c**
  - Odpowiada za obsługę zamówień przez kasjera.
  - Aktualizuje status stolików i dochód.

- **client.c**
  - Symuluje klientów wchodzących do pizzerii.
  - Tworzy procesy dla klientów, którzy zajmują stoliki i składają zamówienia.

- **firefighter.c**
  - Obsługuje sygnał pożaru (SIGUSR1).
  - Zamyka lokal w przypadku sytuacji awaryjnej.

- **utilities.c / utilities.h**
  - Zawiera funkcje pomocnicze, takie jak logowanie, zarządzanie interfejsem, obsługa semaforów i pamięci współdzielonej.

---

## Funkcjonalności
1. **Logowanie zdarzeń:**
   - Wszystkie kluczowe zdarzenia są rejestrowane w pliku `debug.log` oraz wyświetlane w konsoli.
2. **Interfejs użytkownika:**
   - Wyświetla status stolików, zamówienia i logi w przejrzysty sposób z użyciem kolorów.
3. **Obsługa sygnałów:**
   - SIGUSR1 - sygnał pożaru, powodujący natychmiastowe zamknięcie lokalu.
   - SIGINT - sygnał zamknięcia programu.
4. **Synchronizacja:**
   - Wykorzystuje semafory do zarządzania dostępem do pamięci współdzielonej.
5. **Komunikacja między procesami:**
   - Kolejki komunikatów do przesyłania zamówień i logów.

---

## Wymagania techniczne
- **System operacyjny:** Linux (wymagany do obsługi funkcji IPC).
- **Kompilator:** GCC (z obsługą `-pthread`).
- **Biblioteki:**
  - `pthread`
  - `sys/ipc.h`
  - `sys/msg.h`
  - `sys/sem.h`
  - `sys/shm.h`

---

## Kompilacja i uruchomienie

### Kompilacja
Użyj poniższego polecenia, aby skompilować projekt:
```bash
gcc -Wall -pthread main.c cashier.c client.c firefighter.c utilities.c -o pizzeria
```

### Uruchomienie
Aby uruchomić program, użyj:
```bash
./pizzeria [X1] [X2] [X3] [X4]
```
- X1, X2, X3, X4 - liczba stolików 1-, 2-, 3- i 4-osobowych.
- Przykład:
  ```bash
  ./pizzeria 2 2 2 2
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

## Struktury danych
- **`Table`** - przechowuje informacje o stolikach.
- **`order_message`** - struktura zamówienia przesyłanego przez kolejkę komunikatów.
- **`log_message`** - struktura do logowania zdarzeń.

---

## Informacje dodatkowe
- Plik `debug.log` przechowuje logi działania programu.
- Program automatycznie usuwa wszystkie zasoby IPC po zakończeniu pracy.

---

## Autor
Project power by Oskar Świątek.

