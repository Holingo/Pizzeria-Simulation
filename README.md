# Projekt: Symulacja Restauracji z Obsługą Wieloprocesową

## Założenia projektowe
Projekt symuluje działanie pizzerii, w której kelner zarządza restauracją, przydziela stoliki grupom klientów oraz obsługuje zdarzenia związane z funkcjonowaniem restauracji. System opiera się na procesach i mechanizmach systemowych dostępnych w UNIX/Linux, takich jak pamięć współdzielona, semafory, kolejki komunikatów oraz obsługa sygnałów.

---

## Opis kodu

### Struktura projektu
1. **`main.c`**:
   - Główna pętla programu.
   - Tworzy procesy klientów oraz kasjera.
   - Monitoruje działanie kasjera i uruchamia strażaka w razie potrzeby.
   
2. **`cashier.c`**:
   - Zarządza zasobami restauracji, w tym przydziałem stolików.
   - Obsługuje sygnał pożaru, ewakuując klientów i czyszcząc zasoby.

3. **`customer.c`**:
   - Klienci rezerwują stoliki, zamawiają pizzę i uwalniają zasoby po zakończeniu posiłku.
   - Obsługuje reakcje na sygnały, takie jak ewakuacja.

4. **`firefighter.c`**:
   - Odpowiada za symulację pożaru poprzez wysłanie odpowiedniego sygnału do procesu kasjera.

5. **`common.h`**:
   - Zawiera wspólne definicje struktur i funkcji pomocniczych, takich jak pamięć współdzielona, semafory i kolory terminala.

---

### Opis struktury Reustaracji

#### Stoliki
- Restauracja dysponuje stolikami różnych rozmiarów:
  - 1-osobowe.
  - 2-osobowe.
  - 3-osobowe.
  - 4-osobowe.
- Domyślnie każdy rodzaj stolika występuje w liczbie 1. Liczba stolików może być zmieniana w konfiguracji.

#### Grupy Klientów
- Do restauracji wchodzą grupy klientów liczące od 1 do 3 osób.
- Przydział stolików:
  - Grupa 1-osobowa może zająć dowolny stolik.
  - Grupa 2-osobowa wymaga stolika 2-osobowego lub większego.
  - Grupa 3-osobowa wymaga stolika 3-osobowego lub większego.

---

### Mechanizmy Zarządzania

#### Kelner
- Zarządza operacjami w restauracji:
  - Przydziela stoliki grupom klientów.
  - Obsługuje procesy klientów.
  - Zwalnia stoliki po zakończeniu posiłków.
  - Obsługuje zdarzenia wyjątkowe, takie jak pożar.

#### Klienci
- Każda grupa klientów jest reprezentowana przez osobny proces.
- Klienci wysyłają żądanie przydziału stolika do kelnera.
- Po przydziale stolika zamawiają pizzę i zajmują miejsce, a następnie opuszczają restaurację.

---

### Mechanizmy Synchronizacji i Komunikacji

#### Pamięć Współdzielona
- Struktura `SharedData` przechowuje informacje o stolikach, klientach oraz statystykach restauracji.

#### Semafory
- Zapewniają synchronizację dostępu do pamięci współdzielonej, unikając konfliktów między procesami.

#### Kolejki Komunikatów
- Umożliwiają komunikację między procesami klientów a kelnerem.

#### Sygnały
- Obsługują zdarzenia wyjątkowe:
  - Sygnał `SIGUSR1` wywołuje zdarzenie pożaru, zmuszając restaurację do ewakuacji.

---

### Statystyki Restauracji

#### Zbierane Dane
- Liczba obsłużonych klientów.
- Liczba odrzuconych klientów.
- Łączne zarobki restauracji.
- Średni czas obsługi klienta.
- Czas trwania programu.

#### Wyświetlanie
- Po zakończeniu działania restauracji kelner wyświetla statystyki, które można również zapisać do pliku logów.

---

### Zarządzanie Wyjątkami

#### Brak Wolnych Stolików
- Klienci, którzy nie otrzymują stolika, są odrzucani, a ich procesy kończą się kontrolowanym komunikatem.

#### Pożar w Restauracji
- Wywołany sygnałem `SIGUSR1`, pożar powoduje ewakuację wszystkich klientów i zamknięcie restauracji.

#### Obsługa Błędów
- Każda funkcja systemowa (np. `shmget`, `semget`, `msgget`) jest zabezpieczona przed błędami przy użyciu `perror()`.

---

### Konfiguracja i Skalowalność

#### Parametry Konfiguracyjne
- Liczba stolików każdego rodzaju.
- Maksymalna liczba klientów.
- Zakres wielkości grup klientów.

#### Testy Skalowalności
- Program działa wydajnie przy dużej liczbie klientów (np. 100, 1000 klientów).

---

## Co udało się zrobić
- Implementacja komunikacji między procesami (pamięć współdzielona, kolejki komunikatów).
- Obsługa sygnałów do zarządzania sytuacjami awaryjnymi.
- Mechanizmy synchronizacji za pomocą semaforów.
- Dynamiczne zarządzanie zasobami stolików w restauracji.
- Obsługa wyjątków i błędów przy użyciu `perror()` i `errno`.

---

## Problemy napotkane w trakcie pracy
- Trudności w synchronizacji procesów podczas intensywnego obciążenia.
- Wyzwania w implementacji efektywnego mechanizmu ewakuacji klientów podczas pożaru.
- Integracja różnych mechanizmów IPC w spójny system.

---

## Elementy specjalne
- Implementacja zarobków restauracji w pamięci współdzielonej.
- Dynamiczne generowanie klientów o losowych rozmiarach grup.
- Kolorowe komunikaty terminalowe dla lepszej czytelności.

---

## Testy i wyniki
Testy były przeprowadzane w następujących scenariuszach:
1. Obsługa maksymalnej liczby klientów bez zakłóceń.
2. Symulacja pożaru i ewakuacja klientów.
3. Testy poprawności zarządzania zasobami stolików i pamięci współdzielonej.

Wyniki testów:
- Wszystkie scenariusze zostały pozytywnie zaliczone.
- Ewakuacja działała poprawnie przy wszystkich możliwych konfiguracjach.

---

## Kluczowe fragmenty kodu
### Tworzenie i obsługa plików
[Kod źródłowy - `customer.c`](https://github.com/user/repo/blob/main/customer.c#L12)

### Tworzenie procesów
[Kod źródłowy - `main.c`](https://github.com/user/repo/blob/main/main.c#L14)

### Synchronizacja procesów
[Kod źródłowy - `common.h`](https://github.com/user/repo/blob/main/common.h#L45)

### Obsługa sygnałów
[Kod źródłowy - `cashier.c`](https://github.com/user/repo/blob/main/cashier.c#L56)

### Komunikacja między procesami
[Kod źródłowy - `customer.c`](https://github.com/user/repo/blob/main/customer.c#L45)

---

## Aktywność na GitHub
- Repozytorium zawiera systematyczne commity dokumentujące rozwój projektu.
- [Link do repozytorium](https://github.com/Holingo/Pizzeria-Simulation)

---
