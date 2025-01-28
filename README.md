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
- Przykładowy test dla liczby `10 osób`.
https://github.com/Holingo/Pizzeria-Simulation/blob/fb58893f8f5537f8d42eed1c275fd7ae5b667cee/Logs/Log_10.txt#L59-L67

- Przykładowy test dla liczby `100 osób`.
https://github.com/Holingo/Pizzeria-Simulation/blob/fb58893f8f5537f8d42eed1c275fd7ae5b667cee/Logs/Log_100.txt#L578-L587

- Przykładowy test dla liczby `1000 osób`.
https://github.com/Holingo/Pizzeria-Simulation/blob/fb58893f8f5537f8d42eed1c275fd7ae5b667cee/Logs/Log_1000.txt#L5861-L5870

2. Symulacja pożaru i ewakuacja klientów.
```c
[Main] Uruchomiono ./cashier (PID: 236515)
[Kasjer] PID: 236515
[Main] Uruchomiono ./customer (PID: 236516)
[Kasjer] Grupa 1 (PID 236516) dostała stolik 0
[Klient PID: 236516] Zamówił: Hawajska (28 zł) | Stolik 0
...
```

```c
    PID TTY          TIME CMD
 215219 pts/0    00:00:00 bash
 236514 pts/0    00:00:00 main
 236515 pts/0    00:00:07 cashier
 236516 pts/0    00:00:00 customer
 236517 pts/0    00:00:00 customer
 236518 pts/0    00:00:00 customer
 236519 pts/0    00:00:00 ps

holingo@Oscar:~/Pizzeria-Simulation$ ./firefighter 236515

[Strażak] Wysyłam sygnał pożaru do kasjera (PID: 236515)
```

```c
[Kasjer] POŻAR! Ewakuacja wszystkich klientów!
[Main] Kasjer zakończył działanie. Przerywanie generacji klientów.

= = = Statystyki programu = = =
Czas trwania programu: 38.00 sekund
Łączna liczba klientów wygenerowanych: 3
Liczba klientów obsłużonych: 3
Liczba klientów odrzuconych: 0
Łączna liczba odebranych komunikatów: 3
Średni czas obsługi klienta: 11.50 sekund
Maksymalne użycie pamięci: 1064 KB
Łączne obciążenie CPU: 7.96 sekund
[Main] Wszystkie procesy zakończone
```

```c
    PID TTY          TIME CMD
 215219 pts/0    00:00:00 bash
 236583 pts/0    00:00:00 ps
 ```

3. Testy poprawności zarządzania zasobami stolików i pamięci współdzielonej.
```c
[Main] Uruchomiono ./cashier (PID: 236589)
[Kasjer] PID: 236589
[Main] Uruchomiono ./customer (PID: 236590)
[Klient PID: 236590] Zamówił: Pepperoni (30 zł) | Stolik 1
[Klient PID: 236590] Jem pizzę...
[Kasjer] Grupa 2 (PID 236590) dostała stolik 1
[Main] Uruchomiono ./customer (PID: 236591)
...
```

```c
    PID TTY          TIME CMD
 215219 pts/0    00:00:00 bash
 236588 pts/0    00:00:00 main
 236589 pts/0    00:00:06 cashier
 236590 pts/0    00:00:00 customer
 236591 pts/0    00:00:00 customer
 236593 pts/0    00:00:00 ps
```

```c
ipcs:
------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages
0x0000238d 98316      holingo    666        0            0
0x0000ae0c 98317      holingo    666        304          38
0x00091011 131110     holingo    666        0            0
0x5120bf3e 98354      holingo    666        192          23

------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status
0x5320bf3e 163845     holingo    666        108        0
0x000004d2 131116     holingo    666        432        0
0x00001234 163888     holingo    666        152        3

------ Semaphore Arrays --------
key        semid      owner      perms      nsems
0x0000162e 98316      holingo    666        1
0x000142d1 98322      holingo    666        1
0x00005678 131095     holingo    666        1
0x4d20bf3e 98345      holingo    666        1
```

```c
/proc/236589/status:
Name:   cashier
Umask:  0022
State:  T (stopped)
Tgid:   236589
Ngid:   0
Pid:    236589
PPid:   236588
TracerPid:      0
Uid:    1000    1000    1000    1000
Gid:    1000    1000    1000    1000
FDSize: 64
Groups: 4 20 24 25 27 29 30 44 46 100 107 1000
NStgid: 236589
NSpid:  236589
NSpgid: 236588
NSsid:  215219
VmPeak:     2684 kB
VmSize:     2684 kB
VmLck:         0 kB
VmPin:         0 kB
VmHWM:      1096 kB
VmRSS:      1096 kB
RssAnon:              92 kB
RssFile:            1004 kB
RssShmem:              0 kB
VmData:      224 kB
VmStk:       132 kB
VmExe:         4 kB
VmLib:      1748 kB
VmPTE:        44 kB
VmSwap:        0 kB
HugetlbPages:          0 kB
CoreDumping:    0
THP_enabled:    1
Threads:        1
SigQ:   2/15178
SigPnd: 0000000000000000
ShdPnd: 0000000000000000
SigBlk: 0000000000000000
SigIgn: 0000000000000000
SigCgt: 0000000000000200
CapInh: 0000000000000000
CapPrm: 0000000000000000
CapEff: 0000000000000000
CapBnd: 000001ffffffffff
CapAmb: 0000000000000000
NoNewPrivs:     0
Seccomp:        0
Seccomp_filters:        0
Speculation_Store_Bypass:       thread vulnerable
SpeculationIndirectBranch:      conditional enabled
Cpus_allowed:   3
Cpus_allowed_list:      0-1
Mems_allowed:   1
Mems_allowed_list:      0
voluntary_ctxt_switches:        2
nonvoluntary_ctxt_switches:     36
```

