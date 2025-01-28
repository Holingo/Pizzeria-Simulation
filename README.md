# Projekt: Symulacja Restauracji z Obsługą Wieloprocesową

## Założenia projektowe
Projekt symuluje działanie restauracji (pizzerii) z wykorzystaniem mechanizmów wieloprocesowych. System obsługuje klientów, zarządza stolikami oraz obsługuje sytuacje awaryjne, takie jak pożar. Procesy i komunikacja między nimi realizowane są za pomocą pamięci współdzielonej, kolejek komunikatów, semaforów oraz sygnałów.

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