Wyniki testów:
- Wszystkie scenariusze zostały pozytywnie zaliczone.
- Ewakuacja działała poprawnie przy wszystkich możliwych konfiguracjach.

---

## Kluczowe fragmenty kodu
### Tworzenie i obsługa plików - `fopen()` `fclose(),`
[Kod źródłowy - `customer.c`](https://github.com/Holingo/Pizzeria-Simulation/blob/f6b261bbb2f393d427a464e319a31ccde51b4dfa/customer.c#L12-L25)
https://github.com/Holingo/Pizzeria-Simulation/blob/f6b261bbb2f393d427a464e319a31ccde51b4dfa/customer.c#L12-L25

### Tworzenie procesów - `fork()` `ecec()` `exit()` `wait()`
[Kod źródłowy - `main.c`](https://github.com/Holingo/Pizzeria-Simulation/blob/f6b261bbb2f393d427a464e319a31ccde51b4dfa/main.c#L3-L23)
https://github.com/Holingo/Pizzeria-Simulation/blob/f6b261bbb2f393d427a464e319a31ccde51b4dfa/main.c#L3-L23

### Synchronizacja procesów - `semop()` `semget` `semctl`
[Kod źródłowy - `common.h`](https://github.com/Holingo/Pizzeria-Simulation/blob/f6b261bbb2f393d427a464e319a31ccde51b4dfa/common.h#L93-L101)
https://github.com/Holingo/Pizzeria-Simulation/blob/f6b261bbb2f393d427a464e319a31ccde51b4dfa/common.h#L93-L101
https://github.com/Holingo/Pizzeria-Simulation/blob/f6b261bbb2f393d427a464e319a31ccde51b4dfa/cashier.c#L69
https://github.com/Holingo/Pizzeria-Simulation/blob/f6b261bbb2f393d427a464e319a31ccde51b4dfa/cashier.c#L10-L16

### Obsługa sygnałów - `signal()` `kill`
[Kod źródłowy - `cashier.c`](https://github.com/Holingo/Pizzeria-Simulation/blob/f6b261bbb2f393d427a464e319a31ccde51b4dfa/cashier.c#L122)
https://github.com/Holingo/Pizzeria-Simulation/blob/f6b261bbb2f393d427a464e319a31ccde51b4dfa/cashier.c#L122
https://github.com/Holingo/Pizzeria-Simulation/blob/f6b261bbb2f393d427a464e319a31ccde51b4dfa/cashier.c#L25

# Segmenty pamięci dzielonej  - `shmget()` `shmat()` `shmdt()` `shmctl()`
[Kod źródłowy - `customer.c`](https://github.com/Holingo/Pizzeria-Simulation/blob/f6b261bbb2f393d427a464e319a31ccde51b4dfa/customer.c#L62-L64)
https://github.com/Holingo/Pizzeria-Simulation/blob/f6b261bbb2f393d427a464e319a31ccde51b4dfa/customer.c#L62-L64
https://github.com/Holingo/Pizzeria-Simulation/blob/f6b261bbb2f393d427a464e319a31ccde51b4dfa/customer.c#L51
https://github.com/Holingo/Pizzeria-Simulation/blob/f6b261bbb2f393d427a464e319a31ccde51b4dfa/common.h#L63-L72

### Komunikacja między procesami - `msgget` `msgsnd` `msgrcv`
[Kod źródłowy - `customer.c`](https://github.com/Holingo/Pizzeria-Simulation/blob/f6b261bbb2f393d427a464e319a31ccde51b4dfa/customer.c#L71-L78)
https://github.com/Holingo/Pizzeria-Simulation/blob/f6b261bbb2f393d427a464e319a31ccde51b4dfa/customer.c#L71-L78
https://github.com/Holingo/Pizzeria-Simulation/blob/f6b261bbb2f393d427a464e319a31ccde51b4dfa/cashier.c#L74
https://github.com/Holingo/Pizzeria-Simulation/blob/f6b261bbb2f393d427a464e319a31ccde51b4dfa/cashier.c#L128-L133

---

## Aktywność na GitHub
- Repozytorium zawiera systematyczne commity dokumentujące rozwój projektu.
- [Link do repozytorium](https://github.com/Holingo/Pizzeria-Simulation)

---
